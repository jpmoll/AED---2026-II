#include <iostream>
using namespace std;

struct Node{
    int v;
    Node * next;
    Node(int val) : v(val), next(nullptr){}
};

class Cola{
    Node * head = nullptr;
    Node * tail = nullptr;
    public:
        void print();
        void Push(int a);
        bool Pop(int &a);
};

void Cola::print(){
    cout << "Head -> ";
    Node * p = head;
    while(p){
        cout << p->v << " -> ";
        p = p->next;
    }
    cout << " <- Tail" << endl;
}

void Cola::Push(int a){
    Node *newN = new Node(a);
    if(head == nullptr){
        head = newN;
        tail = newN;
    } else {
        tail->next = newN;
        tail = newN;
    }
}

bool Cola::Pop(int &a){
    Node *tmp;
    if(head == nullptr){
        return false;
    } else {
        tmp = head;
        a = head->v;
        head = head->next;
        delete tmp;
        return true;
    }
}

int main(){
    Cola c;
    for(int i = 0; i < 8; i++){
        c.print();
        c.Push(i);
    }
    int v;
    for(int i = 0; i < 9; i++){
        if(c.Pop(v)){
            cout << "eliminando elemento " << v << endl;
        } else {
            cout << "lista vacia" << endl;
        }
        c.print();
    }
    return 0;
}