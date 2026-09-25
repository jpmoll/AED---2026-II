#include <iostream>
#include <assert.h>

struct CNode
{
    CNode(int v)
    {   value = v; next = 0; }
    int value;
    CNode* next;
};

class CForward_list
{
public:
    CForward_list();
    ~CForward_list();
    void push_front(int x);
    void pop_front();
    void push_back(int x);
    void pop_back();
    int& operator[](int i);
    int& front();
    int& back();
    void print();
    
private:
    CNode* head;
    int nelem;
};

CForward_list::CForward_list()
{
    head = 0;
    nelem = 0;
}

CForward_list::~CForward_list()
{
    // borrar elementos
}


void CForward_list::push_front(int x)
{
    CNode* t = new CNode(x);
    t->next = head;
    head = t;
    nelem++;
}

void CForward_list::pop_front()
{
    assert(nelem > 0);
    CNode* t = head;
    head = t->next;
    delete t;
    nelem--;
}

void CForward_list::push_back(int x)
{
    CNode** p;
    for ( p = &head; *p != 0; p = &( (*p)->next ) );
    *p = new CNode(x);
    nelem++;
}

void CForward_list::pop_back()
{
    assert(nelem > 0);
    CNode** p;
    for ( p = &head; (*p)->next != 0; p = &( (*p)->next ) );
    delete *p;
    *p = 0;
    nelem--;
}

int& CForward_list::operator[](int i)
{
    assert(i < nelem);
    CNode* q  = head;
    for ( int k = 0; k < i ;++k, q = q->next );
    return q->value;
}

int& CForward_list::front()
{
    assert( nelem > 0 );
    return head->value;
}

int& CForward_list::back()
{
    CNode* q  = head;
    for ( int k = 0; k < nelem-1; ++k, q = q->next );
    return q->value;
}

void CForward_list::print()
{
    for ( CNode* q = head; q; q = q->next )
        std::cout<<q->value<<" ";
    std::cout<<"\n";
}

int main()
{
    int lista1[] = {1,3,6,9};
    int lista2[] = {1,5,10,15,20};
    
    CForward_list l1;
    CForward_list l2;

    for (int *i = lista1; i < lista1 + 4; i++){
        l1.push_back(*i);
    }
    
    for (int *i = lista2; i < lista2 + 5; i++){
        l2.push_back(*i);
    }

    l1.print();
    l2.print();

    
    std::cout<<"\n";
}
