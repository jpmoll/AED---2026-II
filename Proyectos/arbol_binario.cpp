#include <iostream>
using namespace std;

struct Node {
    int value;
    Node* left;
    Node* right;
    Node(int v) : value(v), left(nullptr), right(nullptr) {}
};

class BST {
    Node* root;

public:
    BST() : root(nullptr) {}

    // FIND: retorna true si encuentra x, y deja p apuntando al nodo (o al nullptr donde debería estar)
    bool find(int x, Node**& p) {
        p = &root;
        while (*p && (*p)->value != x) {
            if (x > (*p)->value)
                p = &((*p)->right);
            else
                p = &((*p)->left);
        }
        return *p != nullptr;  // equivalente a *p != 0, o !!*p
    }

    // INSERT: usa find para ubicar dónde insertar
    bool ins(int x) {
        Node** p;
        if (find(x, p)) return false;  // ya existe
        *p = new Node(x);
        return true;
    }

    // REMOVE: los 3 casos del pizarrón
    bool rem(int x) {
        Node** p;
        if (!find(x, p)) return false;  // no existe

        Node* t = *p;

        if ((*p)->right == nullptr)         // caso 0: no tiene hijo derecho
            *p = (*p)->left;
        else if ((*p)->left == nullptr)     // caso 1: no tiene hijo izquierdo
            *p = (*p)->right;
        else {                              // caso 2: tiene ambos hijos
            // buscar el mayor del subárbol izquierdo (o menor del derecho)
            Node** q = &((*p)->left);
            while ((*q)->right)
                q = &((*q)->right);
            (*p)->value = (*q)->value;
            t = *q;
            *q = (*q)->left;
        }

        delete t;
        return true;
    }

    // Inorder para verificar (da los valores ordenados)
    void inorder(Node* n) {
        if (!n) return;
        inorder(n->left);
        cout << n->value << " ";
        inorder(n->right);
    }

    void print() {
        inorder(root);
        cout << endl;
    }
};

int main() {
    BST tree;

    // El árbol del pizarrón: 55, 42, 61, 11, 44, 57, 68, 58
    tree.ins(55);
    tree.ins(42);
    tree.ins(61);
    tree.ins(11);
    tree.ins(44);
    tree.ins(57);
    tree.ins(68);
    tree.ins(58);

    cout << "Inorder: ";
    tree.print();  // 11 42 44 55 57 58 61 68

    tree.rem(55);
    cout << "Tras rem(55): ";
    tree.print();

    tree.rem(11);
    cout << "Tras rem(11): ";
    tree.print();

    return 0;
}
