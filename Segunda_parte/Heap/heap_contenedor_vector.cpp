#include <iostream>
#include <vector>
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

// Heap d-ario sobre un arreglo. d = 2 es el heap binario clásico.
//
// IDEA CLAVE: no hay nodos ni punteros. El árbol es solo una forma de LEER el
// arreglo: los índices 0,1,2,3... se acomodan por niveles, de izquierda a derecha.
//
//   Arreglo:   índice:  0   1   2   3   4   5   6   7
//              valor:  11  42  57  55  44  61  68  58
//
//   Árbol que "ve" ese arreglo (d = 2):
//
//                    11  (0)
//                 /          \
//             42  (1)        57  (2)
//            /     \         /     \
//        55 (3)   44 (4)  61 (5)   68 (6)
//         /
//     58 (7)
//
// Comparado con las versiones de punteros: aquí desaparecen slot(), path(),
// route(), destroy() y el Node. Todo se reduce a las fórmulas padre/hijo.
//
template <typename T, typename Comp = Min<T>>
class ArrayHeap {

    vector<T> a;   // a[0] es la raíz; el "árbol" es solo una forma de leer los índices
    size_t d;
    Comp comp;

    // ------------------------------------------------------------------
    // Con d = 2 son las fórmulas que recuerdas: (i-1)/2, 2i+1, 2i+2
    //
    //   parent(i)   = (i-1) / d
    //   child(i, c) = d*i + c + 1        (c = 0 .. d-1, el número de hijo)
    //
    // Numeración con d = 2:
    //
    //                  0
    //               /     \
    //             1         2
    //            / \       / \
    //           3   4     5   6
    //
    //   child(1, 0) = 2*1+0+1 = 3        child(1, 1) = 2*1+1+1 = 4
    //   parent(4)   = (4-1)/2 = 1        parent(3)   = (3-1)/2 = 1
    //
    // Numeración con d = 3:
    //
    //                          0
    //               /          |          \
    //             1            2            3
    //           / | \        / | \        / | \
    //          4  5  6      7  8  9     10 11 12
    //
    //   child(2, 1) = 3*2+1+1 = 8        parent(8)  = (8-1)/3 = 2
    //   child(3, 0) = 3*3+0+1 = 10       parent(10) = (10-1)/3 = 3
    //
    // OJO: parent(0) = (0-1)/d usa size_t sin signo -> daría un número enorme.
    // Por eso siftUp pregunta "i > 0" ANTES de llamar a parent(i).
    // ------------------------------------------------------------------
    size_t parent(size_t i) const {
      return (i - 1) / d;
    }

    size_t child(size_t i, size_t c) const {
      return d * i + c + 1;
    }

    // ------------------------------------------------------------------
    // Sube a[i] mientras gane a su padre
    //
    // Ejemplo (min-heap, d = 2): a = [42, 55, 61] y acabamos de agregar 11 al final.
    //
    //   a = [42 55 61 11]     i = 3,  parent(3) = (3-1)/2 = 1
    //
    //          42                  ¿11 < a[1]=55?  sí -> swap
    //         /  \
    //       55    61
    //       /
    //    [11]
    //
    //   a = [42 11 61 55]     i = 1,  parent(1) = (1-1)/2 = 0
    //
    //          42                  ¿11 < a[0]=42?  sí -> swap
    //         /  \
    //      [11]   61
    //       /
    //     55
    //
    //   a = [11 42 61 55]     i = 0  -> "i > 0" es falso, el while termina
    //
    //         [11]
    //         /  \
    //       42    61
    //       /
    //     55
    //
    // Si comp(a[i], a[parent]) da false en algún paso, el while termina antes:
    // ya está en su lugar.
    // ------------------------------------------------------------------
    void siftUp(size_t i) {
        while (i > 0 && comp(a[i], a[parent(i)])) {
            swap(a[i], a[parent(i)]);
            i = parent(i);
        }
    }

