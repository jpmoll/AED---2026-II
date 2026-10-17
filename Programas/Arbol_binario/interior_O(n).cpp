#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Node {
    int data;
    Node* left;
    Node* right;

    Node(int value, Node* l = nullptr, Node* r = nullptr) {
        data = value;
        left = l;
        right = r;
    }
};

class BinaryTree {
private:
    Node* root;

    bool es_hoja(Node* p){
        return p && p->left == nullptr && p->right == nullptr;
    }

    void hojas(Node* p, vector<Node*> &v) {
        if (!p) return;
        if (es_hoja(p)) {
            v.push_back(p);
            return;
        }
        hojas(p->left, v);
        hojas(p->right, v);
    }

    void es_izquierdo(Node* p, vector<Node*> &v) {
        if (!p || es_hoja(p)) return;
        v.push_back(p);
        es_izquierdo(p->left ? p->left : p->right, v);
    }

    void es_derecho(Node* p, vector<Node*> &v) {
        if (!p || es_hoja(p)) return;
        v.push_back(p);
        es_derecho(p->right ? p->right : p->left, v);
    }

    void es_arbol(Node* p, vector<Node*>& v) {
        if (!p) return;
        v.push_back(p);
        es_arbol(p->right, v);
        es_arbol(p->left, v);
    }
    //                          root             true               true
    void printNonPerimeter(Node* node, bool isLeftBorder, bool isRightBorder) {
        if (!node) return;

        bool esHoja = node->left == nullptr && node->right == nullptr;

        if (!esHoja && !isLeftBorder && !isRightBorder) {
            cout << node->data << " ";
        }

        printNonPerimeter(node->left,
            isLeftBorder,
            isRightBorder && node->right == nullptr);
        printNonPerimeter(node->right,
            isLeftBorder && node->left == nullptr,
            isRightBorder);

    }

public:
    BinaryTree() {
        root = nullptr;
    }

    void setRoot(Node* node) {
        root = node;
    }

    void printNonPerimeter() {
        printNonPerimeter(root, true, true);
        cout << endl;
    }
};

void runTest(int id, Node* root, const string& expected) {
    BinaryTree tree;
    tree.setRoot(root);

    cout << "===== TEST " << id << " =====" << endl;

    cout << "Salida:   ";
    tree.printNonPerimeter();

    cout << "Esperado: " << expected << endl;
    cout << endl;
}

int main() {

    Node* t1 = new Node(50,
        new Node(30,
            new Node(20,
                new Node(10, nullptr, new Node(15)),
                new Node(25, new Node(22), new Node(28))),
            new Node(40,
                new Node(35, new Node(32), new Node(38)),
                new Node(45))),
        new Node(70,
            new Node(60,
                new Node(55, new Node(52), nullptr),
                new Node(65)),
            new Node(80,
                new Node(75),
                new Node(90, new Node(85), nullptr))));

    runTest(1, t1, "25 35 40 55 60");


    Node* t2 = new Node(8,
        new Node(4,
            new Node(2, new Node(1), new Node(3)),
            new Node(6, new Node(5), new Node(7))),
        new Node(12,
            new Node(10, new Node(9), new Node(11)),
            new Node(14, new Node(13), new Node(15))));

    runTest(2, t2, "6 10");

    Node* t3 = new Node(10,
        new Node(5, new Node(2), new Node(7)),
        new Node(15, nullptr, new Node(20)));

    runTest(3, t3, "(nada)");


    Node* t4 = new Node(10);

    runTest(4, t4, "(nada)");


    runTest(5, nullptr, "(nada)");

    Node* t6 = new Node(10,
        new Node(5,
            new Node(3,
                new Node(1))));

    runTest(6, t6, "(nada)");

    return 0;
}
