#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
using namespace std;

template <typename T>
struct Node {
    T val;
    int x;
    int y;
    Node* down;
    Node* right;

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
    Node<T>** V;
    Node<T>** H;
    
    void fuera_limites(int x, int y) const {
        if (x < 0 || x >= rows || y < 0 || y >= cols)
            throw std::out_of_range("Indice fuera de rango");
    }
        
    bool find_X(int x, int y, Node<T>** &p) const {
        fuera_limites(x, y);
        p = &V[x];
        while (*p && (*p)->y < y)
            p = &((*p)->right);
        return *p && (*p)->y == y;
    }
    
    bool find_Y(int x, int y, Node<T>** &p) const {
        fuera_limites(x, y);
        p = &H[y];
        while (*p && (*p)->x < x)
            p = &((*p)->down);
        return *p && (*p)->x == x;
    }

    void insertNode(int x, int y, const T& val) {
        Node<T>** px;
        Node<T>** py;
        find_X(x, y, px);
        find_Y(x, y, py);
        
        Node<T>* nuevo = new Node<T>(x, y, val);
        
        nuevo->right = *px;  *px = nuevo;
        nuevo->down  = *py;  *py = nuevo;
    }
    
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
    
    ~SMatrix() {
        // TODO: liberar nodos
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
        return defaultVal;
    }
    
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
};
 
// tablero

bool sinVecinos(const SMatrix<int>& m, int x, int y, int n) {
    for (int dx = -1; dx <= 1; dx++)
        
        for (int dy = -1; dy <= 1; dy++) {
            
            if (dx == 0 && dy == 0)
                continue;
            
            int i = x + dx, j = y + dy;
            
            if (i < 0 || i >= n || j < 0 || j >= n)
                continue;
            
            if (m.get(i, j) == 1)
                return false;
        }
    return true;
}

bool colocar(SMatrix<int>& m, int r, int n, mt19937& rng) {
    if (r == n) return true;

    vector<int> orden(n);
    for (int j = 0; j < n; ++j) orden[j] = j;
    shuffle(orden.begin(), orden.end(), rng);

    for (int c : orden) {
        if (!m.filaVacia(r))
            continue;
        if (!m.columnaVacia(c))
            continue;
        if (!sinVecinos(m, r, c, n))
            continue;

        m.set(r, c, 1);
        
        if (colocar(m, r + 1, n, rng))
            return true;
        
        m.set(r, c, 0);
    }
    return false;
}

void generarMinas(SMatrix<int>& m, int n) {
    mt19937 rng(random_device{}());
    colocar(m, 0, n, rng);
}


void imprimirTablero(const SMatrix<int>& m, int n) {
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            cout << (m.get(i, j) == 1 ? '1' : '-') << " ";
        cout << endl;
    }

}

int main() {
    const int n = 10;
    SMatrix<int> tablero(n, n);

    generarMinas(tablero, n);
    imprimirTablero(tablero, n);

    return 0;
}
