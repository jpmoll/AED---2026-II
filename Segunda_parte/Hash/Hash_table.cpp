#include <iostream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <cassert>
using namespace std;

/*
 * TABLA HASH con encadenamiento (separate chaining), sin contenedores de la STL.
 *
 *   table (arreglo de punteros, tamaño 2^p)
 *   +-----+
 *   | [0] | --> null
 *   | [1] | --> (k=17, v) --> (k=49, v) --> null      <- colisión: 2 claves, mismo casillero
 *   | [2] | --> null
 *   | [3] | --> (k=5, v) --> null
 *   | ... |
 *   +-----+
 *
 * Cada casillero es el inicio de una lista enlazada. Buscar = calcular el casillero y recorrer
 * SU lista (corta si la dispersión es buena).
 *
 * ========================================================================================
 * LA FÓRMULA DE DISPERSIÓN: hashing multiplicativo (Knuth / "Fibonacci hashing")
 *
 *     indice = ( k * A  mod 2^64 )  >>  (64 - p)        con  A = 2^64 / phi = 0x9E3779B97F4A7C15
 *
 *     phi = (1 + raiz(5)) / 2 = 1.618...   (razón áurea),   1/phi = 0.6180339887...
 *     Es la versión entera de:  indice = floor( m * frac( k * 0.6180339887 ) )
 *
 * ¿Por qué es buena?
 *  1) Claves consecutivas caen LEJOS entre sí. Multiplicar por la razón áurea reparte los
 *     puntos k*A casi como el mejor reparto posible (cada clave nueva cae en uno de los
 *     huecos más grandes que quedan).
 *
 *        claves 0,1,2,3,4,5,6,7  con 8 casilleros:   0 4 1 6 3 0 5 2
 *        (cada clave salta ~0.618 de la tabla respecto a la anterior; no se amontonan)
 *        (con 8 claves y 8 casilleros aún hay alguna colisión: 0 y 5; es normal)
 *
 *  2) Se usan los bits ALTOS del producto (>> (64-p)). En una multiplicación, los bits bajos
 *     del resultado dependen solo de los bits bajos de la clave, pero los altos dependen de
 *     TODOS. Por eso el módulo clásico  k % 8  es malo: solo mira los 3 bits bajos.
 *
 *        claves 0, 8, 16, 24, 32, ...  con  k % 8  -> TODAS al casillero 0  (una lista enorme)
 *        con Fibonacci                       -> 0 7 7 6 6 5 5 4  (repartidas, ver main)
 *        y con una tabla grande el reparto es mucho mejor (ver prueba 6 del main)
 *
 *  3) El tamaño es potencia de 2 (2^p), así no hace falta un primo ni una división.
 * ========================================================================================
 */

// ---------- Functor: convierte la CLAVE en un entero de 64 bits ----------
// Después la tabla aplica la dispersión multiplicativa sobre ese entero.
template <typename K>
struct Hash {
    uint64_t operator()(const K& k) const { return (uint64_t)k; }     // enteros
};

// Para strings: h = h*31 + c   (hash polinomial; cada letra pesa distinto según su posición)
//   "ab" -> 'a'*31 + 'b'      "ba" -> 'b'*31 + 'a'      (distintos, aunque tengan las mismas letras)
template <>
struct Hash<string> {
    uint64_t operator()(const string& s) const {
        uint64_t h = 0;
        for (char c : s) h = h * 31 + (unsigned char)c;
        return h;
    }
};

// La dispersión de Fibonacci sobre un entero de 64 bits, para una tabla de 2^p casilleros
inline uint64_t fibIndex(uint64_t h, int p) {
    return (h * 0x9E3779B97F4A7C15ull) >> (64 - p);
}

template <typename K, typename V>
struct Node {
    K key;
    V val;
    Node* next;
    Node(const K& k, const V& v) : key(k), val(v), next(nullptr) {}
};

template <typename K, typename V, typename H = Hash<K>>
class HashTable {
    Node<K, V>** table;   // arreglo de punteros al primer nodo de cada lista
    int p;                // la tabla tiene 2^p casilleros
    size_t cap;           // 2^p
    size_t n;             // cantidad de elementos
    H hash;               // functor clave -> entero

    size_t index(const K& k) const { return fibIndex(hash(k), p); }

    /*
     * FIND: igual que el del BST. Deja p apuntando al PUNTERO que apunta al nodo
     * (o al final de la lista, si no está).
     *
     *   buscar 49 en el casillero [1]:
     *
     *   table[1] --> (17) --> (49) --> null
     *      ^           ^
     *      p empieza   p avanza (17 != 49)  ... p = &((17)->next), *p es el nodo 49 -> true
     *
     *   buscar 33:  p avanza hasta el final:  *p == nullptr  -> false
     *   y p queda justo en el hueco donde se enganchará el nodo nuevo.
     */
    bool find(const K& k, Node<K, V>**& pp) const {
        pp = &table[index(k)];
        while (*pp && !((*pp)->key == k))
            pp = &((*pp)->next);
        return *pp != nullptr;
    }

