#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <cassert>
using namespace std;

/*
 * MATRIZ DISPERSA con listas enlazadas cruzadas (orthogonal lists)
 *
 * Solo se guardan los valores DISTINTOS del valor por defecto. Cada nodo vive en DOS listas
 * a la vez: la de su fila (puntero right) y la de su columna (puntero down).
 *
 * Ejemplo, matriz 3x4 con valor por defecto 0:
 *
 *            col0  col1  col2  col3
 *     fila0:   5     .     .     7
 *     fila1:   .     2     .     .
 *     fila2:   9     .     .     4
 *
 * Cómo queda en memoria (x = fila, y = columna; cada nodo es (x,y)=valor):
 *
 *             H[0]        H[1]      H[2]      H[3]
 *               |           |         X         |
 *               v           v                   v
 *   V[0] --> (0,0)=5 -------|--------------->(0,3)=7 --> null
 *               |                               |
 *               |           v                   |
 *   V[1] -------|------> (1,1)=2   --> null     |
 *               |                               |
 *               v                               v
 *   V[2] --> (2,0)=9 ----------------------->(2,3)=4 --> null
 *
 *   H[j] = inicio de la columna j      V[i] = inicio de la fila i      X = nullptr (vacía)
 *
 *   ---> = puntero 'right' (avanza por la FILA, y crece)
 *    |
 *    v   = puntero 'down'  (avanza por la COLUMNA, x crece)
 *   (un '-|-' es un cruce de líneas, no una conexión)
 *
 * Regla de orden: cada fila está ordenada por y ascendente, cada columna por x ascendente.
 * Eso permite buscar recorriendo solo hasta pasarse del valor buscado.
 *
 * Costo: memoria O(nodos + filas + columnas) en vez de O(filas * columnas).
 */

template <typename T>
struct Node {
    T val;
    int x;          // fila
    int y;          // columna
    Node* down;     // siguiente nodo en la misma COLUMNA (x mayor)
    Node* right;    // siguiente nodo en la misma FILA (y mayor)

    Node(int x, int y, const T& val){
        this -> val = val;
        this -> x = x;
        this -> y = y;
        down = nullptr;
        right = nullptr;
    }
};

template <typename T>
class SMatrix {

private:

    int rows;
    int cols;

    T defaultVal;
    Node<T>** V;    // V[i]: primer nodo de la fila i
    Node<T>** H;    // H[j]: primer nodo de la columna j

    void fuera_limites(int x, int y) const {
        if (x < 0 || x >= rows || y < 0 || y >= cols)
            throw std::out_of_range("Indice fuera de rango");
    }

    /*
     * find_X: recorre la FILA x buscando la columna y.
     * Devuelve true si existe, y deja p apuntando al PUNTERO que apunta al nodo
     * (o al hueco donde debería ir). Es el mismo truco del find del BST.
     *
     *   buscar (1,3) en la fila 1:
     *
     *   V[1] --> (1,0) --> (1,1) --> (1,4) --> null
     *              ^         ^         ^
     *            y=0<3     y=1<3     y=4>=3  PARAR: p = &((1,1)->right)
     *
     *   *p es el nodo (1,4): su y (4) != 3  => no existe (return false)
     *   y p es justo el campo que habría que cambiar para insertar (1,3) ahí.
     *   Si hubiera sido (1,4): *p->y == 4 => existe (return true).
     */
    bool find_X(int x, int y, Node<T>** &p) const {
        fuera_limites(x, y);
        p = &V[x];
        while (*p && (*p)->y < y)
            p = &((*p)->right);
        return *p && (*p)->y == y;
    }

    /*
     * find_Y: lo mismo pero por la COLUMNA y, avanzando con 'down' hasta x.
     *
     *   H[3]
     *    |
     *    v
     *   (0,3) --> (2,3) --> (5,3) --> null        buscar x=4
     *     ^         ^         ^
     *   0<4       2<4       5>=4  PARAR: p = &((2,3)->down)
     */
    bool find_Y(int x, int y, Node<T>** &p) const {
        fuera_limites(x, y);
        p = &H[y];
        while (*p && (*p)->x < x)
            p = &((*p)->down);
        return *p && (*p)->x == x;
    }

