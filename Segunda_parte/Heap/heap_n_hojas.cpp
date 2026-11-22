#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <cassert>
using namespace std;

// comp(a, b) == true  significa "a debe estar MÁS ARRIBA que b"
template <typename T>
struct Min {#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <cassert>
using namespace std;

// comp(a, b) == true  significa "a debe estar MÁS ARRIBA que b"
//
//        Min-heap            Max-heap
//           11                  68
//          /  \                /  \
//        42    57            58    61
//
template <typename T>
struct Min {
    bool operator()(const T& a, const T& b) const { return a < b; }
};

template <typename T>
struct Max {
    bool operator()(const T& a, const T& b) const { return a > b; }
};

// Nodo con d hijos (en vez de left/right)
//
//   d = 3:
//      +--------------------------+
//      | value                    |
//      | child = [ c0 | c1 | c2 ] |   cada c es un Node* (nullptr si no hay hijo)
//      +--------------------------+
//                 |    |    |
//                 v    v    v
//              hijo0 hijo1 hijo2
//
//   (en el heap binario eran "left" y "right"; ahora son child[0] y child[1])
//
template <typename T>
struct Node {
    T value;
    vector<Node*> child;
    Node(const T& v, size_t d) : value(v), child(d, nullptr) {}
};

// Heap d-ario con punteros. d = número de ramas (d=2 es el heap binario normal).
//
// Cómo cambia la forma del árbol según d (con n = 13 nodos):
//
//   d = 2:  1 | 2 | 4 | 6    -> 4 niveles (alto, flaco)
//   d = 3:  1 | 3 | 9        -> 3 niveles
//   d = 4:  1 | 4 | 8        -> 3 niveles
//
//   Más ramas = árbol más bajo:
//     - ins() sube menos niveles (menos swaps)
//     - rem() baja menos niveles, pero en cada uno compara d hijos
//
template <typename T, typename Comp = Min<T>>
class DHeap {
    Node<T>* root;
    size_t n;      // cantidad de nodos
    size_t d;      // ramas por nodo
    Comp comp;

    // ------------------------------------------------------------------
    // Posiciones 0-based por niveles: raíz = 0.
    //   hijos de j:   d*j+1, d*j+2, ..., d*j+d
    //   padre de j:   (j-1)/d
    //   j es el hijo número (j-1)%d de su padre
    //
    // Numeración con d = 3 (cada nodo tiene 3 hijos):
    //
    //                          0
    //               /          |          \
    //             1            2            3
    //           / | \        / | \        / | \
    //          4  5  6      7  8  9     10 11 12
    //
    //   hijos de 0 = 3*0+1 .. 3*0+3 = 1,2,3
    //   hijos de 3 = 3*3+1 .. 3*3+3 = 10,11,12
    //
    // Para llegar a j: subimos hasta la raíz anotando "qué hijo soy" y luego
    // recorremos al revés.
    //
    // Ejemplo d = 3, j = 10:
    //
    //   j = 10:  (10-1)%3 = 0  -> soy el hijo 0     ;  padre = (10-1)/3 = 3
    //   j =  3:  ( 3-1)%3 = 2  -> soy el hijo 2     ;  padre = ( 3-1)/3 = 0
    //   j =  0:  llegamos a la raíz, paramos
    //
    //   r (subiendo)  = [0, 2]
    //   r (reverse)   = [2, 0]     <- el camino de bajada
    //
    //                  0              raíz
    //                  | child[2]
    //                  v
    //                  3
    //                  | child[0]
    //                  v
    //                 10              <- posición j
    //
    // Ejemplo d = 2, j = 5 (heap binario):  (5-1)%2=0, padre 2;  (2-1)%2=1, padre 0
    //   r = [0,1] -> reverse [1,0] -> derecha, izquierda  (igual que en el heap binario)
    // ------------------------------------------------------------------
    vector<size_t> route(size_t j) const {
        vector<size_t> r;
        while (j > 0) {
            r.push_back((j - 1) % d);
            j = (j - 1) / d;
        }
        reverse(r.begin(), r.end());
        return r;
    }

    // ------------------------------------------------------------------
    // Igual que antes: dirección del puntero que apunta a la posición j (nodo o hueco)
    //
    // Ejemplo d = 3, j = 8  ->  route(8) = [1, 1]
    //
    //                  0               p = &root
    //          /       |       \
    //         1       [2]       3      p = &(root->child[1])
    //                / | \
    //               7 [8]  9           p = &(nodo2->child[1])   <- slot(8)
    //
    // Devuelve Node** para poder ESCRIBIR en el hueco:
    //     *slot(n) = new Node(x, d);
    // ------------------------------------------------------------------
    Node<T>** slot(size_t j) {
        Node<T>** p = &root;
        for (size_t c : route(j))
            p = &((*p)->child[c]);
        return p;
    }

