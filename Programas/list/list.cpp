
#include <iostream>
#include <string>
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

    void push_front(int value) {
        Node* newNode = new Node(value);

        if (head == nullptr) {
            head = tail = newNode;
            return;
        }

        head->prev = newNode;
        newNode->next = head;
        head = newNode;
    }

    void push_back(int value) {
        Node* newNode = new Node(value);

        if (head == nullptr) {
            head = tail = newNode;
            return;
        }

        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }

    void pop_front() {
        if (head == nullptr) return;
        Node* tmp = head;
        if (tmp->next == nullptr)
        {
            head = tail = nullptr;
            delete tmp;
            return;
        }

        Node* next = tmp->next;
        next->prev = nullptr;
        head = next;
        delete tmp;
    }

    void pop_back() {
        if (tail == nullptr) return;
        Node* tmp = tail;
        if (tail == head) {
            tail = head = nullptr;
            delete tmp;
            return;
        }

        Node* prev = tail->prev;
        prev->next = nullptr;
        tail = prev;
        delete tmp;
    }

    void insertPos(int pos, int value) {

        if (pos == 0) {
            push_front(value);
            return;
        }

        Node* tmp = head;
        int i = 0;

        for (; tmp && i < pos; i++, tmp = tmp->next);

        if (!tmp) {
            if (i == pos)
                push_back(value);
            return;
        }

        Node* curr = tmp;
        Node* p = new Node(value);

        Node* aCurr = curr->prev;

        p->next = curr;
        p->prev = aCurr;

        aCurr->next = p;
        curr->prev = p;

    }

    bool find(int value, Node* &p) {
        p = head;
        for (; p && p->data != value; p = p->next);
        return p;
    }

    bool eliminar(int value) {
        Node* p = nullptr;
        if (!find(value, p)) {
            return 0;
        }

        if (p == head) {
            pop_front();
            return 1;
        }
        else if (p == tail) {
            pop_back();
            return 1;
        }

        Node* ant = p->prev;
        Node* des = p->next;

        ant->next = des;
        des->prev = ant;
        
        delete p;
        return 1;
    }

    bool eliminar_pos(int pos) {
        if (pos == 0) {
            pop_front();
            return 1;
        }
        Node* p = head;
        int count = 0;
        for (;p && count < pos; count++, p = p->next);

        if (p == nullptr) {
            return 0;
        }

        if (p == tail) {
            pop_back();
            return 1;
        }

        Node* ant = p->prev;
        Node* des = p->next;

        ant->next = des;
        des->prev = ant;

        delete p;
        return 1;
    }

    void eliminar_duplicados() {

        for (Node* p = head;p ;p = p->next) {
            Node* iter = p->next;

            while (iter) {
                if (iter->data == p->data) {
                    Node* borrar = iter;
                    Node* bNext = borrar->next;
                    (borrar->prev)->next = bNext;
                    if (bNext) {
                        bNext->prev = borrar->prev;
                    } 
                    if (borrar == tail) {
                        tail = borrar->prev;
                    }

                    iter = iter->next;
                    delete borrar;
                }
                else {
                    iter = iter->next;
                }
            }
        }
    }

    void eliminar_todos_duplicados() {
        for (Node* p = head; p; ) {
            Node* candidato = p;
            Node* iter = p->next;
            bool flag = 0;
            while (iter) {
                if (candidato->data == iter->data) {
                    flag = 1;
                    Node* borrar = iter;
                    Node* nBorrar = iter->next;
                    (borrar->prev)->next = nBorrar;
                    if (nBorrar) {
                        nBorrar->prev = borrar->prev;
                    } if (borrar == tail) {
                        tail = borrar->prev;
                    }
                    iter = iter->next;
                    delete borrar;
                }
                else {
                    iter = iter->next;
                }
            }
            if (flag) {
                Node* prev = candidato->prev;
                Node* next = candidato->next;
                if (prev) prev->next = next;
                if (next) next->prev = prev;

                if (p == head) head = p->next;
                if (p == tail) tail = tail->prev;
                p = p->next;
                delete candidato;
            }
            else
            {
                p = p->next;
            }
        }
    }

    void invertir() {
        Node* nHead = tail;
        Node* nTail = head;
        for (Node* p = head; p;) {
            Node* ant = p->prev;
            Node* sig = p->next;
            p->next = ant;
            p->prev = sig;
            p = sig;
        }
        head = nHead;
        tail = nTail;
    }

    void rotar(int k) {
        // primeros k nodos al final
        Node* ini = head;
        Node* fin = head;
        for (int i = 0; fin && i < k; i++, fin = fin->next);
        if (!fin) {
            return;
        }
        for (;ini != fin; ) {
            push_back(ini->data);
            ini = ini->next;
            pop_front();
        }

    }

    void print() const {
        Node* current = head;

        while (current != nullptr) {
            cout << current->data;

            if (current->next != nullptr)
                cout << " <-> ";

            current = current->next;
        }

        cout << endl;
    }

    void printBackward() const {
        Node* current = tail;

        while (current != nullptr) {
            cout << current->data;

            if (current->prev != nullptr)
                cout << " <-> ";

            current = current->prev;
        }

        cout << endl;
    }

    void reverseInGroups(int k) {

        if (k <= 1 || head == nullptr) return;

        Node* curr = head;
        Node* nHead = nullptr;
        Node* finGrupoAnterior = nullptr; // último nodo del grupo invertido

        while (curr) {

            // comprobar si quedan al menos k nodos
            Node* tmp = curr;
            int cnt = 0;

            while (tmp && cnt < k){
                tmp = tmp->next;
                cnt++;
            }

            if (cnt < k) { // menos de k nodos restantes: quedan como están
                if (finGrupoAnterior) finGrupoAnterior->next = curr;
                break;
            }

            // invertir k nodos
            Node* prev = nullptr;
            Node* grupoInicio = curr;
            int i = 0;

            while (curr && i < k) {
                Node* nxt = curr->next;
                curr->next = prev;
                prev = curr;
                curr = nxt;
                ++i;
            }

            // prev es la nueva cabeza del grupo invertido
            if (!nHead) nHead = prev;
            if (finGrupoAnterior) finGrupoAnterior->next = prev;

            // el inicio del grupo invertido pasa a ser el final para enlazar luego
            finGrupoAnterior = grupoInicio;
        }

        if (nHead) head = nHead;
    }
};

