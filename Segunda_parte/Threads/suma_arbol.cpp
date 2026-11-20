#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <cassert>
using namespace std;

template <typename T>
struct Min {
    bool operator()(const T& a, const T& b) const { return a < b; }
};

template <typename T>
struct Max {
    bool operator()(const T& a, const T& b) const { return a > b; }
};

template <typename T>
struct Node {
    T value;
    vector<Node*> child;
    Node(const T& v, size_t d) : value(v), child(d, nullptr) {}
};

template <typename T, typename Comp = Min<T>>
class DHeap {
    Node<T>* root;
    size_t n;
    size_t d;
    Comp comp;

    vector<size_t> route(size_t j) const {
        vector<size_t> r;
        while (j > 0) {
            r.push_back((j - 1) % d);
            j = (j - 1) / d;
        }
        reverse(r.begin(), r.end());
        return r;
    }

    Node<T>** slot(size_t j) {
        Node<T>** p = &root;
        for (size_t c : route(j))
            p = &((*p)->child[c]);
        return p;
    }

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

    // ---------- Suma ----------

    // Suma secuencial de TODO el subárbol que cuelga de t
    static long long sumSub(const Node<T>* t) {
        if (!t) return 0;
        long long s = t->value;
        for (const Node<T>* c : t->child) s += sumSub(c);
        return s;
    }

    // Recorre solo la parte de arriba. Cuando llega al nivel pedido NO sigue bajando:
    // guarda ese nodo en 'frontera' (será el trabajo de un thread).
    // Los nodos que quedan por encima del nivel se suman aquí, en el thread principal.
    // nivel: 1 = la raíz, 2 = los hijos de la raíz, etc.
    long long top(Node<T>* t, size_t nivel, vector<Node<T>*>& frontera) const {
        if (!t) return 0;
        if (nivel == 1) {
            frontera.push_back(t);
            return 0;
        }
        long long s = t->value;
        for (Node<T>* c : t->child) s += top(c, nivel - 1, frontera);
        return s;
    }

public:
    DHeap(size_t ramas = 2) : root(nullptr), n(0), d(ramas) { assert(d >= 2); }
    ~DHeap() { destroy(root); }
    DHeap(const DHeap&) = delete;
    DHeap& operator=(const DHeap&) = delete;

    size_t size() const { return n; }

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

    // Versión secuencial (referencia)
    long long sum() const { return sumSub(root); }

    // Versión paralela: un thread por cada nodo del nivel indicado
    long long sumPar(size_t nivel = 2) const {
        vector<Node<T>*> frontera;
        long long total = top(root, nivel, frontera);   // parte de arriba, sin threads

        vector<long long> parcial(frontera.size(), 0);  // cada thread escribe SOLO en su casilla
        vector<thread> hilos;
        hilos.reserve(frontera.size());

        for (size_t i = 0; i < frontera.size(); i++)
            hilos.emplace_back([&parcial, &frontera, i]() {
                parcial[i] = sumSub(frontera[i]);
            });

        for (thread& h : hilos) h.join();               // esperar a todos

        for (long long p : parcial) total += p;         // combinar
        return total;
    }
};

int main() {
    cout << "Núcleos reportados por el sistema: " << thread::hardware_concurrency() << endl;

    const size_t D = 8;               // 8 ramas: la raíz tiene 8 hijos (nivel 2)
    const int N = 3000000;

    DHeap<int, Min<int>> h(D);
    for (int i = 1; i <= N; i++) h.ins(i);

    // Suma esperada: 1+2+...+N
    long long esperada = (long long)N * (N + 1) / 2;

    using clk = chrono::steady_clock;
    auto ms = [](clk::time_point a, clk::time_point b) {
        return chrono::duration<double, milli>(b - a).count();
    };

    auto t0 = clk::now();
    long long s1 = h.sum();
    auto t1 = clk::now();
    long long s2 = h.sumPar(2);       // 8 threads (uno por hijo de la raíz)
    auto t2 = clk::now();
    long long s3 = h.sumPar(3);       // 64 threads, para comparar

    cout << "Esperada:            " << esperada << endl;
    cout << "Secuencial:          " << s1 << "  (" << ms(t0, t1) << " ms)" << endl;
    cout << "Paralela nivel 2:    " << s2 << "  (" << ms(t1, t2) << " ms, 8 threads)" << endl;
    cout << "Paralela nivel 3:    " << s3 << endl;
    assert(s1 == esperada && s2 == esperada && s3 == esperada);
    cout << "Todas coinciden OK" << endl;
    return 0;
}
