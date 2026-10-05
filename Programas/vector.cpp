#include <iostream>
using namespace std;

class Vector{
    int *arr;
    int tam_arr;
    int tam_act;
    public:
        void ini_arr();
        void duplicar_arr();
        void recortar_arr();
        void push_back(int a);
        void pop_back();
        void push_front(int a);
        void pop_front();
        void print();
        int &operator[](int a);
        Vector(int valor_tam) : arr(nullptr), tam_arr(valor_tam), tam_act(0){
            arr = new int[tam_arr];
            ini_arr();
        }
        ~Vector(){
            delete []arr;
        }
};

void Vector::ini_arr(){
    for (int *i = arr; i < arr + tam_arr; i++){
        *i = -1;
    }
}

void Vector::duplicar_arr(){
    cout << "ARRAY LLENO, creando copia y duplicando tamanho" << endl;
    tam_arr *= 2;
    int *p = new int[tam_arr];
    int *copia = p;
    int *i = arr;
    while (i < arr + tam_act) {
        *copia++ = *i++;
    }
    while (copia < p + tam_arr) {
        *copia++ = -1;
    }
    delete[] arr;
    arr = p;
}

void Vector::recortar_arr(){
    cout << "La mitad del array esta vacia" << endl;
    tam_arr /= 2;
    int *p = new int[tam_arr];
    int *copia = p;
    int *i = arr;
    int *fin = arr + tam_act;
    while (i < fin) {
        *copia++ = *i++;
    }
    delete[] arr;
    arr = p;
}


void Vector::push_back(int a){
    if (tam_act == tam_arr) {
            duplicar_arr();
    }
    *(arr + tam_act++) = a;
}

void Vector::pop_back(){
    int *p = arr;
    while(*p != -1){
        p++;
    }
    p--;
    cout << "Eliminando elemento " << *p << endl;
    *p = -1;
    tam_act--;
    if(tam_act == (tam_arr / 2)){
        recortar_arr();
    }
}

void Vector::push_front(int a){
    if(tam_act == tam_arr){
        duplicar_arr();
    }
    for(int *i = arr + tam_act; i > arr; --i){
        *i = *(i - 1);
    }
    *arr = a;
    tam_act++;
}

void Vector::pop_front(){
    cout << "Eliminando elemento " << *arr << endl;
    for(int *i = arr; i < arr + tam_act; i++){
        *i = *(i + 1);
    }
    tam_act--;
    *(arr + tam_act) = -1;
    if(tam_act == (tam_arr / 2)){
        recortar_arr();
    }
}

void Vector::print(){
    cout << "[ ";
    for (int *i = arr; i < arr + tam_arr; i++){
        if(*i == -1){
            cout << "- ";
        } else {
            cout << *i << " ";
        }
    }
    cout << "]" << endl;
}

int &Vector::operator[](int a){
    return arr[a];
}

int main(){
    Vector mV(5);
    mV.push_back(6);
    mV.print();
    mV.push_back(2);
    mV.print();
    mV.push_back(9);
    mV.print();
    mV.push_back(10);
    mV.print();
    mV.push_back(8);
    mV.print();
    mV.push_back(25);
    mV.print();
    mV.pop_back();
    mV.print();
    mV.pop_back();
    mV.print();
    mV.push_front(1);
    mV.print();
    mV.push_front(98);
    mV.print();
    mV.pop_front();
    mV.print();
    mV.pop_front();
    mV.print();
    return 0;
}