    // ------------------------------------------------------------------
    // Nodos desde la raíz hasta la posición j (inclusive)
    //
    // Ejemplo d = 3, j = 10  ->  v = [ nodo0, nodo3, nodo10 ]
    //
    //                  (0)  <- v[0]
    //          /        |        \
    //         1         2        (3)  <- v[1]
    //                           / | \
    //                       ((10)) 11 12   <- v[2]
    //
    // El padre de v[i] es v[i-1].
    // ------------------------------------------------------------------
    vector<Node<T>*> path(size_t j) {
        vector<Node<T>*> v;
        Node<T>* cur = root;
        v.push_back(cur);
        for (size_t c : route(j)) {
            cur = cur->child[c];
            v.push_back(cur);
        }
        return v;
    }

    // Postorden: primero se borran los d hijos, después el nodo.
    //
    //          A
    //        / | \          orden: C, D, E, B, F, G, A
    //       B  F  G
    //     / | \
    //    C  D  E
    void destroy(Node<T>* t) {
        if (!t) return;
        for (Node<T>* c : t->child) destroy(c);
        delete t;
    }

public:
    DHeap(size_t ramas = 2) : root(nullptr), n(0), d(ramas) { assert(d >= 2); }
    ~DHeap() { destroy(root); }
    DHeap(const DHeap&) = delete;
    DHeap& operator=(const DHeap&) = delete;

    bool empty() const { return n == 0; }
    size_t size() const { return n; }
    const T& top() const { return root->value; }  // precondición: !empty()

    // ------------------------------------------------------------------
    // INSERT: pegar en la posición n (primer hueco) y subir mientras gane a su padre
    //
    // Ejemplo (min-heap, d = 3): ya hay 10 nodos y insertamos 5.
    //
    // Paso 1: slot(n) = slot(10) -> child[0] del nodo 3 (el 42). Pegamos 5 ahí.
    //
    //                          11
    //               /          |          \
    //             44          20          42
    //           / | \       / | \        /
    //         55 57 68    61 58 70     [5]       <- pth = [11, 42, 5]
    //
    // Paso 2: ¿5 < 42? Sí -> swap
    //
    //                          11
    //               /          |          \
    //             44          20          [5]
    //           / | \       / | \        /
    //         55 57 68    61 58 70      42
    //
    // Paso 3: ¿5 < 11? Sí -> swap
    //
    //                         [5]
    //               /          |          \
    //             44          20          11
    //           / | \       / | \        /
    //         55 57 68    61 58 70      42
    //
    // Llegamos a la raíz (i == 0), el for termina.
    // Si comp(hijo, padre) es false en algún paso -> break (ya está en su lugar).
    // ------------------------------------------------------------------
    void ins(const T& x) {
        *slot(n) = new Node<T>(x, d);
        n++;

        vector<Node<T>*> pth = path(n - 1);
        for (size_t i = pth.size() - 1; i > 0; i--) {
            if (comp(pth[i]->value, pth[i - 1]->value))
                swap(pth[i]->value, pth[i - 1]->value);
            else
                break;
        }
    }

    // ------------------------------------------------------------------
    // REMOVE (raíz): mover el último valor a la raíz, borrar el último nodo y bajar
    //
    // Ejemplo (min-heap, d = 3) con n = 13:
    //
    //   Estado inicial:
    //                          5                  <- se extrae este
    //               /          |          \
    //             44          20          11
    //           / | \       / | \       / | \
    //         55 57 68    61 58 70    42 33 90    <- 90 es el último (posición 12)
    //
    //   Paso 1 y 2: copiar 90 a la raíz y borrar el último nodo
    //
    //                        [90]                 <- el 5 desaparece
    //               /          |          \
    //             44          20          11
    //           / | \       / | \       / |
    //         55 57 68    61 58 70    42 33 (x)   <- (x) = nullptr
    //
    //   Paso 3a: ¿cuál es el MEJOR de los 3 hijos de 90? Se recorren en orden:
    //
    //              best = 44
    //              20 le gana a 44?  sí -> best = 20
    //              11 le gana a 20?  sí -> best = 11     (ganador: 11)
    //
    //            ¿11 le gana a 90? Sí -> swap, seguimos bajando
    //
    //                         11
    //               /          |          \
    //             44          20         [90]
    //           / | \       / | \       / |
    //         55 57 68    61 58 70    42 33
    //
    //   Paso 3b: hijos de 90: 42, 33 y un hueco (nullptr, se ignora con "if (c ...)")
    //
    //              best = 42
    //              33 le gana a 42?  sí -> best = 33
    //
    //            ¿33 le gana a 90? Sí -> swap
    //
    //                         11
    //               /          |          \
    //             44          20          33
    //           / | \       / | \       / |
    //         55 57 68    61 58 70    42 [90]
    //
    //   Paso 3c: 90 ya no tiene hijos (best == nullptr) -> break. Listo.
    //
    //   Diferencia con el heap binario: en vez de comparar 2 hijos por nivel,
    //   aquí se comparan hasta d hijos (el for con "best").
    // ------------------------------------------------------------------
    bool rem() {
        if (empty()) return false;

        Node<T>** last = slot(n - 1);
        root->value = (*last)->value;
        delete *last;
        *last = nullptr;
        n--;

        Node<T>* cur = root;
        while (cur) {
            // el "mejor" de los d hijos según el functor
            Node<T>* best = nullptr;
            for (Node<T>* c : cur->child)
                if (c && (!best || comp(c->value, best->value)))
                    best = c;

            if (best && comp(best->value, cur->value)) {
                swap(best->value, cur->value);
                cur = best;
            } else
                break;
        }
        return true;
    }

