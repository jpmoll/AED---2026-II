#include <iostream>
using namespace std;

int * mid(int * a, int * b){
    return a + (b - a) /2;
}

void bin_bus(int *ini, int *fin, int *& pos, int n){
    if (n < *ini) {
        pos = ini;
        cout << n << " se insertara al principio del array" << endl;
        return;
    }
    if (n > *fin) {
        pos = fin + 1;
        cout << n << " se insertara al final del array" << endl;
        return;
    }
    int *medio = mid(ini, fin);
    while (ini <= fin) {
        if (*medio == n) {
            pos = medio; 
            return;
        } else if (*medio < n) {
            ini = medio + 1;
        } else {
            fin = medio - 1;
        }
        medio = mid(ini, fin);
    }
    pos = ini;
    cout << n << " se insertara entre " << *(ini - 1) << " y " << *ini << endl;
}

bool bin_bus_del(int *ini, int *fin, int *& pos, int n){
    while (ini <= fin) {
        int *medio = mid(ini, fin);
        if (*medio == n) {
            pos = medio; 
            return true;
        } else if (*medio < n) {
            ini = medio + 1; 
        } else {
            fin = medio - 1; 
        }
    }
    cout << "No se encontro el valor " << n << endl;
    return false; 
}


class elementor{
    int arr[10];
    int tam_arr;
    int tam_act;
    elementor *next;
    int *ini;
    int *fin;
    public:
        void print();
        void insertar_elem(int *&pos, int a);
        void add(int a);
        void del(int a);
        elementor() : tam_arr(10), tam_act(0), next(nullptr), ini(nullptr), fin(nullptr) {
            for(int * i = arr; i < arr + 10; i++){
                *i = -1;
            }
        }
        ~elementor(){
        }
};

void elementor::print(){
    cout << "[ ";
    for(int* p = arr; p  < arr + 10; p++){
        if(*p == -1){
            cout << " - ";
        } else {
            cout << *p << " ";
        }
    }
    cout << " ]" << endl;
}

void elementor::insertar_elem(int *&pos, int a){
    if (pos == fin && *fin <= a) {
        *fin = a;
    } else {
        for (int *i = fin + 1; i > pos; --i) {
            *i = *(i - 1);
        }
        *pos = a;
    }
    tam_act++;
    fin++;
}


void elementor::add(int a){
    if(ini == nullptr){
        ini = arr;
        *ini = a;
        fin = ini;
        tam_act++;
    } else if (ini == fin){
        if(*ini < a){
            *(ini + 1) = a;
            tam_act++;
            fin++;
        } else {
            *(ini + 1) = *ini;
            *ini = a;
            tam_act++;
            fin++;
        }
    } else{
        int *pos = nullptr;
        bin_bus(ini, fin, pos, a);
        insertar_elem(pos, a);
    }
}

void elementor::del(int a){
    int *pos =nullptr;
    if(*ini == a){
        cout << "Eliminando elemento " << *arr << endl;
        for(int *i = arr; i < arr + tam_act; i++){
            *i = *(i + 1);
        }
        tam_act--;
        *(arr + tam_act) = -1;
        fin--;
    } else if(*fin == a){
        cout << "Eliminando elemento " << *fin << endl;
        *fin = -1;
        fin--;
        tam_act--;
    } else if(bin_bus_del(ini, fin, pos, a)){
        cout << "Eliminando elemento " << a << endl;
        for(int *i = pos; i < pos + tam_act; i++){
            *i = *(i + 1);
        }
        tam_act--;
        *(pos + tam_act) = -1;
        fin--;
    }

}

int main(){
    elementor e;
    e.print();
    e.add(6);
    e.print();
    e.add(2);
    e.print();
    e.add(12);
    e.print();
    e.add(0);
    e.print();
    e.add(10);
    e.print();
    e.add(1);
    e.print();
    e.add(9);
    e.print();
    e.add(9);
    e.print();
    e.del(0);
    e.print();
    e.del(12);
    e.print();
    e.del(3);
    e.print();
    e.del(9);
    e.print();
    e.del(2);
    e.print();
    return 0;
}