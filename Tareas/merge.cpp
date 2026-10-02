#include <iostream>
#include <stdexcept>
using namespace std;

template <class T>
struct Node{
    T val;
    Node* next;
    Node (T v){
        val = v;
        next = nullptr;
    }
};

template <class T>
class forward_list{
private:

    Node<T>* head;
    int count;

public:
    forward_list(){
        head = nullptr;
        count = 0;
    }
    ~forward_list(){}


    bool vacio(){
        return head == nullptr;
    }

    int size(){
        return count;
    }

    Node<T>*& h(){
        return head;
    }

    // out of one, 0 1 2 3 ...
    T& operator[] (size_t idx){
        if (idx >= count){ 
            throw out_of_range("index fuera de rango");
        }
        Node <T>* temp = head;
        for (size_t i = 0; i < idx; i++){ // < y no != o <= porque tenemos out of one
            temp = temp->next;
        }
        return temp -> val;
    }

    void push_front(T v){
        Node<T>* temp = new Node<T>(v);
        if (vacio()){
            head = temp;
            count++;
            return;
        }
        temp->next = head;
        head = temp;
        count++;
        return;
    }

    void push_back(T v){
        Node<T>* temp = new Node<T>(v);
        if (vacio()){
            head = temp;
            count++;
            return;
        }
        Node<T> **p = &head;
        for (;(*p)->next != nullptr; p = &((*p)->next));
        (*p)->next = temp;
        count++;
        return;
    }

    void pop_front(){
        if (vacio()){
            return;
        }
        Node<T>* temp = head;
        head = head->next;
        delete temp;
        count--;
    }

    void pop_back(){
        if (vacio()){
            return;
        }
        Node<T>** temp = &head;
        for (;(*temp)->next != nullptr; temp = &((*temp)->next));
        delete *temp;
        *temp = nullptr;
        count--;
    }

    void print(){
        for (Node<T>* p = head;p;p=p->next){
            cout << p->val << " -> ";
        }
        cout << "nullptr" << endl;
    }

};

template <class T>
struct Merge{
    void Ejecuta(Node<T>*& L1, Node<T>*& L2) {
        Node<T>** p = &L1;
        while (*p && L2){
            if (L2->val < (*p)->val){
                Node<T>* temp = L2;
                L2 = L2->next;
                temp -> next = *p;
                *p = temp;
            }
            p = &((*p)->next);
        }

        if (L2 != nullptr){
            *p = L2;
            L2 = nullptr;
        }
    }
};


int main (){
    forward_list<int> l1;
    forward_list<int> l2;

    int lista1[] = {1,3,6,9};
    int lista2[] = {1,5,10,15,20};
    

    for (int *i = lista1; i < lista1 + 4; i++){
        l1.push_back(*i);
    }
    
    for (int *i = lista2; i < lista2 + 5; i++){
        l2.push_back(*i);
    }

    l1.print();
    l2.print();

    Merge<int> m1;
    m1.Ejecuta(l1.h(), l2.h());
    l1.print();

    return 0;
}
