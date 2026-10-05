#include <iostream>
using namespace std;

class Pila{
    int elem[5];
    int * top = NULL;
    public:
        void Print();
        bool Push(int a);
        bool Pop(int &a);
};

void Pila::Print(){
    if (top == nullptr) {
        cout << "La pila está vacía" << endl;
    } else {
        cout << "Top" << endl;
        for (int *ptr = top - 1; ptr >= elem; ptr--) {
            cout << *ptr << endl;
        }
    }
}

bool Pila:: Push(int a){
    int tam = 5;
    if (top < elem + tam){
        if (top == NULL){
            top = elem;
            *top = a;
            top++;
            return true;
        } else {
            *top = a;
            top++;
            return true;
        }
    } else {
        cout << "pila llena, top no apunta dentro del array" << endl;
        return false;
    }
}

bool Pila::Pop(int &a){
    if (top < elem){
        cout << "pila vacia";
        return false;
    } else {
        *top = NULL;
        top--;
        a = *top;
        return true;
    }
}

int main(){
    Pila p;
    p.Push(6);
    p.Push(1);
    p.Push(6);
    p.Push(4);
    p.Push(3);
    p.Print();
    p.Push(0);
    int v_elim;
    while(p.Pop(v_elim)){
        cout << v_elim << " se elimino de la pila" << endl;
    }
    cout << endl;
    return  0;
}