#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// ---------- Functores de comparación ----------
// comp(a, b) == true  significa "a debe estar MÁS ARRIBA que b"
//
//   Min: a < b  ->  el menor sube      Max: a > b  ->  el mayor sube
//
//        Min-heap            Max-heap
//           11                  68
//          /  \                /  \
//        42    57            58    61
//   (los hijos de cada nodo están por debajo de él; el menor/mayor manda)
//
template <typename T>
struct Min {
    bool operator()(const T& a, const T& b) const { return a < b; }
};

template <typename T>
struct Max {
    bool operator()(const T& a, const T& b) const { return a > b; }
};

// ---------- Nodo ----------
//
//      +-------------------+
//      | value             |
//      | left  ----> Node* |   (nullptr si no hay hijo)
//      | right ----> Node* |
//      +-------------------+
//
template <typename T>
struct Node {
    T value;
    Node* left;
    Node* right;
    Node(const T& v) : value(v), left(nullptr), right(nullptr) {}
};

// ---------- Heap (árbol binario completo con punteros) ----------
//
// "Completo" = se llena por niveles, de izquierda a derecha, sin huecos.
// Cada posición tiene un número k (1-based), en orden por niveles:
//
//   nivel 0:                 1
//                         /     \
//   nivel 1:            2         3
//                      / \       / \
//   nivel 2:          4   5     6   7
//                    /
//   nivel 3:        8   ...
//
// Con n nodos, el heap ocupa exactamente las posiciones 1..n.
// La próxima posición libre siempre es n+1.
//
template <typename T, typename Comp = Min<T>>
class Heap {
    Node<T>* root;
    size_t n;     // cantidad de nodos; sirve para ubicar la posición k en el árbol
    Comp comp;    // el functor que decide si es min-heap o max-heap

    // ------------------------------------------------------------------
    // slot(k): devuelve la DIRECCIÓN del puntero que apunta a la posición k.
    // Los bits de k (sin el 1 inicial) son el camino: 0 = izquierda, 1 = derecha.
    //
    // Ejemplo k = 6:
    //
    //   6 en binario = 1 1 0
    //                  | \_/
    //          (se quita) 1 0  ->  derecha, izquierda
    //
    //                  1            <- root
    //               /     \
    //             2         [3]     <- bit 1: derecha
    //            / \        / \
    //           4   5    [[6]]  7   <- bit 0: izquierda  (posición 6)
    //
    // Ejemplo k = 8 (1000 -> "000" -> izq, izq, izq):
    //
    //                  1
    //                /
    //              2                <- izquierda
    //            /
    //          4                    <- izquierda
    //        /
    //     [[8]]                     <- izquierda
    //
    // Devuelve Node** (puntero al puntero) para poder ESCRIBIR en el hueco:
    //     *slot(n+1) = new Node(x);    // el hueco es un nullptr que llenamos
    // ------------------------------------------------------------------
    Node<T>** slot(size_t k) {
        Node<T>** p = &root;
        int h = 0;
        while ((k >> (h + 1)) != 0) h++;            // h = posición del bit más alto
        for (int i = h - 1; i >= 0; i--)
            p = ((k >> i) & 1) ? &((*p)->right) : &((*p)->left);
        return p;
    }

    // ------------------------------------------------------------------
    // path(k): los nodos desde la raíz hasta la posición k (inclusive).
    // Reemplaza al puntero "padre", que nuestro Node no tiene.
    //
    // Ejemplo k = 6  ->  v = [ nodo1, nodo3, nodo6 ]
    //
    //                  (1)  <- v[0]
    //               /     \
    //             2        (3)  <- v[1]
    //            / \       / \
    //           4   5   ((6))  7   <- v[2]
    //
    // El padre de v[i] es v[i-1]. Por eso ins() sube comparando
    // pth[i] con pth[i-1].
    // ------------------------------------------------------------------
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

    // destroy: borra en POSTORDEN (primero hijos, luego el nodo)
    //
    //          A            orden de borrado:  C, D, B, E, A
    //         / \
    //        B   E          (si borráramos A primero, perderíamos
    //       / \              los punteros a B y E)
    //      C   D
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