    /*
     * insertNode: el nodo nuevo se ENGANCHA EN DOS LISTAS A LA VEZ.
     *
     *   insertar (1,2)=8 entre (1,1) y (1,4):
     *
     *   FILA 1  (find_X dejó px en el 'right' de (1,1)):
     *     antes:   (1,1) --> (1,4)
     *                    ^px
     *     después: (1,1) --> (1,2) --> (1,4)
     *                  nuevo->right = *px  (apunta a (1,4))
     *                  *px = nuevo         (el 'right' de (1,1) apunta al nuevo)
     *
     *   COLUMNA 2 (find_Y dejó py en el 'down' de (0,2), por ejemplo):
     *     antes:   (0,2) --> (3,2)
     *     después: (0,2) --> (1,2) --> (3,2)
     *                  nuevo->down = *py ;  *py = nuevo
     *
     *   OJO con el orden: primero nuevo->right = *px y recién después *px = nuevo.
     *   Al revés se perdería el resto de la lista.
     */
    void insertNode(int x, int y, const T& val) {
        Node<T>** px;
        Node<T>** py;
        find_X(x, y, px);
        find_Y(x, y, py);

        Node<T>* nuevo = new Node<T>(x, y, val);

        nuevo->right = *px;  *px = nuevo;
        nuevo->down  = *py;  *py = nuevo;
    }

    /*
     * removeNode: hay que DESENGANCHAR el nodo de sus DOS listas antes de borrarlo.
     *
     *   quitar (1,2):
     *
     *   FILA:     (1,1) --> [(1,2)] --> (1,4)      px apunta al campo que apunta a temp
     *                  *px = temp->right   =>  (1,1) --> (1,4)
     *
     *   COLUMNA:  (0,2) --> [(1,2)] --> (3,2)      py igual
     *                  *py = temp->down    =>  (0,2) --> (3,2)
     *
     *   Recién entonces delete temp. Si se olvida una de las dos listas, queda un
     *   puntero colgante (apunta a memoria liberada).
     */
    void removeNode(int x, int y) {
        Node<T>** px;
        Node<T>** py;
        if (!find_X(x, y, px)) return;
        find_Y(x, y, py);

        Node<T>* temp = *px;
        *px = temp->right;
        *py = temp->down;
        delete temp;
    }



public:

    bool filaVacia(int x) const{
        return V[x] == nullptr;
    }

    bool columnaVacia(int y) const {
        return H[y] == nullptr;
    }

    SMatrix(int rows, int cols, const T& defaultVal = T())
    : rows(rows), cols(cols), defaultVal(defaultVal) {
        V = new Node<T>*[rows];
        H = new Node<T>*[cols];
        for (int i = 0; i < rows; ++i) V[i] = nullptr;
        for (int j = 0; j < cols; ++j) H[j] = nullptr;
    }

    // (CAMBIO: antes era un TODO y se filtraban todos los nodos)
    // Todos los nodos están en alguna fila, así que basta recorrer las filas.
    // No hace falta tocar las columnas: son los mismos nodos.
    //
    //   V[i] --> [n1] --> [n2] --> [n3] --> null
    //             guardar el siguiente ANTES de borrar el actual
    ~SMatrix() {
        for (int i = 0; i < rows; ++i) {
            Node<T>* n = V[i];
            while (n) {
                Node<T>* sig = n->right;
                delete n;
                n = sig;
            }
        }
        delete[] V;
        delete[] H;
    }

    SMatrix(const SMatrix&) = delete;
    SMatrix& operator=(const SMatrix&) = delete;

