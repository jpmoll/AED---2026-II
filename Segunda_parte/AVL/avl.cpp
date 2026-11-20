#include <algorithm>
#include <iostream>
#include <string>
using namespace std;

/*
 * AVL = BST + regla: en TODO nodo, las alturas de sus dos subárboles difieren como máximo en 1.
 *
 * Convenciones (las de tu código):
 *   - altura de nullptr = 0, altura de una hoja = 1
 *   - balance(n) = altura(izq) - altura(der)
 *         balance > 0  -> más pesado a la IZQUIERDA
 *         balance < 0  -> más pesado a la DERECHA
 *         balance en {-1, 0, 1} -> OK;  +2 o -2 -> hay que rotar
 */

struct Cnode {
    int value;
    int height;
    Cnode* a[2]; // [0]: left, [1]: right
    Cnode(int v) : value(v), height(1) { a[0] = a[1] = nullptr; }
};

class Arbol {
    Cnode* Root = nullptr;

    int getHeight(Cnode* n) { return n ? n->height : 0; }
    int getBalance(Cnode* n) { return n ? getHeight(n->a[0]) - getHeight(n->a[1]) : 0; }
    void update(Cnode* n) { n->height = 1 + max(getHeight(n->a[0]), getHeight(n->a[1])); }

    /*
     * ========================================================================
     * ROTACIÓN A LA DERECHA  (arregla un nodo pesado a la izquierda)
     *
     *  El nodo 'y' sube su hijo izquierdo 'x' a su lugar, y 'y' baja a la derecha de 'x'.
     *  El subárbol T (derecha de x) queda "huérfano" y pasa a ser la izquierda de y.
     *
     *          y                           x
     *         / \                         / \
     *        x   C       ----->          A   y
     *       / \                             / \
     *      A   T                           T   C
     *
     *  Por qué T cabe ahí: en el BST, A < x < T < y < C.
     *  Después de rotar: x tiene a A a su izquierda; y tiene a T (entre x e y) a su izquierda.
     *  El recorrido in-order sigue siendo  A x T y C  -> sigue siendo un BST.
     *
     *  Orden de las alturas: PRIMERO y (que ahora es hijo), DESPUÉS x (que ahora es padre),
     *  porque la altura de x depende de la nueva altura de y.
     * ========================================================================
     */
    Cnode* rotateRight(Cnode* y) {
        Cnode* x = y->a[0];
        Cnode* T = x->a[1];
        x->a[1] = y;
        y->a[0] = T;
        update(y);
        update(x);
        return x;       // x es la nueva raíz de este subárbol; el llamador debe enlazarla
    }

    /*
     * ========================================================================
     * ROTACIÓN A LA IZQUIERDA  (arregla un nodo pesado a la derecha) - es la imagen en espejo
     *
     *        x                               y
     *       / \                             / \
     *      A   y         ----->            x   C
     *         / \                         / \
     *        T   C                       A   T
     *
     *  In-order antes y después:  A x T y C
     * ========================================================================
     */
    Cnode* rotateLeft(Cnode* x) {
        Cnode* y = x->a[1];
        Cnode* T = y->a[0];
        y->a[0] = x;
        x->a[1] = T;
        update(x);
        update(y);
        return y;
    }

