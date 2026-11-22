#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <stdexcept>
#include <cassert>
using namespace std;

/*
 * MATRIZ DISPERSA con UN SOLO EJE: un arreglo H de listas enlazadas (una por columna)
 *
 * Solo se guardan los valores DISTINTOS del valor por defecto. Cada nodo vive en UNA
 * sola lista: la de su columna. Ya no existe V (las filas) ni el puntero 'right'.
 *
 * Ejemplo, la misma matriz 3x4 con valor por defecto 0:
 *
 *            col0  col1  col2  col3
 *     fila0:   5     .     .     7
 *     fila1:   .     2     .     .
 *     fila2:   9     .     .     4
 *
 * Cómo queda en memoria. Cada nodo se escribe (x)=valor, donde x = fila.
 * La columna y NO se guarda en el nodo: ya la dice el índice de H.
 *
 *     H[0]        H[1]        H[2]        H[3]
 *      |           |           X           |
 *      v           v                       v
 *    (0)=5       (1)=2                   (0)=7
 *      |           |                       |
 *      v           v                       v
 *    (2)=9        null                   (2)=4
 *      |                                   |
 *      v                                   v
 *     null                                null
 *
 *   H[j] = inicio de la columna j      X = nullptr (columna vacía)
 *    |
 *    v   = puntero 'down' (avanza por la COLUMNA, x crece)
 *
 * Regla de orden: cada columna está ordenada por x ascendente.
 * Eso permite buscar recorriendo solo hasta pasarse de la fila buscada.
 *
 * ---------------------------------------------------------------
 * Comparación con la versión de DOS ejes (V y H):
 *
 *                              | dos ejes (V y H)     | un eje (solo H)
 *   ---------------------------+----------------------+---------------------------
 *   memoria por nodo           | val, x, y, right,    | val, x, down
 *                              | down                 | (más liviano)
 *   get / set (x,y)            | recorre 1 fila       | recorre 1 columna
 *   recorrer la columna y      | rápido               | rápido (es la lista H[y])
 *   recorrer la fila x         | rápido (lista V[x])  | LENTO: hay que mirar las
 *                              |                      | cols columnas una por una
 *   enganchar / desenganchar   | en 2 listas          | en 1 sola lista
 *
 * Moraleja: se elige el eje que más se va a recorrer. Si se necesitan ambos
 * recorridos rápidos, se pagan los dos ejes. Con solo H (o solo V) la estructura
 * es más simple y gasta menos memoria, pero un eje queda "caro".
 *
 * Costo de memoria: O(nodos + columnas) en vez de O(filas * columnas).
 */

template <typename T>
struct Node {
    T val;
    int x;          // fila (la columna es el índice en H)
    Node* down;     // siguiente nodo en la misma COLUMNA (x mayor)

    Node(int x, const T& val) {
        this->val = val;
        this->x = x;
        down = nullptr;
    }
};

template <typename T>
class SMatrix {

private:

    int rows;
    int cols;

    T defaultVal;
    Node<T>** H;    // H[j]: primer nodo de la columna j

    void fuera_limites(int x, int y) const {
        if (x < 0 || x >= rows || y < 0 || y >= cols)
            throw std::out_of_range("Indice fuera de rango");
    }

    /*
     * find: recorre la COLUMNA y buscando la fila x.
     * Devuelve true si existe, y deja p apuntando al PUNTERO que apunta al nodo
     * (o al hueco donde debería ir). Es el mismo truco del find del BST.
     *
     *   Ejemplo A: buscar (2,3) en la columna 3
     *
     *   H[3] --> (0)=7 --> (2)=4 --> null
     *              ^         ^
     *            0<2       2>=2   PARAR: p = &((0)->down)
     *
     *   *p es el nodo (2)=4 y su x (2) == 2  => existe (return true)
     *
     *   Ejemplo B: buscar (1,3) en la misma columna
     *
     *   H[3] --> (0)=7 --> (2)=4 --> null
     *              ^         ^
     *            0<1       2>=1   PARAR: p = &((0)->down)
     *
     *   *p es el nodo (2)=4 y su x (2) != 1  => no existe (return false)
     *   y p es justo el campo que habría que cambiar para insertar (1,3) ahí.
     *
     *   Ejemplo C: buscar (1,2) en la columna 2 (vacía)
     *
     *   H[2] --> null
     *     ^
     *   *p es nullptr: el while ni entra. p = &H[2]  => no existe,
     *   y el "hueco" es el propio H[2].
     */
    bool find(int x, int y, Node<T>** &p) const {
        fuera_limites(x, y);
        p = &H[y];
        while (*p && (*p)->x < x)
            p = &((*p)->down);
        return *p && (*p)->x == x;
    }

