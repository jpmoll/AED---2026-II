#include <iostream>
#include <string>
#include <list>
using namespace std;


struct Node {
    int data;
    Node* prev;
    Node* next;

    Node(int value) {
        data = value;
        prev = nullptr;
        next = nullptr;
    }
};

class DoublyLinkedList {
private:

    Node* head;
    Node* tail;

public:

    DoublyLinkedList() {
        head = nullptr;
        tail = nullptr;
    }

    void eliminar_todos_duplicados(list<int>& l) {
        for (auto it = l.begin(); it != l.end(); ) {
            bool flag = 0;
            for (auto j = next(it); j != l.end(); ) {
                if (*j == *it) {
                    flag = 1;
                    j = l.erase(j);
                }
                else {
                    ++j;
                }
            }
            if (flag) {
                it = l.erase(it);
            }
            else {
                ++it;
            }
        }

    }

    template<class Pred>
    void mi_remove_if(list<int>& l, Pred p) {
        for (auto iter = l.begin(); iter != l.end(); ) {
            if (p(*iter)) {
                iter = l.erase(iter);
            }
            else {
                iter++;
            }
        }
    }

    void print(list<int> const l) {
        for (auto i = l.begin(); i != l.end(); i++) {
            cout << *i << " ";
        }
        cout << endl;
    }

};

struct es_par {
    bool operator()(int n) {
        return n % 2 == 0;
    }
};

void mi_unique(list<int>& l) {
    if (l.empty()) return;
    auto it = l.begin();
    auto sig = next(it);
    while (sig != l.end()) {
        if (*sig == *it) {
            sig = l.erase(sig);
        }
        else
        {
            it = sig;
            sig++;
        }
    }
}

struct es_impar {
    bool operator()(int n) {
        return n % 2 != 0;
    }
};

template <class pred>
void splice_if(list<int>& entrada, list<int>& salida, pred p) {
    for (auto i = entrada.begin(); i != entrada.end();) {
        if (p(*i)) {
            auto sig = next(i);
            salida.splice(salida.end(), entrada, i);
            i = sig;
        }
        else {
            i++;
        }
    }
}

int main() {

    DoublyLinkedList DL;
    list<int> l = { 1,2,3,3,4,5,6,7,7,8,9,1 };
    list<int> s;

    es_impar pred;

    DL.print(l);
    
    splice_if(l, s, pred);

    DL.print(l);
    DL.print(s);



    return 0;
}
