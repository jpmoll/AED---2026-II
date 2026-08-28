#include <iostream>
using namespace std;

template <class T>
struct ASC {
    bool operator()(T a, T b) {
        return a < b;
    }
};

template <class T>
struct DES {
    bool operator()(T a, T b) {
        return a > b;
    }
};

template <class T, class O, int N = 10>
class list_ord {
private:
    int tam = N;
    int elem = 0;
    O ord;
    T Arr[N];
public:

    bool find(T val, T* &pos) {
        T* ini = Arr;
        T* end = Arr + elem - 1;

        while (ini <= end) {
            T* mid = ini + (end - ini) / 2;

            if (*mid == val) {
                pos = mid;
                return true;
            } else if (ord(*mid, val)) {
                ini = mid + 1;
            } else {
                end = mid - 1;
            }
        }
        pos = ini;
        return false;
    }

    void add(T e) {
        if (elem >= tam) {
            cout << "Lista llena" << endl;
            return;
        }

        T* pos;
        find(e, pos);

        for (T* p = Arr + elem; p > pos; p--) {
            *p = *(p - 1);
        }

        *pos = e;
        elem++;
    }

    bool del(T e) {
    T* pos;
    if (!find(e, pos)) {
        return false;   // no existe, nada que borrar
    }

    // expandir hacia la izquierda mientras siga siendo igual a e
    T* izq = pos;
    while (izq > Arr && *(izq - 1) == e) {
        izq--;
    }

    // expandir hacia la derecha mientras siga siendo igual a e
    T* der = pos;
    while (der < Arr + elem - 1 && *(der + 1) == e) {
        der++;
    }

    int cantidad = der - izq + 1;   // cuántos elementos iguales hay

    // desplazar todo lo que está después de 'der' hacia 'izq'
    for (T* p = der + 1; p < Arr + elem; p++) {
        *(p - cantidad) = *p;
    }

    elem -= cantidad;
    return true;
}


    void print() {
        for (T* p = Arr; p < Arr + elem; p++) {
            cout << *p << ", ";
        }
        cout << endl;
    }
};

int main() {
    list_ord<int, ASC<int>> lo;
    lo.add(5);
    lo.add(3);
    lo.add(2);
    lo.print();   // 2, 3, 5

    list_ord<int, DES<int>> lo2;
    lo2.add(5);
    lo2.add(3);
    lo2.add(2);
    lo2.print();  // 5, 3, 2
}
