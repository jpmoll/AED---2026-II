#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cassert>
using namespace std;

// comp(a, b) == true  significa "a debe estar MÁS ARRIBA que b"
template <typename T>
struct Min {
    bool operator()(const T& a, const T& b) const { return a < b; }
};

template <typename T>
struct Max {
    bool operator()(const T& a, const T& b) const { return a > b; }
};

// Heap d-ario sobre un arreglo. d = 2 es el heap binario clásico.
template <typename T, typename Comp = Min<T>>
class ArrayHeap {

    vector<T> a;   // a[0] es la raíz; el "árbol" es solo una forma de leer los índices
    size_t d;
    Comp comp;

    // Con d = 2 son las fórmulas que recuerdas: (i-1)/2, 2i+1, 2i+2
    size_t parent(size_t i) const {
      return (i - 1) / d;
    }

    size_t child(size_t i, size_t c) const {
      return d * i + c + 1;
    } 

    // Sube a[i] mientras gane a su padre
    void siftUp(size_t i) {
        while (i > 0 && comp(a[i], a[parent(i)])) {
            swap(a[i], a[parent(i)]);
            i = parent(i);
        }
    }

    // Baja a[i] mientras algún hijo le gane
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

    // INSERT: agregar al final (primer hueco) y subir
    void ins(const T& x) {
        a.push_back(x);
        siftUp(a.size() - 1);
    }

    // REMOVE (raíz): el último pasa a la raíz, se acorta el arreglo y se baja
    bool rem() {
        if (empty()) return false;
        a[0] = a.back();
        a.pop_back();
        if (!empty()) siftDown(0);
        return true;
    }

    // El arreglo ES el recorrido por niveles
    void print() const {
        size_t level = 1, count = 0;
        for (size_t i = 0; i < a.size(); i++) {
            cout << a[i] << " ";
            if (++count == level) { cout << "| "; level *= d; count = 0; }
        }
        cout << endl;
    }

    void printArray() const {
        cout << "[ ";
        for (const T& x : a) cout << x << " ";
        cout << "]" << endl;
    }
};

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
