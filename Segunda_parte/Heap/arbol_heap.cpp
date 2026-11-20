#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// ---------- Functores de comparación ----------
// comp(a, b) == true  significa "a debe estar MÁS ARRIBA que b"
template <typename T>
struct Min {
    bool operator()(const T& a, const T& b) const { return a < b; }
};

template <typename T>
struct Max {
    bool operator()(const T& a, const T& b) const { return a > b; }
};

// ---------- Nodo ----------
template <typename T>
struct Node {
    T value;
    Node* left;
    Node* right;
    Node(const T& v) : value(v), left(nullptr), right(nullptr) {}
};

// ---------- Heap (árbol binario completo con punteros) ----------
template <typename T, typename Comp = Min<T>>
class Heap {
    Node<T>* root;
    size_t n;     // cantidad de nodos; sirve para ubicar la posición k en el árbol
    Comp comp;    // el functor que decide si es min-heap o max-heap

    // Devuelve la DIRECCIÓN del puntero que apunta a la posición k (1-based).
    // Es el equivalente al find() del BST: te deja parado en el "hueco" o en el nodo.
    // Truco: los bits de k (sin el 1 inicial) son el camino: 0 = izquierda, 1 = derecha.
    // Ej: k=6 = 110 -> quitamos el 1 inicial -> "10" -> derecha, luego izquierda.
    //     (raíz -> right -> left es la posición 6 en el orden por niveles)
    Node<T>** slot(size_t k) {
        Node<T>** p = &root;
        int h = 0;
        while ((k >> (h + 1)) != 0) h++;            // h = posición del bit más alto
        for (int i = h - 1; i >= 0; i--)
            p = ((k >> i) & 1) ? &((*p)->right) : &((*p)->left);
        return p;
    }

    // Nodos desde la raíz hasta la posición k (inclusive). Reemplaza al puntero "padre",
    // que nuestro Node no tiene.
    vector<Node<T>*> path(size_t k) {
        vector<Node<T>*> v;
        Node<T>* cur = root;
        v.push_back(cur);
        int h = 0;
        while ((k >> (h + 1)) != 0) h++;
        for (int i = h - 1; i >= 0; i--) {
            cur = ((k >> i) & 1) ? cur->right : cur->left;
            v.push_back(cur);
        }
        return v;
    }

    void destroy(Node<T>* t) {
        if (!t) return;
        destroy(t->left);
        destroy(t->right);
        delete t;
    }

public:
    Heap() : root(nullptr), n(0) {}
    ~Heap() { destroy(root); }
    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;

    bool empty() const { return n == 0; }
    size_t size() const { return n; }
    const T& top() const { return root->value; }  // precondición: !empty()

    // INSERT: 1) pegar en la primera posición libre, 2) subir mientras gane a su padre
    void ins(const T& x) {
        *slot(n + 1) = new Node<T>(x);   // el hueco siempre es nullptr
        n++;

        vector<Node<T>*> pth = path(n);
        for (size_t i = pth.size() - 1; i > 0; i--) {
            if (comp(pth[i]->value, pth[i - 1]->value))
                swap(pth[i]->value, pth[i - 1]->value);
            else
                break;                   // ya está en su lugar
        }
    }

    // REMOVE (extrae la raíz): 1) copiar el último valor a la raíz, 2) borrar el último
    //                          nodo, 3) bajar la raíz mientras algún hijo le gane
    bool rem() {
        if (empty()) return false;

        Node<T>** last = slot(n);        // puntero al último nodo
        root->value = (*last)->value;    // (si n==1, last==&root y esto es inofensivo)
        delete *last;
        *last = nullptr;                 // el padre ya no apunta a memoria liberada
        n--;

        Node<T>* cur = root;             // si el heap quedó vacío, root es nullptr y no entra
        while (cur) {
            Node<T>* best = cur->left;   // el "mejor" hijo según el functor
            if (cur->right && (!best || comp(cur->right->value, best->value)))
                best = cur->right;

            if (best && comp(best->value, cur->value)) {
                swap(best->value, cur->value);
                cur = best;
            } else
                break;
        }
        return true;
    }

    // Recorrido por niveles: así se ve la forma real del heap
    void print() {
        if (!root) { cout << "(vacío)" << endl; return; }
        queue<Node<T>*> q;
        q.push(root);
        size_t level = 1, count = 0;
        while (!q.empty()) {
            Node<T>* t = q.front(); q.pop();
            cout << t->value << " ";
            if (t->left)  q.push(t->left);
            if (t->right) q.push(t->right);
            if (++count == level) { cout << "| "; level *= 2; count = 0; }
        }
        cout << endl;
    }
};

int main() {
    int datos[] = {55, 42, 61, 11, 44, 57, 68, 58};

    Heap<int, Min<int>> minH;
    Heap<int, Max<int>> maxH;
    for (int x : datos) { minH.ins(x); maxH.ins(x); }

    cout << "Min-heap (por niveles): "; minH.print();
    cout << "Max-heap (por niveles): "; maxH.print();

    cout << "\nExtrayendo todo del min-heap: ";
    while (!minH.empty()) { cout << minH.top() << " "; minH.rem(); }

    cout << "\nExtrayendo todo del max-heap: ";
    while (!maxH.empty()) { cout << maxH.top() << " "; maxH.rem(); }
    cout << endl;

    return 0;
}