    /*
     * REHASH: cuando la tabla se llena (factor de carga > 3/4) se duplica (p -> p+1).
     * NO se crean nodos nuevos: se RE-ENGANCHAN los mismos nodos en la tabla nueva,
     * porque el casillero depende del tamaño y cambia para casi todos.
     *
     *   antes (p=3, 8 casilleros)           después (p=4, 16 casilleros)
     *   [1] --> (17) --> (49)               [x] --> (17)         [y] --> (49)
     *
     *   Por cada nodo: guardar next, calcular su casillero nuevo, enlazarlo al frente.
     */
    void rehash() {
        Node<K, V>** viejo = table;
        size_t capVieja = cap;

        p++;
        cap = (size_t)1 << p;
        table = new Node<K, V>*[cap]();            // () -> todos en nullptr

        for (size_t i = 0; i < capVieja; i++) {
            Node<K, V>* t = viejo[i];
            while (t) {
                Node<K, V>* sig = t->next;         // guardar ANTES de tocar t->next
                size_t j = index(t->key);          // índice con el p nuevo
                t->next = table[j];
                table[j] = t;
                t = sig;
            }
        }
        delete[] viejo;
    }

public:
    HashTable(int p0 = 3) : p(p0), cap((size_t)1 << p0), n(0) {
        assert(p0 >= 1 && p0 < 63);
        table = new Node<K, V>*[cap]();
    }

    ~HashTable() {
        for (size_t i = 0; i < cap; i++) {
            Node<K, V>* t = table[i];
            while (t) {
                Node<K, V>* sig = t->next;
                delete t;
                t = sig;
            }
        }
        delete[] table;
    }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    size_t size() const { return n; }
    size_t capacity() const { return cap; }
    double loadFactor() const { return (double)n / (double)cap; }

    // INSERT / UPDATE. Devuelve true si insertó una clave nueva, false si solo actualizó.
    bool put(const K& k, const V& v) {
        Node<K, V>** pp;
        if (find(k, pp)) {
            (*pp)->val = v;                        // ya existía: actualizar
            return false;
        }
        if (n + 1 > cap * 3 / 4) {                 // factor de carga > 0.75: crecer
            rehash();
            find(k, pp);                           // la tabla cambió: pp ya no sirve, buscar de nuevo
        }
        *pp = new Node<K, V>(k, v);                // engancharlo donde dejó pp
        n++;
        return true;
    }

    // Si está, copia el valor en 'out' y devuelve true.
    bool get(const K& k, V& out) const {
        Node<K, V>** pp;
        if (!find(k, pp)) return false;
        out = (*pp)->val;
        return true;
    }

    bool contains(const K& k) const {
        Node<K, V>** pp;
        return find(k, pp);
    }

    /*
     * REMOVE: un solo caso (a diferencia del BST). Saltar el nodo y borrarlo.
     *
     *   table[1] --> (17) --> [49] --> (65) --> null        quitar 49
     *                  ^pp
     *   *pp = t->next   =>   (17) --> (65)
     *
     * Si 49 era el primero de la lista, pp es &table[1] y funciona igual (sin caso especial).
     */
    bool rem(const K& k) {
        Node<K, V>** pp;
        if (!find(k, pp)) return false;
        Node<K, V>* t = *pp;
        *pp = t->next;
        delete t;
        n--;
        return true;
    }

    // Muestra los casilleros con sus listas (solo para tablas chicas)
    void print() const {
        for (size_t i = 0; i < cap; i++) {
            cout << "  [" << i << "] -> ";
            for (Node<K, V>* t = table[i]; t; t = t->next)
                cout << "(" << t->key << ":" << t->val << ") -> ";
            cout << "null\n";
        }
    }

    // Lista más larga: mide la calidad de la dispersión (ideal: muy corta)
    size_t maxChain() const {
        size_t mx = 0;
        for (size_t i = 0; i < cap; i++) {
            size_t c = 0;
            for (Node<K, V>* t = table[i]; t; t = t->next) c++;
            if (c > mx) mx = c;
        }
        return mx;
    }

    // Invariantes: cada nodo está en SU casillero, sin claves repetidas, cuenta == n
    bool verificar() const {
        size_t total = 0;
        for (size_t i = 0; i < cap; i++)
            for (Node<K, V>* t = table[i]; t; t = t->next) {
                if (index(t->key) != i) return false;
                for (Node<K, V>* u = t->next; u; u = u->next)
                    if (u->key == t->key) return false;
                total++;
            }
        return total == n;
    }
};

