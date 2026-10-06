#include <iostream>
#include <vector>
#include <stack>
using namespace std;

template <class T>
struct Node{

    T val;

    Node* izq;
    Node* der;

    Node(T v){
        val = v;
        izq = nullptr;
        der = nullptr;
    }
};

/*
template <class T>
struct Node{

    T val;
    Node<T>* dir[2];

    Node(T v){
        val = v;
        dir[0] = nullptr;
        dir[1] = nullptr;
    }
};
*/

template <class T>
class BTree {
private:
    Node<T>* root;
public:
    BTree(){
        root = nullptr;
    }
    ~BTree(){}

    bool vacio(){
        return root == nullptr;
    }

    bool find(T v, Node<T>**& p){
        p = &root;
        while (*p && (*p)->val != v){
            if ((*p)->val < v) {
                p = &((*p)->der);
            } else {
                p = &((*p)->izq);
            }
        }
        return *p && (*p)->val == v;
    }

    void insert(T v){
        Node<T>** p = &root;
        
        if (find(v, p)){
            return;
        } else {
            *p = new Node<T>(v);
        }
    }
    
    bool esHoja(Node<T>* n){
        return n && !n->izq && !n->der;
    }
    
    void hojas(Node<T>* n, vector<Node<T>*>& v){
        if (!n) return;
        if (esHoja(n)) {
            v.push_back(n);
            return;
        }
        hojas(n->izq, v);
        hojas(n->der, v);
    }
    
    void esIzquierda(Node<T>* n, vector<Node<T>*>& v){
        if (!n || esHoja(n)) return;
        v.push_back(n);
        esIzquierda(n->izq ? n->izq : n->der, v);
    }
    
    void esDerecha(Node<T>* n, vector<Node<T>*>& v){
        if (!n || esHoja(n)) return;
        esDerecha(n->der ? n->der : n->izq, v);
        v.push_back(n);
    }
    
    void perimetro(){
        
        vector<Node<T>*> v;
        
        if (esHoja(root)){
            v.push_back(root);
        } else {
            v.push_back(root);
            esIzquierda(root->izq ? root->izq : root->der, v);
            hojas(root, v);
            esDerecha(root->der ? root->der : root->izq, v);
        }
        
        cout << "Perimetro : ";
        for (auto x : v)
            cout << x->val << " ";
        
        cout << endl;
        
    }
    
    
    
    void recorrido(Node<T>* n, vector<Node<T>*>& t){
        if(!n) return;
        t.push_back(n);
        recorrido(n->der, t);
        recorrido(n->izq, t);
    }
    
    void interior(){
        
        vector<Node<T>*> v;
        vector<Node<T>*> t;
        
        recorrido(root,t);
        
        if (esHoja(root)){
            v.push_back(root);
        } else {
            v.push_back(root);
            esIzquierda(root->izq ? root->izq : root->der, v);
            hojas(root, v);
            esDerecha(root->der ? root->der : root->izq, v);
        }
        
        cout << "Interior  : ";
        for (auto i : t){
            bool enPerimetro = false;
            for (auto j : v){
                if (i == j){
                    enPerimetro = true;
                    break;
                }
            }
            if (enPerimetro == false){
                cout << i->val;
            }
        }
        cout << endl;
        
    }
    
    void maxh(Node<T>* n, int h, int& mh){
        if (!n) return;
        if (h > mh){
            mh = h;
        }
        maxh( n->der, h + 1 , mh);
        maxh( n->izq, h + 1 , mh);
    }
    
    int maxhr(Node<T>* n){
        if(!n) return 0;
        int l, r;
        l = maxhr(n->izq);
        r = maxhr(n->der);
        return max(l,r)+1;
    }
    
    void inorder(Node<T>* n) {
        if (!n) return;
        
        inorder(n->izq);
        cout << n->val << " ";
        inorder(n->der);

    }
    
    void preorder(Node<T>* n) {
        if (!n) return;
        
        cout << n->val << " ";
        inorder(n->izq);
        inorder(n->der);

    }
    
    void postorder(Node<T>* n) {
        if (!n) return;
        
        inorder(n->izq);
        inorder(n->der);
        cout << n->val << " ";

    }
    
    void print() {
        inorder(root);
        cout << endl;
    }
    
    Node<T>* getRoot(){
        return root;
    }

};

int main(){
    BTree<int> t;
    
    t.insert(15);
    t.insert(10);
    t.insert(20);
    t.insert(8);
    t.insert(12);
    t.insert(17);
    t.insert(22);
    t.insert(26);
    t.insert(16);
    t.insert(3);
    
    cout << "Preorder  : ";
    t.preorder(t.getRoot());
    cout << endl;
    
    cout << "inorder   : ";
    t.inorder(t.getRoot());
    cout << endl;
    
    cout << "postorder : ";
    t.postorder(t.getRoot());
    cout << endl;
    
    t.perimetro();
    t.interior();
    
    // Con void usando referencias
    int alt_max = 0;
    t.maxh(t.getRoot(),1, alt_max);
    cout << alt_max << endl;
    
    // Con retorno
    int a_mx = t.maxhr(t.getRoot());
    cout << a_mx << endl;
    
    return 0;
};