    /*
     * ========================================================================
     * INSERT recursivo. Se inserta como en un BST y, al REGRESAR de la recursión (de abajo
     * hacia arriba), cada ancestro recalcula su altura y revisa su balance.
     *
     * Los 4 casos. 'node' es el primer ancestro desbloqueado en +2 / -2, y 'v' el valor
     * recién insertado. Mirando hacia dónde cayó v se sabe cuál es el caso:
     *
     * ---- CASO 1: LEFT-LEFT (balance > 1 y v < izq->value) ----------> UNA rotación derecha
     *
     *   insertar 30, 20, 10
     *
     *        30  (+2)                     20
     *        /                           /  \
     *      20           -rotR(30)->    10    30
     *      /
     *    10
     *
     * ---- CASO 2: RIGHT-RIGHT (balance < -1 y v > der->value) -------> UNA rotación izquierda
     *
     *   insertar 10, 20, 30
     *
     *      10  (-2)                       20
     *        \                           /  \
     *         20          -rotL(10)->  10    30
     *           \
     *            30
     *
     * ---- CASO 3: LEFT-RIGHT (balance > 1 y v > izq->value) ---------> DOS rotaciones
     *
     *   insertar 30, 10, 20        Una sola rotación derecha NO sirve: el "codo" persiste.
     *
     *        30 (+2)                  30 (+2)                  20
     *        /                        /                       /  \
     *      10         -rotL(10)->    20          -rotR(30)->  10    30
     *        \                       /
     *         20                   10
     *
     *   1º rotL sobre el hijo izquierdo: convierte el codo en una línea recta (caso LL)
     *   2º rotR sobre el nodo desbalanceado: ya es el caso 1
     *
     * ---- CASO 4: RIGHT-LEFT (balance < -1 y v < der->value) --------> DOS rotaciones
     *
     *   insertar 10, 30, 20
     *
     *      10 (-2)                  10 (-2)                      20
     *        \                        \                         /  \
     *         30       -rotR(30)->     20        -rotL(10)->  10    30
     *         /                          \
     *       20                            30
     *
     *   1º rotR sobre el hijo derecho (codo -> línea recta, caso RR)
     *   2º rotL sobre el nodo desbalanceado
     *
     * RESUMEN:   zig-zag (LR / RL) = rotar el hijo primero, luego el nodo
     *            línea recta (LL / RR) = una sola rotación
     *
     * ¿Por qué comparar v con el hijo funciona? Si el nodo quedó en +2 por una inserción,
     * v cayó DENTRO del subárbol izquierdo (o derecho) y ese hijo tiene balance ±1 (no 0):
     * v < izq->value => cayó en la izquierda del hijo (LL); v > izq->value => derecha (LR).
     * (Para BORRAR esto ya no sirve: ahí se usa getBalance del hijo, que sí puede ser 0.)
     *
     * Tras la rotación la altura del subárbol vuelve a ser la que tenía ANTES de insertar,
     * así que los ancestros de más arriba ya no se desbalancean: máximo 1 rotación (simple
     * o doble) por inserción.
     * ========================================================================
     */
    Cnode* insert(Cnode* node, int v) {
        if (!node) return new Cnode(v);
        if (v < node->value)      node->a[0] = insert(node->a[0], v);
        else if (v > node->value) node->a[1] = insert(node->a[1], v);
        else return node;                     // sin duplicados

        update(node);                         // 1) recalcular altura al regresar
        int balance = getBalance(node);       // 2) revisar balance

        if (balance > 1 && v < node->a[0]->value)  return rotateRight(node);        // LL
        if (balance < -1 && v > node->a[1]->value) return rotateLeft(node);         // RR
        if (balance > 1 && v > node->a[0]->value) {                                 // LR
            node->a[0] = rotateLeft(node->a[0]);
            return rotateRight(node);
        }
        if (balance < -1 && v < node->a[1]->value) {                                // RL
            node->a[1] = rotateRight(node->a[1]);
            return rotateLeft(node);
        }
        return node;
    }

    void destroy(Cnode* n) {
        if (!n) return;
        destroy(n->a[0]);
        destroy(n->a[1]);
        delete n;
    }

    void inorder(Cnode* n) const {
        if (!n) return;
        inorder(n->a[0]);
        cout << n->value << " ";
        inorder(n->a[1]);
    }

    // Dibuja el árbol de lado: la raíz a la izquierda, la derecha arriba, la izquierda abajo.
    // Cada nodo muestra valor y (h=altura, b=balance).
    void sideways(Cnode* n, int depth) const {
        if (!n) return;
        sideways(n->a[1], depth + 1);
        cout << string(depth * 8, ' ') << n->value
             << " (h=" << n->height
             << ",b=" << (n->a[0] ? n->a[0]->height : 0) - (n->a[1] ? n->a[1]->height : 0) << ")\n";
        sideways(n->a[0], depth + 1);
    }

    // Devuelve la altura real y comprueba: altura guardada correcta, |balance| <= 1, orden BST
    bool check(Cnode* n, long lo, long hi, int& h) const {
        if (!n) { h = 0; return true; }
        if (n->value <= lo || n->value >= hi) return false;
        int hl, hr;
        if (!check(n->a[0], lo, n->value, hl)) return false;
        if (!check(n->a[1], n->value, hi, hr)) return false;
        h = 1 + max(hl, hr);
        return h == n->height && abs(hl - hr) <= 1;
    }

public:
    Arbol() = default;
    Arbol(const Arbol&) = delete;
    Arbol& operator=(const Arbol&) = delete;
    ~Arbol() { destroy(Root); }

    void Agregar(int v) { Root = insert(Root, v); }

    void print() const { inorder(Root); cout << endl; }
    void mostrar() const { sideways(Root, 0); }
    bool esAVL() const { int h; return check(Root, -(1L << 40), 1L << 40, h); }
};

void caso(const string& nombre, initializer_list<int> vals) {
    Arbol t;
    cout << "=== " << nombre << ": insertar";
    for (int v : vals) { cout << " " << v; t.Agregar(v); }
    cout << "\n";
    t.mostrar();
    cout << (t.esAVL() ? "AVL válido\n\n" : "¡NO es AVL!\n\n");
}

int main() {
    caso("LL", {30, 20, 10});
    caso("RR", {10, 20, 30});
    caso("LR", {30, 10, 20});
    caso("RL", {10, 30, 20});

    // La lista de tu programa original
    Arbol t;
    for (int v : {10, 20, 30, 40, 50, 25, 11, 0, 2, 19, 32, 77, 13, 69, 90, 5, 7, 14}) {
        t.Agregar(v);
        if (!t.esAVL()) { cout << "Falló al insertar " << v << "\n"; return 1; }
    }
    cout << "Tu lista completa, inorder: ";
    t.print();
    t.mostrar();
    cout << (t.esAVL() ? "AVL válido tras cada inserción\n" : "¡NO es AVL!\n");
    return 0;
}
