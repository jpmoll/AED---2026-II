#include <iostream>
using namespace std;

struct Node{
    int v;
    Node * next;
    Node(int val) : v(val), next(nullptr){}
};

class Pila{
    Node * top = nullptr;
    public:
        void print();
        void Push(int a);
        bool Pop(int &a);
};

void Pila::print(){
    Node *p = top;
    cout << "Top -> ";
    while(p){
        cout << p->v << " -> ";
        p = p->next;
    }
    cout << endl;
}

void Pila::Push(int a){
    Node *newN = new Node(a);
    if(top == nullptr){
        top = newN;
    } else {
        newN->next = top;
        top = newN;
    }
}

bool Pila::Pop(int &a){
    if(top == nullptr){
        return false;
    } else {
        Node *tmp = top;
        a = tmp->v;
        top = top->next;
        delete tmp;
        return true;
    }
}

int main(){
    Pila p;
    p.print();
    for(int i = 0; i < 6; i++){
        p.Push(i);
        p.print();
    }
    int v_elim;
    for(int i = 0; i < 7; i++){
        if(p.Pop(v_elim)){
            cout << "eliminando elemento " << v_elim << " de la pila" << endl;
        } else {
            cout << "pila vacia" << endl;
        }
        p.print();
    }
    return 0;
}