    // ------------------------------------------------------------------
    // INSERT: 1) pegar en la primera posición libre, 2) subir mientras gane a su padre
    //
    // Ejemplo (min-heap): ya tenemos {42, 55, 61} y insertamos 11.
    //
    // Paso 1: slot(n+1) = slot(4) -> hijo izquierdo de 55. Pegamos 11 ahí.
    //
    //            42
    //          /    \
    //        55      61
    //       /
    //     [11]            <- nodo nuevo (pth = [42, 55, 11])
    //
    // Paso 2: ¿11 < 55? Sí -> swap
    //
    //            42
    //          /    \
    //       [11]     61
    //       /
    //     55
    //
    // Paso 3: ¿11 < 42? Sí -> swap
    //
    //          [11]
    //          /   \
    //        42     61
    //       /
    //     55
    //
    // Paso 4: llegamos a la raíz (i == 0), el for termina.
    //
    // Si en algún paso comp(hijo, padre) es false, hacemos break:
    // el resto del camino ya cumple la propiedad del heap.
    // ------------------------------------------------------------------
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

    // ------------------------------------------------------------------
    // REMOVE (extrae la raíz): 1) copiar el último valor a la raíz, 2) borrar el último
    //                          nodo, 3) bajar la raíz mientras algún hijo le gane
    //
    // Ejemplo (min-heap) con n = 8:
    //
    //   Estado inicial:
    //                 11                 <- se extrae este
    //              /      \
    //            42        57
    //           /  \      /  \
    //         55    44  61    68
    //        /
    //      58                            <- último (posición 8)
    //
    //   Paso 1 y 2: copiar 58 a la raíz y borrar el último nodo
    //
    //                [58]               <- el 11 desaparece
    //              /      \
    //            42        57
    //           /  \      /  \
    //         55    44  61    68
    //        /
    //      (x)                           <- borrado, el padre apunta a nullptr
    //
    //   Paso 3a: hijos de 58 son 42 y 57. "best" = 42 (el mejor según comp).
    //            ¿42 le gana a 58? Sí -> swap, seguimos bajando
    //
    //                 42
    //              /      \
    //          [58]        57
    //           /  \      /  \
    //         55    44  61    68
    //
    //   Paso 3b: hijos de 58 son 55 y 44. best = 44. ¿44 < 58? Sí -> swap
    //
    //                 42
    //              /      \
    //            44        57
    //           /  \      /  \
    //         55   [58]  61    68
    //
    //   Paso 3c: 58 ya no tiene hijos (best == nullptr) -> break. Listo.
    //
    //   Por qué se elige el MEJOR hijo (y no cualquiera): el que suba a ser
    //   padre debe ganarle también a su hermano. Si subiera 55 en vez de 44,
    //   quedaría 55 arriba de 44 y se rompería la propiedad.
    // ------------------------------------------------------------------
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

    // ------------------------------------------------------------------
    // Recorrido por niveles (BFS con cola): así se ve la forma real del heap
    //
    //   Árbol:               Cola (frente -> final)       Salida
    //
    //        11              [11]                          
    //       /  \             sale 11, entran 42 57         11 |
    //     42    57           [42 57]
    //     /                  sale 42, entra 55             
    //   55                   sale 57                       42 57 |
    //                        [55]
    //                        sale 55                       55 |
    //
    //   Resultado impreso:  11 | 42 57 | 55 |
    //
    // "level" es cuántos nodos tiene el nivel actual (1, 2, 4, 8...);
    // "count" cuenta cuántos llevamos impresos en ese nivel. Al llegar a
    // "level" imprimimos "| " y el nivel se duplica.
    // ------------------------------------------------------------------
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

// ---------- Resultado esperado de main ----------
//
// Insertando 55 42 61 11 44 57 68 58, el min-heap queda así:
//
//                 11
//              /      \
//            42        57
//           /  \      /  \
//         55    44  61    68
//        /
//      58
//
// y el max-heap así:
//
//                 68
//              /      \
//            58        61
//           /  \      /  \
//         44    42  55    57
//        /
//      11
//
// (puedes comprobarlo ejecutando el programa: print() imprime por niveles)

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