    /*
     * insertNode: ahora el nodo nuevo se engancha en UNA sola lista.
     * Recibe p, que ya dejó find (no hace falta buscar de nuevo).
     *
     *   Insertar (1,3)=8 entre (0)=7 y (2)=4:
     *
     *   antes:     H[3] --> (0)=7 --> (2)=4
     *                          ^p  (p = &((0)->down))
     *
     *   nuevo->down = *p   (nuevo apunta a (2)=4)
     *
     *              (0)=7 --> (2)=4
     *                          ^
     *              (1)=8 ------+
     *
     *   *p = nuevo         (el 'down' de (0)=7 apunta al nuevo)
     *
     *   después:   H[3] --> (0)=7 --> (1)=8 --> (2)=4
     *
     *   OJO con el orden: primero nuevo->down = *p y recién después *p = nuevo.
     *   Al revés se perdería el resto de la lista.
     *
     *   El mismo código sirve si p es el propio H[y] (insertar al inicio o en
     *   una columna vacía):
     *
     *   H[2] --> null                  nuevo->down = *p (= nullptr)
     *   p = &H[2]                      *p = nuevo        (= H[2] = nuevo)
     *   H[2] --> (1)=8 --> null
     */
    void insertNode(Node<T>** p, int x, const T& val) {
        Node<T>* nuevo = new Node<T>(x, val);
        nuevo->down = *p;
        *p = nuevo;
    }

    /*
     * removeNode: desenganchar el nodo de SU lista (una sola) antes de borrarlo.
     * Recibe p, que ya dejó find y apunta al campo que apunta al nodo.
     *
     *   quitar (1,3)=8:
     *
     *   H[3] --> (0)=7 --> [(1)=8] --> (2)=4
     *                ^p  (p = &((0)->down), apunta a temp)
     *
     *   temp = *p                  temp es el nodo (1)=8
     *   *p = temp->down            el 'down' de (0)=7 salta a (2)=4
     *
     *   H[3] --> (0)=7 --------------> (2)=4        [(1)=8] queda suelto
     *
     *   delete temp                solo ahora se libera
     *
     *   H[3] --> (0)=7 --> (2)=4
     *
     *   (En la versión de dos ejes había que desenganchar también de la fila.)
     */
    void removeNode(Node<T>** p) {
        Node<T>* temp = *p;
        *p = temp->down;
        delete temp;
    }

public:

    // Una columna vacía se detecta al instante.
    bool columnaVacia(int y) const {
        return H[y] == nullptr;
    }

    /*
     * Una fila vacía NO es inmediata: no existe una lista por fila.
     * Hay que mirar las cols columnas, cada una hasta pasarse de x.
     *
     *   ¿la fila 1 está vacía?   (matriz del diagrama)
     *
     *   H[0]: (0)=5 --> (2)=9     0<1 sigue; 2>=1 pero 2!=1  -> no hay fila 1 aquí
     *   H[1]: (1)=2               1>=1 y 1==1                -> ¡hay algo! return false
     *
     *   Si la fila SÍ está vacía hay que revisar las cols columnas completas.
     *   Costo: O(cols + nodos), contra O(1) de columnaVacia.
     */
    bool filaVacia(int x) const {
        if (x < 0 || x >= rows) throw std::out_of_range("Indice fuera de rango");
        for (int j = 0; j < cols; ++j) {
            Node<T>* n = H[j];
            while (n && n->x < x)
                n = n->down;
            if (n && n->x == x) return false;
        }
        return true;
    }

    SMatrix(int rows, int cols, const T& defaultVal = T())
    : rows(rows), cols(cols), defaultVal(defaultVal) {
        H = new Node<T>*[cols];
        for (int j = 0; j < cols; ++j) H[j] = nullptr;
    }

    // Todos los nodos están en alguna columna: basta recorrer H.
    // (En la versión de dos ejes se recorrían las filas; aquí no existen.)
    //
    //   H[j] --> [n1] --> [n2] --> [n3] --> null
    //             guardar el siguiente ANTES de borrar el actual
    ~SMatrix() {
        for (int j = 0; j < cols; ++j) {
            Node<T>* n = H[j];
            while (n) {
                Node<T>* sig = n->down;
                delete n;
                n = sig;
            }
        }
        delete[] H;
    }