    T get(int x, int y) const {
        fuera_limites(x, y);
        Node<T>** p;
        if (find_X(x, y, p))
            return (*p)->val;
        return defaultVal;          // lo que no está guardado vale el valor por defecto
    }

    /*
     * set: 3 casos según (¿existe el nodo?) y (¿el valor es el de por defecto?)
     *
     *                      | val != default        | val == default
     *   -------------------+-----------------------+-------------------
     *   ya existe          | actualizar val        | BORRAR el nodo
     *   no existe          | INSERTAR el nodo      | no hacer nada
     *
     * Así un valor "igual al default" nunca ocupa memoria.
     */
    void set(int x, int y, const T& val) {
        fuera_limites(x, y);
        Node<T>** p;
        bool existe = find_X(x, y, p);

        if (existe && val != defaultVal)
            (*p)->val = val;
        else if (!existe && val != defaultVal)
            insertNode(x, y, val);
        else if (existe && val == defaultVal)
            removeNode(x, y);
    }

    void printDense() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                std::cout << get(i, j) << "\t";
            std::cout << "\n";
        }
    }

    // (NUEVO) Muestra lo que REALMENTE hay guardado: las listas de filas y de columnas.
    void printEstructura() const {
        cout << "  Filas (right):\n";
        for (int i = 0; i < rows; ++i) {
            cout << "    V[" << i << "] -> ";
            for (Node<T>* n = V[i]; n; n = n->right)
                cout << "(" << n->x << "," << n->y << ")=" << n->val << " -> ";
            cout << "null\n";
        }
        cout << "  Columnas (down):\n";
        for (int j = 0; j < cols; ++j) {
            cout << "    H[" << j << "] -> ";
            for (Node<T>* n = H[j]; n; n = n->down)
                cout << "(" << n->x << "," << n->y << ")=" << n->val << " -> ";
            cout << "null\n";
        }
    }

    // (NUEVO) Comprueba los invariantes de la estructura. Devuelve la cantidad de nodos
    // guardados, o -1 si algo está mal:
    //   - cada fila ordenada por y, cada columna ordenada por x
    //   - cada nodo en la lista correcta (fila x / columna y)
    //   - la misma cantidad de nodos contando por filas que por columnas
    //   - ningún nodo guarda el valor por defecto
    int verificar() const {
        int porFilas = 0, porCols = 0;
        for (int i = 0; i < rows; ++i) {
            int prev = -1;
            for (Node<T>* n = V[i]; n; n = n->right) {
                if (n->x != i || n->y <= prev || n->val == defaultVal) return -1;
                prev = n->y;
                porFilas++;
            }
        }
        for (int j = 0; j < cols; ++j) {
            int prev = -1;
            for (Node<T>* n = H[j]; n; n = n->down) {
                if (n->y != j || n->x <= prev) return -1;
                prev = n->x;
                porCols++;
            }
        }
        return porFilas == porCols ? porFilas : -1;
    }
};

// (CAMBIO: había un '}' sobrante aquí que impedía compilar)