int main() {
    // ---------------------------------------------------------------
    // 1) Por qué Fibonacci y no k % m
    // ---------------------------------------------------------------
    cout << "=== 1) Dispersión: k % 8  vs  Fibonacci (8 casilleros, p=3) ===\n";
    cout << "claves consecutivas 0..7:\n  k % 8     : ";
    for (uint64_t k = 0; k < 8; k++) cout << k % 8 << " ";
    cout << "\n  Fibonacci : ";
    for (uint64_t k = 0; k < 8; k++) cout << fibIndex(k, 3) << " ";

    cout << "\nclaves múltiplos de 8 (0, 8, 16, ..., 56):\n  k % 8     : ";
    for (uint64_t k = 0; k < 64; k += 8) cout << k % 8 << " ";
    cout << "   <- todas al mismo casillero\n  Fibonacci : ";
    for (uint64_t k = 0; k < 64; k += 8) cout << fibIndex(k, 3) << " ";
    cout << "\n\n";

    // ---------------------------------------------------------------
    // 2) Insertar y ver los casilleros
    // ---------------------------------------------------------------
    cout << "=== 2) Tabla int -> int (8 casilleros; 33 y 12 colisionan en el [3]) ===\n";
    HashTable<int, int> t;
    for (int k : {17, 49, 5, 33, 65, 12})
        t.put(k, k * 10);
    t.print();
    cout << "size = " << t.size() << ", capacity = " << t.capacity()
         << ", carga = " << t.loadFactor() << "\n\n";

    // ---------------------------------------------------------------
    // 3) Rehash: al pasar 3/4 de carga, la tabla se duplica
    // ---------------------------------------------------------------
    cout << "=== 3) Crecimiento (rehash) ===\n";
    size_t capAnt = t.capacity();
    for (int k = 100; k < 140; k++) {
        t.put(k, k);
        if (t.capacity() != capAnt) {
            cout << "  con " << t.size() << " elementos: " << capAnt << " -> "
                 << t.capacity() << " casilleros\n";
            capAnt = t.capacity();
        }
    }
    cout << "verificar = " << boolalpha << t.verificar() << "\n\n";

    // ---------------------------------------------------------------
    // 4) get / update / remove
    // ---------------------------------------------------------------
    cout << "=== 4) get, update, remove ===\n";
    int v;
    cout << "get(49) = " << (t.get(49, v) ? to_string(v) : "no está") << "\n";
    cout << "put(49, 999) devuelve " << t.put(49, 999) << " (false = solo actualizó)\n";
    cout << "get(49) = " << (t.get(49, v) ? to_string(v) : "no está") << "\n";
    cout << "rem(49) = " << t.rem(49) << ", contains(49) = " << t.contains(49) << "\n";
    cout << "rem(49) otra vez = " << t.rem(49) << "\n";
    cout << "get(31337) = " << (t.get(31337, v) ? to_string(v) : "no está") << "\n\n";

    // ---------------------------------------------------------------
    // 5) Claves string (usa el functor Hash<string>)
    // ---------------------------------------------------------------
    cout << "=== 5) string -> int (contar palabras) ===\n";
    HashTable<string, int> palabras;
    const char* texto[] = {"hola", "mundo", "hola", "heap", "avl", "hash", "heap", "hola"};
    for (const char* w : texto) {
        int c = 0;
        palabras.get(w, c);
        palabras.put(w, c + 1);
    }
    palabras.print();
    cout << "\n";

    // ---------------------------------------------------------------
    // 6) Calidad de la dispersión con muchos datos
    // ---------------------------------------------------------------
    cout << "=== 6) Calidad: lista más larga con 200000 claves ===\n";
    {
        HashTable<uint64_t, int> a, b;
        for (uint64_t k = 0; k < 200000; k++) a.put(k, 0);                 // consecutivas
        for (uint64_t k = 0; k < 200000; k++) b.put(k * 1024, 0);          // múltiplos de 1024 (patrón malo para %)
        cout << "  consecutivas : cap=" << a.capacity() << " carga=" << a.loadFactor()
             << " lista más larga=" << a.maxChain() << "\n";
        cout << "  de a 1024    : cap=" << b.capacity() << " carga=" << b.loadFactor()
             << " lista más larga=" << b.maxChain() << "\n\n";
    }

    // ---------------------------------------------------------------
    // 7) Prueba aleatoria contra arreglos simples
    // ---------------------------------------------------------------
    cout << "=== 7) Prueba aleatoria ===\n";
    const int M = 5000;
    static bool hay[M];
    static int val[M];
    HashTable<int, int> h;
    srand(1234);
    for (int i = 0; i < 100000; i++) {
        int k = rand() % M;
        int op = rand() % 3;
        if (op == 0) {                                   // put
            int x = rand();
            bool nueva = h.put(k, x);
            assert(nueva == !hay[k]);
            hay[k] = true; val[k] = x;
        } else if (op == 1) {                            // rem
            assert(h.rem(k) == hay[k]);
            hay[k] = false;
        } else {                                         // get
            int out = -1;
            assert(h.get(k, out) == hay[k]);
            if (hay[k]) assert(out == val[k]);
        }
    }
    int cuenta = 0;
    for (int k = 0; k < M; k++) cuenta += hay[k];
    assert(h.size() == (size_t)cuenta && h.verificar());
    cout << "OK: " << cuenta << " elementos coinciden con la referencia (cap=" << h.capacity() << ")\n";
    return 0;
}
