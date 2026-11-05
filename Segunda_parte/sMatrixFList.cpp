//VERSIÓN CON ADAPTADORES
//coordenadas + valor

#include <iostream>
#include <vector>
#include <forward_list>
#include <stdexcept>

using namespace std;

template <class T>
struct Node;

template <class T>
class SparseMatrix;

template <class T>
struct proxy;

//nodo
template <class T>
struct Node{
	T valor;
	int x, y;
    Node(T v, int _x, int _y): valor(v), x(_x), y(_y) {}
    
    //P(1,2) = 8;
    // T a = P(1,2);
    // P(1,2); "4" <- valor
    
    T& operator()(int x, int y){
        return valor;
    }

};

template <class T>
struct proxy {
    SparseMatrix<T>* sm_ptr;
    int x, y;

    proxy(SparseMatrix<T>* s, int x_, int y_){
        sm_ptr = s;
        x = x_;
        y = y_;
    }

    proxy& operator = (int val){
        sm_ptr->set(val, x, y);
        return *this;
    }

    operator T() {
        return sm_ptr->get(x,y);
    }

};

//sparse_matrix
template <class T>
class SparseMatrix {

private:
	forward_list<Node<T>*> SM;
    int mapa_x, mapa_y;

public:
    SparseMatrix(int m_x, int m_y) {
        mapa_x = m_x;
        mapa_y = m_y;

        //SM inicia vacio porque a la larga va agregando, conforme los limites del mapa        
    }

    bool verificar_rango (int _x, int _y){
        if (_x >= mapa_x || _y >= mapa_y){
            return 0;
        } return 1;
    }

    void set (T val, int x, int y){
        if (!verificar_rango (x,y)){
            return;
        };

        // Logica de buscar si existe un nodo con x y con 
        // y dentro de la forward list
            // encontrar 

        auto prev = SM.before_begin();
        for (auto i = SM.begin(); i != SM.end(); prev = i, i++){
            if ((*i) -> x == x && (*i) -> y == y){ //encontró el nodo
                if(val == 0){ // queremos eliminar
                    delete *i;
                    SM.erase_after(prev);
                } else { // Queremos cambiar el valor
                    (*i)->valor = val;
                    print();
                }
                return;
            }
        }
        Node<T>* nuevo = new Node<T>(val, x, y);
        SM.push_front(nuevo);
        print();
    }

    T get(int x, int y){
        if (!verificar_rango (x,y)){
            //cout << "out of limits" << endl;
            throw out_of_range("coords fuera del mapa");
        };
        
        for (auto i = SM.begin(); i != SM.end(); i++){
            if ((*i) -> x == x && (*i) -> y == y){ //encontró el nodo
                return (*i)->valor; //si existe
            }
        }
        return 0; //no existe
    }

    void print(){
        cout << "FL: ";
        for (auto i = SM.begin(); i != SM.end(); ){
            cout << (*i)->valor << " (" << (*i)->x << ","<< (*i)->y <<") -> ";
            i++;
        }
        cout << endl;
    }

    proxy<T> operator() (int x, int y){
        proxy<T> n(this, x, y);
        return n;
    }

};


int main(){
    SparseMatrix<int> S(2,2);
    S.set(2,1,1); //crea nuevo
    S.set(3,0,1); //crea nuevo
    S.set(4,0,1); //reemplaza
    S.set(0,1,1); //borra

    int p = S.get(0,1);

    cout << "get: " << p << endl << S.get(1,1);

    S(2,1) = 8; //asignar
}
