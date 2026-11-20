#include <iostream>
#include <vector>
#include <queue>
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
