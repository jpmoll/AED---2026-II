#include <iostream>
using namespace std;

template<class T>
struct CNode
{
    CNode(T x)
    {
        value = x; next = 0;
    }
    T value;
    CNode<T>* next;
};

template<class T>
class CList
{
public:
    CList();
    ~CList();
    void push_front(T x);
    void print();
    void Merge(CNode<T>* h1, CNode<T>* h2);
    CNode<T>* head;
};

template<class T>
CList<T>::CList()
{
    head = 0;
}

template<class T>
CList<T>::~CList()
{
}

template<class T>
void CList<T>::push_front(T x)
{
    CNode<T>* t = new CNode<T>(x);
    t->next = head;
    head = t;
}

template<class T>
void CList<T>::Merge(CNode<T>* h1, CNode<T>* h2)
{
    while (h1 != h2) {
        T next = h1->next->value;
        if (h2->value > h1->value && h2->value < next) {
            CNode<T>* h3 = h1;
            for (; h3->next != h2; h3 = h3->next);
            CNode<T>* h4 = h2->next;
            h3->next = h4;
            h2->next = h1->next;
            h1->next = h2;
            h2 = h4;
        }
        else {
            h1 = h1->next;
        }
    }
}

template<class T>
void CList<T>::print()
{
    CNode<T>* p;
    for (p = head; p; p = p->next)
        std::cout << p->value << "[" << p << "]\n";
    std::cout << "-----\n";
}

int main()
{
    CList<int> l;
    CNode<int>* h1, * h2;
    l.push_front(8);
    l.push_front(6);
    l.push_front(4);
    l.push_front(2);
    h2 = l.head;
    cout << "ini h2: " << h2->value << endl;
    l.push_front(7);
    l.push_front(5);
    l.push_front(3);
    l.push_front(1);
    h1 = l.head;
    cout << "ini h1: " << h1->value << endl;
    l.print();
    l.Merge(h1, h2);
    l.print();
}