void runTest(int id, int n, int k, const string& expected) {
    
    DoublyLinkedList list;

    for (int i = 1; i <= n; i++)
        list.push_back(i);

    cout << "===== TEST " << id << " (n = " << n << ", K = " << k << ") =====" << endl;

    cout << "Original:    ";
    list.print();

    list.reverseInGroups(k);

    cout << "Resultado:   ";
    list.print();

    cout << "Esperado:    " << expected << endl;

    cout << "Hacia atras: ";
    list.printBackward();

    cout << endl;
}

int main() {
    
    DoublyLinkedList DL;


    DL.push_back(1);
    DL.push_back(2);
    DL.push_back(3);
    DL.push_back(4);
    DL.push_back(5);
    DL.push_back(6);

    DL.print();

    DL.rotar(2);

    DL.print();

    

    /*
    runTest(1, 8, 3, "3 <-> 2 <-> 1 <-> 6 <-> 5 <-> 4 <-> 7 <-> 8");
    runTest(2, 6, 2, "2 <-> 1 <-> 4 <-> 3 <-> 6 <-> 5");
    runTest(3, 5, 3, "3 <-> 2 <-> 1 <-> 4 <-> 5");
    runTest(4, 5, 10, "1 <-> 2 <-> 3 <-> 4 <-> 5");
    runTest(5, 5, 1, "1 <-> 2 <-> 3 <-> 4 <-> 5");
    runTest(6, 6, 6, "6 <-> 5 <-> 4 <-> 3 <-> 2 <-> 1");
    */


    return 0;
}