    // ------------------------------------------------------------------
    // Baja a[i] mientras algún hijo le gane
    //
    // "best" empieza siendo el propio i y se compara contra cada uno de los d hijos.
    // Al final, best es el mejor entre el nodo y todos sus hijos.
    // (En la versión de punteros el nodo se comparaba aparte; aquí va todo junto.)
    //
    // Ejemplo (min-heap, d = 2): a = [58, 42, 57, 55, 44, 61, 68]
    //
    //   i = 0 (58):  hijos child(0,0)=1 -> 42,  child(0,1)=2 -> 57
    //
    //          [58]               best = 0 (58)
    //         /     \             k=1: 42 < 58?  sí -> best = 1
    //       42       57           k=2: 57 < 42?  no
    //      /  \     /  \          best = 1 != i  -> swap(a[0], a[1]), i = 1
    //    55   44  61   68
    //
    //   a = [42, 58, 57, 55, 44, 61, 68]
    //
    //   i = 1 (58):  hijos child(1,0)=3 -> 55,  child(1,1)=4 -> 44
    //
    //           42                best = 1 (58)
    //         /     \             k=3: 55 < 58?  sí -> best = 3
    //      [58]      57           k=4: 44 < 55?  sí -> best = 4
    //      /  \     /  \          best = 4 != i  -> swap(a[1], a[4]), i = 4
    //    55   44  61   68
    //
    //   a = [42, 44, 57, 55, 58, 61, 68]
    //
    //   i = 4 (58):  hijos child(4,0)=9, child(4,1)=10  -> ambos >= a.size() (7)
    //
    //           42                no hay hijos (k < a.size() es falso)
    //         /     \             best == i  -> break
    //       44       57
    //      /  \     /  \
    //    55  [58] 61   68
    //
    // Por eso se pregunta "k < a.size()": los índices de hijos que se pasan del
    // final del arreglo simplemente no existen.
    // ------------------------------------------------------------------
    void siftDown(size_t i) {
        while (true) {
            size_t best = i;                       // el mejor entre i y sus hijos
            for (size_t c = 0; c < d; c++) {
                size_t k = child(i, c);
                if (k < a.size() && comp(a[k], a[best]))
                    best = k;
            }
            if (best == i) break;                  // ya está en su lugar
            swap(a[i], a[best]);
            i = best;
        }
    }

public:
    ArrayHeap(size_t ramas = 2) : d(ramas) { assert(d >= 2); }

    bool empty() const { return a.empty(); }
    size_t size() const { return a.size(); }
    const T& top() const { return a[0]; }   // precondición: !empty()

    // ------------------------------------------------------------------
    // INSERT: agregar al final (primer hueco) y subir
    //
    // push_back ES "pegar en la primera posición libre": el árbol completo
    // crece siempre por el final del último nivel.
    //
    // Ejemplo (min-heap, d = 3): a = [11 44 61 42 55 57 68] e insertamos 58.
    //
    //   push_back(58):   a = [11 44 61 42 55 57 68 58]      i = 7
    //                    parent(7) = (7-1)/3 = 2
    //
    //                         11
    //               /          |          \
    //             44          61          42
    //           / | \        /
    //         55 57 68    [58]             ¿58 < a[2]=61?  sí -> swap
    //
    //   siftUp:          a = [11 44 58 42 55 57 68 61]      i = 2
    //                    parent(2) = (2-1)/3 = 0
    //
    //                         11
    //               /          |          \
    //             44         [58]          42
    //           / | \        /
    //         55 57 68      61             ¿58 < a[0]=11?  no -> termina
    //
    // Con d = 3 el árbol es más bajo: aquí subió 1 nivel y se detuvo.
    // ------------------------------------------------------------------
    void ins(const T& x) {
        a.push_back(x);
        siftUp(a.size() - 1);
    }

