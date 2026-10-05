// CicrcularLinkedList.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>

using namespace std;

class node
{
public:
    int data;
    node* next;
    node(int d, node *n = nullptr){
        data = d;
        next = n;
    }
};

class CircularLinkedList
{
    node* head=NULL;
public:
    void add(int value)
    {
        if(!head){
            head = new node(value);
            head->next = head;
        }else{
            node*p = head;
            node *prev = nullptr;
            while(p->next != head && p->data < value){
                prev = p;
                p = p->next;
            }
            if(p->next==p){
                if(p->data < value){
                    head->next = new node(value, head);
                }else{
                    head = new node(value, p);
                    p->next = head;
                }
            }else{
                if(p->next==head){
                    if(p->data < value){
                        p->next = new node(value, head);
                    }else{
                        prev->next = new node(value, p);
                    }                   
                }else{
                    if(p==head){
                        prev = head;
                        while(prev->next != p){prev = prev->next;}
                        prev->next= new node(value, p);
                        head = prev->next;
                    }else{
                        prev->next = new node(value, p);
                    }
                }
            }
        }
    }

    void del(int value)
    {
        node *p = head;
        node *prev = nullptr;
        for(;p->next != head && p->data != value;prev = p, p = p->next);
        if(p==head){
            if(p->next == p){
                head = nullptr;
                delete p;
                return;
            }else{
                prev = head;
                while(prev->next != p){prev = prev->next;}
                head = p->next;
            }
        }
        node* tmp = p->next;
        prev->next = tmp;
        p->next = p;
        delete p;
    }

    void print()
    {
        int cont = 0;
        node* ptr = head;
        cout << "head->";
        while (head && cont < 1)
        {
            cout << ptr->data<<" -> ";
            ptr = ptr->next;
            if (ptr == head) cont++;
        }
        if (head) cout << ptr->data;
        cout<< " <- head \n ";

    }
};

int main()
{
    int ADD[10] = { 2,4,6,8,10,1,3,5,7,9 };
    int DEL[10] = { 9,7,5,3,1,10,8,6,4,2 };
    CircularLinkedList CLL;
    for (int i = 0; i < 10; i++)
    {
        cout << "ADD " << ADD[i] << endl;
        CLL.add(ADD[i]);
        CLL.print();
    }

    for (int i = 0; i < 10; i++)
    {
        cout << "DEL " << DEL[i] << endl;
        CLL.del(DEL[i]);
        CLL.print();
    }
}