    // ------------------------------------------------------------------
    // Por niveles (aquí sí se usa cola, solo para imprimir)
    //
    // Cada nivel tiene d veces más nodos que el anterior:
    //
    //   d = 3:   nivel 0 -> 1 nodo
    //            nivel 1 -> 3 nodos
    //            nivel 2 -> 9 nodos ...
    //
    //   por eso "level *= d" (en el heap binario era "level *= 2").
    //
    // Ejemplo d = 3 con el heap del insert:
    //
    //                          5
    //               /          |          \
    //             44          20          11
    //           / | \       / | \       / | \
    //         55 57 68    61 58 70    42 33 90
    //
    //   Salida:  5 | 44 20 11 | 55 57 68 61 58 70 42 33 90 |
    // ------------------------------------------------------------------
    void print() {
        if (!root) { cout << "(vacío)" << endl; return; }
        queue<Node<T>*> q;
        q.push(root);
        size_t level = 1, count = 0;
        while (!q.empty()) {
            Node<T>* t = q.front(); q.pop();
            cout << t->value << " ";
            for (Node<T>* c : t->child)
                if (c) q.push(c);
            if (++count == level) { cout << "| "; level *= d; count = 0; }
        }
        cout << endl;
    }
};

// ------------------------------------------------------------------
// Prueba: para varios d, min y max deben extraer en orden
//
//   1) insertar 500 números al azar en el heap y en un vector "ref"
//   2) ordenar ref con cmp (less -> ascendente, greater -> descendente)
//   3) comparar: cada top() del heap debe ser igual al siguiente de ref
//
//        heap (top)      ref ordenado
//           11     ==       11   ok
//           42     ==       42   ok
//           44     ==       44   ok   ... x500
// ------------------------------------------------------------------
template <typename Comp, typename Cmp>
void test(size_t d, Cmp cmp) {
    DHeap<int, Comp> h(d);
    vector<int> ref;
    for (int i = 0; i < 500; i++) {
        int x = rand() % 1000;
        h.ins(x);
        ref.push_back(x);
    }
    sort(ref.begin(), ref.end(), cmp);
    for (int x : ref) {
        assert(h.top() == x);
        assert(h.rem());
    }
    assert(h.empty() && !h.rem());
}

int main() {
    int datos[] = {55, 42, 61, 11, 44, 57, 68, 58, 20, 70, 5, 33, 90};

    for (size_t d : {2, 3, 4}) {
        DHeap<int, Min<int>> h(d);
        for (int x : datos) h.ins(x);
        cout << "Min-heap d=" << d << ": ";
        h.print();
    }

    DHeap<int, Max<int>> m(3);
    for (int x : datos) m.ins(x);
    cout << "Max-heap d=3: ";
    m.print();

    cout << "Extrayendo max-heap d=3: ";
    while (!m.empty()) { cout << m.top() << " "; m.rem(); }
    cout << endl;

    for (size_t d = 2; d <= 6; d++) {
        test<Min<int>>(d, less<int>());
        test<Max<int>>(d, greater<int>());
    }
    cout << "Pruebas aleatorias OK (d = 2..6, min y max)" << endl;
    return 0;
}
    bool operator()(const T& a, const T& b) const { return a < b; }
};

template <typename T>
struct Max {
    bool operator()(const T& a, const T& b) const { return a > b; }
};

// Nodo con d hijos (en vez de left/right)
template <typename T>
struct Node {
    T value;
    vector<Node*> child;
    Node(const T& v, size_t d) : value(v), child(d, nullptr) {}
};

// Heap d-ario con punteros. d = número de ramas (d=2 es el heap binario normal).
template <typename T, typename Comp = Min<T>>
class DHeap {
    Node<T>* root;
    size_t n;      // cantidad de nodos
    size_t d;      // ramas por nodo
    Comp comp;