    SMatrix(const SMatrix&) = delete;
    SMatrix& operator=(const SMatrix&) = delete;

    T get(int x, int y) const {
        fuera_limites(x, y);
        Node<T>** p;
        if (find(x, y, p))
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
     * find se llama UNA vez y su p sirve para insertar o borrar.
     */
    void set(int x, int y, const T& val) {
        fuera_limites(x, y);
        Node<T>** p;
        bool existe = find(x, y, p);

        if (existe && val != defaultVal)
            (*p)->val = val;
        else if (!existe && val != defaultVal)
            insertNode(p, x, val);
        else if (existe && val == defaultVal)
            removeNode(p);
    }

    /*
     * printDense: cada celda llama a get, que recorre su columna.
     * Cuesta O(filas * columnas * largo de columna): sirve para mostrar,
     * no para usarla en serio.
     */
    void printDense() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                std::cout << get(i, j) << "\t";
            std::cout << "\n";
        }
    }

    // Muestra lo que REALMENTE hay guardado: solo las listas de columnas.
    // Cada nodo se imprime como (fila)=valor.
    void printEstructura() const {
        cout << "  Columnas (down), cada nodo es (fila)=valor:\n";
        for (int j = 0; j < cols; ++j) {
            cout << "    H[" << j << "] -> ";
            for (Node<T>* n = H[j]; n; n = n->down)
                cout << "(" << n->x << ")=" << n->val << " -> ";
            cout << "null\n";
        }
    }

    // Comprueba los invariantes de la estructura. Devuelve la cantidad de nodos
    // guardados, o -1 si algo está mal:
    //   - cada columna ordenada por x (estrictamente creciente)
    //   - x dentro de [0, rows)
    //   - ningún nodo guarda el valor por defecto
    int verificar() const {
        int total = 0;
        for (int j = 0; j < cols; ++j) {
            int prev = -1;
            for (Node<T>* n = H[j]; n; n = n->down) {
                if (n->x <= prev || n->x >= rows || n->val == defaultVal) return -1;
                prev = n->x;
                total++;
            }
        }
        return total;
    }
};

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
    // 3) Insertar en medio de una columna (un solo enganche)
    //
    //    columna 3 antes:   (0)=7 ------------> (2)=4
    //    insertar (1,3)=8
    //    columna 3 después: (0)=7 --> (1)=8 --> (2)=4
    // ---------------------------------------------------------------
    cout << "=== 3) set(1,3,8): inserta entre (0,3) y (2,3) ===\n";
    m.set(1, 3, 8);
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
    //    columna 1 antes:   H[1] --> (1)=20 --> null
    //    set(1,1,0)
    //    columna 1 después: H[1] --> null      => columnaVacia(1) == true
    //
    //    La fila 1 todavía tiene a (1,3)=8, así que filaVacia(1) == false.
    // ---------------------------------------------------------------
    cout << "=== 5) set(1,1,0): borra el nodo ===\n";
    m.set(1, 1, 0);
    m.printDense();
    cout << "columnaVacia(1) = " << boolalpha << m.columnaVacia(1)
         << ", filaVacia(1) = " << m.filaVacia(1)
         << " (la fila 1 aún tiene a (1,3))\n";
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
    // 7) Otro tipo de dato: double con valor por defecto -1.5
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
    for (int i = 0; i < R; ++i) {
        bool hayAlgo = false;
        for (int j = 0; j < C; ++j) {
            assert(s.get(i, j) == ref[i][j]);
            if (ref[i][j] != 0) { esperados++; hayAlgo = true; }
        }
        assert(s.filaVacia(i) == !hayAlgo);                  // filaVacia (cara) también coincide
    }
    for (int j = 0; j < C; ++j) {
        bool hayAlgo = false;
        for (int i = 0; i < R; ++i) if (ref[i][j] != 0) hayAlgo = true;
        assert(s.columnaVacia(j) == !hayAlgo);
    }
    assert(s.verificar() == esperados);
    cout << "OK: " << R * C << " celdas coinciden; nodos guardados = " << esperados
         << " de " << R * C << " posibles\n";

    return 0;
}
