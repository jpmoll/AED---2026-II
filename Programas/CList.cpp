#include <iostream>
using namespace std;

template <class T>

class nodo{
  public:
    T valor;
    nodo<T>* next;
    nodo<T>* prev;
    nodo(T v, nodo<T>*n = NULL, nodo<T>*p = NULL);
};

template <class T>
nodo<T>::nodo(T v, nodo<T> *n, nodo<T> *p){
  valor = v;
  next = n;
  prev = p;
}

template <class T>
class CList{
  public:
    nodo<T> *head = NULL;
    nodo<T> *tail = NULL;
    void push_back(T a);
    void pop_back();
    void push_front(T a);
    void pop_front();
    void print();
};

template <class T>
void CList<T>::push_back(T a){
    if(head == NULL){
        head = new nodo<T>(a);
        tail = head;
    } else {
        nodo<T>* p = new nodo<T>(a,nullptr, tail);
        tail->next = p;
        tail = p;   
    }
}

template <class T>
void CList<T>::pop_back(){
  if(!head){
    cout << "Lista vacía" << endl;
  } else {
    nodo<T> *p = tail;
    p->prev->next = nullptr;
    p->prev = nullptr;
    delete p;
  }
}

int main(){
    
    return 0;
}