#include <iostream>
#include <vector>
#include <stack>
#include <limits>
using namespace std;

template <class T>
struct Node {

    T val;

    Node* izq;
    Node* der;

    Node(T v) {
        val = v;
        izq = nullptr;
        der = nullptr;
    }
};

template <class T>
class BTree {
private:
    Node<T>* root;
public:
    BTree() {
        root = nullptr;
    }
    ~BTree() {}

    bool vacio() {
        return root == nullptr;
    }

    bool find(T v, Node<T>**& p) {
        p = &root;
        while (*p && (*p)->val != v) {
            if ((*p)->val < v) {
                p = &((*p)->der);
            }
            else {
                p = &((*p)->izq);
            }
        }
        return *p && (*p)->val == v;
    }

    void insert(T v) {
        Node<T>** p = &root;

        if (find(v, p)) {
            return;
        }
        else {
            *p = new Node<T>(v);
        }
    }

    bool esBST(Node<T>* p, int min, int max) {
        if (!p) return true;
        if (p->val < min || p->val > max) {
            return false;
        }
        return esBST(p->izq, min, p->val) && esBST(p->der, p->val, max);
    }

    void BST() {
        if (esBST(root, INT_MIN, INT_MAX)) {
            cout << "Es BST";
        }
        else {
            cout << "No es BST";
        }
        cout << endl;
    }

    void imprimir_nivel(Node<T>* p, int objetivo, int actual) {
        if (!p) return;

        if (actual == objetivo) {
            cout << p->val << " ";
            return;
        }
        imprimir_nivel(p->izq, objetivo, actual + 1);
        imprimir_nivel(p->der, objetivo, actual + 1);

    }

    void vals_nivel(int nivel) {
        imprimir_nivel(root, nivel, 0);
        cout << endl;
    }

    void cal_altura(Node<T>* p, int& max, int actual) {
        if (!p) return;

        if (actual > max) {
            max = actual;
        }

        cal_altura(p->izq, max, actual + 1);
        cal_altura(p->der, max, actual + 1);
    }

    int altura() {
        int max = 0;
        cal_altura(root, max, 0);
        return max;
    }

    void cal_camino(Node<T>* p, vector<Node<T>*>& mejor, vector<Node<T>*>& actual, int nivel_actual, int& max) {
        if (!p) return;

        actual.push_back(p);

        if (nivel_actual > max) {
            mejor = actual;
            max = nivel_actual;
        }

        cal_camino(p->izq, mejor, actual, nivel_actual + 1, max);
        cal_camino(p->der, mejor, actual, nivel_actual + 1, max);

        actual.pop_back();
    }

    vector<Node<T>*> camino_largo() {
        vector<Node<T>*> actual;
        vector<Node<T>*> mejor;

        int max = -1;

        cal_camino(root, mejor, actual, 0, max);
        return mejor;
    }

    void caminos(Node<T>* p, vector<int>& camino) {
        if (!p) return;

        camino.push_back(p->val);

        bool eshoja = p->der == nullptr && p->izq == nullptr;

        if (eshoja) {
            for (auto i : camino) {
                cout << i << " ";
            } cout << endl;
        }
        else {
            caminos(p->izq, camino);
            caminos(p->der, camino);
        }

        camino.pop_back();
    }

    void print_caminos() {
        vector<int> camino;
        cout << "Caminos" << endl;
        caminos(root, camino);
    }

    bool espejo(Node<T>* p1, Node<T>* p2) {
        if (!p1 && !p2) return true;
        if (!p1 || !p2) return false;

        return p1->val == p2->val &&
            espejo(p1->izq, p2->der) &&   // nodos exteriores
            espejo(p1->der, p2->izq);     // nodos interiores

    }

    void insert_balance(int* v, int min, int max) {
        if (min > max) return;

        int mid = min + ((max - min) / 2);
        insert(v[mid]);

        insert_balance(v, min, mid - 1);
        insert_balance(v, mid + 1, max);
    }
    
};

void VecInsert(BTree<int>& t, int* v, int size)
{
    t.insert_balance(v, 0, size-1);
}

int main(){
    BTree<int> t;
    
    int v[] = { 20,19,18,15,16,17,14,11,13,12,9,7,8,10,6,5,2,4,3,1 };

    VecInsert(t, v, 20);

    t.BST();
    t.vals_nivel(3);
    cout << t.altura() << endl;
    
    vector<Node<int>*> camino = t.camino_largo();
    for (auto i : camino) {
        cout << i->val << " ";
    } cout << endl;

    t.print_caminos();

    return 0;
};