    // Posiciones 0-based por niveles: raíz = 0.
    //   hijos de j:   d*j+1, d*j+2, ..., d*j+d
    //   padre de j:   (j-1)/d
    //   j es el hijo número (j-1)%d de su padre
    // Para llegar a j: subimos hasta la raíz anotando "qué hijo soy" y luego recorremos al revés.
    vector<size_t> route(size_t j) const {
        vector<size_t> r;
        while (j > 0) {
            r.push_back((j - 1) % d);
            j = (j - 1) / d;
        }
        reverse(r.begin(), r.end());
        return r;
    }

    // Igual que antes: dirección del puntero que apunta a la posición j (nodo o hueco)
    Node<T>** slot(size_t j) {
        Node<T>** p = &root;
        for (size_t c : route(j))
            p = &((*p)->child[c]);
        return p;
    }

    // Nodos desde la raíz hasta la posición j (inclusive)
    vector<Node<T>*> path(size_t j) {
        vector<Node<T>*> v;
        Node<T>* cur = root;
        v.push_back(cur);
        for (size_t c : route(j)) {
            cur = cur->child[c];
            v.push_back(cur);
        }
        return v;
    }

    void destroy(Node<T>* t) {
        if (!t) return;
        for (Node<T>* c : t->child) destroy(c);
        delete t;
    }

public:
    DHeap(size_t ramas = 2) : root(nullptr), n(0), d(ramas) { assert(d >= 2); }
    ~DHeap() { destroy(root); }
    DHeap(const DHeap&) = delete;
    DHeap& operator=(const DHeap&) = delete;

    bool empty() const { return n == 0; }
    size_t size() const { return n; }
    const T& top() const { return root->value; }  // precondición: !empty()

    // INSERT: pegar en la posición n (primer hueco) y subir mientras gane a su padre
    void ins(const T& x) {
        *slot(n) = new Node<T>(x, d);
        n++;

        vector<Node<T>*> pth = path(n - 1);
        for (size_t i = pth.size() - 1; i > 0; i--) {
            if (comp(pth[i]->value, pth[i - 1]->value))
                swap(pth[i]->value, pth[i - 1]->value);
            else
                break;
        }
    }

    // REMOVE (raíz): mover el último valor a la raíz, borrar el último nodo y bajar
    bool rem() {
        if (empty()) return false;

        Node<T>** last = slot(n - 1);
        root->value = (*last)->value;
        delete *last;
        *last = nullptr;
        n--;

        Node<T>* cur = root;
        while (cur) {
            // el "mejor" de los d hijos según el functor
            Node<T>* best = nullptr;
            for (Node<T>* c : cur->child)
                if (c && (!best || comp(c->value, best->value)))
                    best = c;

            if (best && comp(best->value, cur->value)) {
                swap(best->value, cur->value);
                cur = best;
            } else
                break;
        }
        return true;
    }

    // Por niveles (aquí sí se usa cola, solo para imprimir)
    void print() {
        if (!root) { cout << "(vacío)" << endl; return; }
        queue<Node<T>*> q;
        q.push(root);
        size_t level = 1, count = 0;
        while (!q.empty()) {
            Node<T>* t = q.front(); q.pop();
            cout << t->value << " ";
            for (Node<T>* c : t->child)
                if (c) q.push(c);
            if (++count == level) { cout << "| "; level *= d; count = 0; }
        }
        cout << endl;
    }
};

// Prueba: para varios d, min y max deben extraer en orden
template <typename Comp, typename Cmp>
void test(size_t d, Cmp cmp) {
    DHeap<int, Comp> h(d);
    vector<int> ref;
    for (int i = 0; i < 500; i++) {
        int x = rand() % 1000;
        h.ins(x);
        ref.push_back(x);
    }
    sort(ref.begin(), ref.end(), cmp);
    for (int x : ref) {
        assert(h.top() == x);
        assert(h.rem());
    }
    assert(h.empty() && !h.rem());
}

int main() {
    int datos[] = {55, 42, 61, 11, 44, 57, 68, 58, 20, 70, 5, 33, 90};

    for (size_t d : {2, 3, 4}) {
        DHeap<int, Min<int>> h(d);
        for (int x : datos) h.ins(x);
        cout << "Min-heap d=" << d << ": ";
        h.print();
    }

    DHeap<int, Max<int>> m(3);
    for (int x : datos) m.ins(x);
    cout << "Max-heap d=3: ";
    m.print();

    cout << "Extrayendo max-heap d=3: ";
    while (!m.empty()) { cout << m.top() << " "; m.rem(); }
    cout << endl;

    for (size_t d = 2; d <= 6; d++) {
        test<Min<int>>(d, less<int>());
        test<Max<int>>(d, greater<int>());
    }
    cout << "Pruebas aleatorias OK (d = 2..6, min y max)" << endl;
    return 0;
}