int main() {
    // ---------------------------------------------------------------
    // 1) La matriz del diagrama de arriba
    // ---------------------------------------------------------------
    cout << "=== 1) Matriz 3x4 del diagrama ===\n";
    SMatrix<int> m(3, 4, 0);
    m.set(0, 0, 5);
    m.set(0, 3, 7);
    m.set(1, 1, 2);
    m.set(2, 0, 9);
    m.set(2, 3, 4);

    cout << "Densa:\n";
    m.printDense();
    cout << "Estructura guardada (solo 5 nodos, no 12 celdas):\n";
    m.printEstructura();
    cout << "nodos = " << m.verificar() << "\n\n";

    // ---------------------------------------------------------------
    // 2) get con y sin valor guardado
    // ---------------------------------------------------------------
    cout << "=== 2) get ===\n";
    cout << "get(0,3) = " << m.get(0, 3) << "  (guardado)\n";
    cout << "get(1,2) = " << m.get(1, 2) << "  (no guardado -> valor por defecto)\n\n";

    // ---------------------------------------------------------------
    // 3) Insertar en medio de una fila y de una columna (enganche en 2 listas)
    //
    //    fila 2 antes:   (2,0)=9 ------------> (2,3)=4
    //    insertar (2,1)=6
    //    fila 2 después: (2,0)=9 --> (2,1)=6 --> (2,3)=4
    //    columna 1 después: (1,1)=2 --> (2,1)=6
    // ---------------------------------------------------------------
    cout << "=== 3) set(2,1,6): inserta entre (2,0) y (2,3) ===\n";
    m.set(2, 1, 6);
    m.printEstructura();
    cout << "nodos = " << m.verificar() << "\n\n";

    // ---------------------------------------------------------------
    // 4) Actualizar un valor que ya existe (no crea nodo)
    // ---------------------------------------------------------------
    cout << "=== 4) set(1,1,20): actualiza, nodos sigue igual ===\n";
    m.set(1, 1, 20);
    cout << "get(1,1) = " << m.get(1, 1) << ", nodos = " << m.verificar() << "\n\n";

    // ---------------------------------------------------------------
    // 5) Poner el valor por defecto = BORRAR el nodo
    //
    //    fila 1 antes:   V[1] --> (1,1)=20 --> null
    //    set(1,1,0)
    //    fila 1 después: V[1] --> null      => filaVacia(1) == true
    // ---------------------------------------------------------------
    cout << "=== 5) set(1,1,0): borra el nodo ===\n";
    m.set(1, 1, 0);
    m.printDense();
    cout << "filaVacia(1) = " << boolalpha << m.filaVacia(1)
         << ", columnaVacia(1) = " << m.columnaVacia(1)
         << " (la col 1 aún tiene a (2,1))\n";
    cout << "nodos = " << m.verificar() << "\n\n";

    // ---------------------------------------------------------------
    // 6) Índices fuera de rango
    // ---------------------------------------------------------------
    cout << "=== 6) fuera de rango ===\n";
    try {
        m.get(3, 0);
    } catch (const out_of_range& e) {
        cout << "Excepción atrapada: " << e.what() << "\n\n";
    }

    // ---------------------------------------------------------------
    // 7) Otro tipo de dato: double con valor por defecto -1
    // ---------------------------------------------------------------
    cout << "=== 7) SMatrix<double> con default -1.5 ===\n";
    SMatrix<double> d(2, 3, -1.5);
    d.set(0, 1, 3.25);
    d.printDense();
    cout << "\n";

    // ---------------------------------------------------------------
    // 8) Prueba aleatoria: la matriz dispersa debe comportarse IGUAL que una
    //    matriz densa (vector de vectores) tras miles de operaciones.
    // ---------------------------------------------------------------
    cout << "=== 8) Prueba aleatoria contra una matriz densa ===\n";
    const int R = 30, C = 25;
    SMatrix<int> s(R, C, 0);
    vector<vector<int>> ref(R, vector<int>(C, 0));

    mt19937 gen(12345);
    uniform_int_distribution<int> fila(0, R - 1), col(0, C - 1), valor(0, 9);  // 0 = default: borra

    for (int k = 0; k < 20000; ++k) {
        int x = fila(gen), y = col(gen), v = valor(gen);
        if (v < 7) v = 0;                 // ~70% de los set son 'poner el default' (borrar / no-op)
        s.set(x, y, v);
        ref[x][y] = v;
        if (k % 1000 == 0) assert(s.verificar() >= 0);       // estructura sana en el camino
    }

    int esperados = 0;
    for (int i = 0; i < R; ++i)
        for (int j = 0; j < C; ++j) {
            assert(s.get(i, j) == ref[i][j]);
            if (ref[i][j] != 0) esperados++;
        }
    assert(s.verificar() == esperados);
    cout << "OK: " << R * C << " celdas coinciden; nodos guardados = " << esperados
         << " de " << R * C << " posibles\n";

    return 0;
}