    // ------------------------------------------------------------------
    // REMOVE (raíz): el último pasa a la raíz, se acorta el arreglo y se baja
    //
    // Ejemplo (min-heap, d = 2): a = [11 42 57 55 44 61 68 58]
    //
    //   Estado inicial:
    //                    11               <- se extrae este
    //                 /      \
    //               42        57
    //              /  \      /  \
    //            55    44  61    68
    //           /
    //         58                          <- a.back(), el último
    //
    //   a[0] = a.back():     a = [58 42 57 55 44 61 68 58]
    //   a.pop_back():        a = [58 42 57 55 44 61 68]     (el último sobra, se corta)
    //
    //                   [58]
    //                 /      \
    //               42        57
    //              /  \      /  \
    //            55    44  61    68
    //
    //   siftDown(0): ver el dibujo en siftDown. Resultado final:
    //
    //                    42
    //                 /      \
    //               44        57
    //              /  \      /  \
    //            55   58   61    68
    //
    //   a = [42 44 57 55 58 61 68]
    //
    // El caso n == 1: a = [x]. a[0] = a.back() (se copia a sí mismo), pop_back
    // deja el arreglo vacío y "if (!empty())" evita llamar siftDown sobre nada.
    // ------------------------------------------------------------------
    bool rem() {
        if (empty()) return false;
        a[0] = a.back();
        a.pop_back();
        if (!empty()) siftDown(0);
        return true;
    }

    // ------------------------------------------------------------------
    // El arreglo ES el recorrido por niveles
    //
    // No hace falta cola: basta recorrer el arreglo de izquierda a derecha y
    // poner una barra al terminar cada nivel. Un nivel tiene d veces más
    // nodos que el anterior (1, d, d*d, ...), por eso "level *= d".
    //
    //   d = 2:   [ 11 | 42 57 | 55 44 61 68 | 58 ]
    //              1      2         4          resto (nivel incompleto)
    //
    //   d = 3:   [ 11 | 44 58 42 | 55 57 68 61 ]
    //              1       3         resto
    //
    //   Salida d = 2:  11 | 42 57 | 55 44 61 68 | 58
    // ------------------------------------------------------------------
    void print() const {
        size_t level = 1, count = 0;
        for (size_t i = 0; i < a.size(); i++) {
            cout << a[i] << " ";
            if (++count == level) { cout << "| "; level *= d; count = 0; }
        }
        cout << endl;
    }

    // Imprime el arreglo tal cual, sin barras:  [ 11 42 57 55 44 61 68 58 ]
    void printArray() const {
        cout << "[ ";
        for (const T& x : a) cout << x << " ";
        cout << "]" << endl;
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
    ArrayHeap<int, Comp> h(d);
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

// ---------- Resultado esperado de main ----------
//
//   Min-heap d=2 arreglo: [ 11 42 57 55 44 61 68 58 ]
//   Min-heap d=2 niveles: 11 | 42 57 | 55 44 61 68 | 58
//   Max-heap d=2 arreglo: [ 68 58 61 44 42 55 57 11 ]
//   Min-heap d=3 niveles: 11 | 44 58 42 | 55 57 68 61
//   Extrayendo min-heap d=2: 11 42 44 55 57 58 61 68
//
// El min-heap d=3 con los mismos 8 datos queda más bajo (3 niveles en vez de 4):
//
//                         11
//               /          |          \
//             44          58          42
//           / | \        /
//         55 57 68      61

int main() {
    int datos[] = {55, 42, 61, 11, 44, 57, 68, 58};

    ArrayHeap<int, Min<int>> minH(2);
    ArrayHeap<int, Max<int>> maxH(2);
    for (int x : datos) { minH.ins(x); maxH.ins(x); }

    cout << "Min-heap d=2 arreglo: "; minH.printArray();
    cout << "Min-heap d=2 niveles: "; minH.print();
    cout << "Max-heap d=2 arreglo: "; maxH.printArray();

    ArrayHeap<int, Min<int>> tri(3);
    for (int x : datos) tri.ins(x);
    cout << "Min-heap d=3 niveles: "; tri.print();

    cout << "Extrayendo min-heap d=2: ";
    while (!minH.empty()) { cout << minH.top() << " "; minH.rem(); }
    cout << endl;

    for (size_t d = 2; d <= 6; d++) {
        test<Min<int>>(d, less<int>());
        test<Max<int>>(d, greater<int>());
    }
    cout << "Pruebas aleatorias OK (d = 2..6, min y max)" << endl;
    return 0;
}
