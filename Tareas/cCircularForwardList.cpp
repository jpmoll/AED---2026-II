
#include <iostream>
using namespace std;

template <class T>
struct Node{
    T val;
    Node<T>* next;
    
    Node(T v){
        val = v;
        next = nullptr;
    }
};

template <class T>
class cCList{
private:
    Node<T>* tail = nullptr;
    
public:
    
    cCList(int N){
        char l[] = "ABCDEFGHIJKLMNÑOPQRSTUVWXYZ";
        char* t = l;
        for (int i = 0; i < N && l[i] != '\0'; i++)
            push_back(t[i]);
        // método kill(int N) - cada cuanto va a matar a un elemento
        // kill (3)  matar a C, F, B, ... imprime los elementos en c -> f -> ... mata todo hasta que se queda vacío
    }
    
    void push_back(T v){
        Node<T>* t = new Node<T>(v);
        if (tail == nullptr){
            tail = t;
            tail->next = tail;
            return;
        }
        
        t->next = tail->next;
        tail->next = t;
        tail = t;
        return;
    }
    
    /*
    bool del(int pos){
        
        if (tail == nullptr){
            return 0;
        }
        
        if (tail == tail->next){
            cout << tail->val << endl;
            delete tail;
            tail = nullptr;
            return 1;
        }
        
        Node<T>* prev = tail;
        
        for (int i = 1; i < pos; i++)
                prev = prev->next;
        
        Node<T>* borrar = prev -> next;
        prev->next = borrar ->next;
        
        if (borrar == tail){
            tail = prev;
        }
        
        cout << borrar->val << " -> ";
        
        delete borrar;
        return 1;
    }
    
    void kill(int v){
        bool flag = 1;
        while (flag){
            flag = del(v);
        }
    }
    */
    
    void kill(int v) {
        
        if (tail == nullptr || v <= 0)
            return;

        Node<T>* prev = tail;

        while (tail != nullptr) {
            for (int i = 1; i < v; i++)
                prev = prev->next;

            Node<T>* borrar = prev->next;

            if (borrar == prev) {
                tail = nullptr;
                
            } else {
                
                prev->next = borrar->next;
                
                if (borrar == tail)
                    tail = prev;
            }
            
            cout << borrar->val << " -> ";

            delete borrar;
        }
        cout << "vacio" << endl;
    }
    
    void print(){
        if (tail == nullptr){
            cout << "lista vacía" << endl;
            return;
        }
        Node<T>* t = tail->next;
        while (1){
            cout << t->val << " -> ";
            if (t == tail){
                cout << "head"<< endl;
                return;
            }
            t = t->next;
        }
    }
};

int main() {
    
    cCList<char> c1(6);
    
    c1.print();
    c1.kill(3);
    
    return 0;
}
