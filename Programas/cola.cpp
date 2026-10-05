#include <iostream>
using namespace std;

class Cola{
    int a[10];
    int *head = nullptr;
    int *tail = nullptr;
    public:
        void print();
        bool Push(int v);
        bool Pop(int &v);
};

void Cola::print() {
    int cont = 0;
    cout << "[ ";
    if(head == a && tail <= a + 9){
        for(int *i = a; i <= tail; i++, cont++){
            cout << *i << " ";
        }
        for(; cont < 10; cont++){
            cout << " - ";
        }
    } else if((head > a && tail <= a+ 9) && head <= tail){
        for(int *i = a; i < head; i++, cont++){
            cout << " - ";
        }
        for(int *i = head; i <= tail; i++, cont++){
            cout << *i << " ";
        }
        for(; cont < 10; cont++){
            cout << " - ";
        }
    } else if(head == nullptr && tail == nullptr){
        for(; cont < 10; cont++){
            cout << " - ";
        }
    } else if(tail < head){
        for(int *i = a; i <= tail; i++, cont++){
            cout << *i << " ";
        }
        for(int *i = tail + 1; i < head; i++, cont++){
            cout << " - ";
        }
        for(int *i = head; i < a + 10; i++, cont++){
            cout << *i << " ";
        }
    }
    cout << "]" << endl;
}


bool Cola::Push(int v){
    if(tail == a + 9 && head == a){
        return false;
    } else {
        if(head == nullptr){
            head = a;
            *head = v;
            tail = head;
            return true;
        } else {
            if(tail < a + 9){
                tail++;
                *tail = v;
                return true;
            } else {
                tail = a;
                *tail = v;
                return true;
            }
        }
    }
}

bool Cola::Pop(int &v){
    if(head == nullptr){
        return false;
    } else {
        v = *head;
        if(head == tail){
            head = nullptr;
            tail = nullptr;
        } else {
            if (head == a + 9){
                head = a;
            } else {
                head++;
            }
        }
        return true;
    }
}

int main(){
    Cola c;
    c.print();
    for(int i = 1; i < 12; i++){
        if(c.Push(i)){
            cout << "se inserto el elemento " << i << endl;
        } else {
            cout << "array lleno" << endl;
        }
        c.print();
    }
    int v;
    for(int i = 0; i < 6; i++){
        if(c.Pop(v)){
            cout << "se elimino el elemento " << v << endl;
        } else {
            cout << "array vacio" << endl;
        }
        c.print();
    }
    for(int i = 0; i < 4; i++){
        if(c.Push(i)){
            cout << "se inserto el elemento " << i << endl;
        } else {
            cout << "array lleno" << endl;
        }
        c.print();
    }
    int x;
    for(int i = 0; i < 9; i++){
        if(c.Pop(v)){
            cout << "se elimino el elemento " << v << endl;
        } else {
            cout << "array vacio" << endl;
        }
        c.print();
    }
    return 0;
}