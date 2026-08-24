
#include <iostream>
using namespace std;

template <class T>
struct ASC {
    bool operator ()(T a, T b){
        return a < b;
    }
};

template <class T>
struct DES {
    bool operator ()(T a, T b){
        return a > b;
    }
};

template <class T, class O, int N = 10>
class list_ord{
private:
    int tam = N;
    int elem = 0;
    O ord;
    T Arr[N];
public:
    void add (T e){
        if (elem < tam){
            *(Arr + elem) = e;
            elem++;
        } else {
            cout << "Lista llena" << endl;
        }
    };
    bool del (T e){
        // comprobar si solo tiene un valor
        // la busqueda binaria sirve con más de 2 valores
        
        return false;
    };
    void print (){
        for(T* p = Arr; p < Arr + elem; p++){
            cout << *p << ", ";
        }
        cout << endl;
    };
};



int main() {
    list_ord<int, ASC<int>> lo;
    lo.add(5);
    lo.add(3);
    lo.add(2);
    lo.print();
}
