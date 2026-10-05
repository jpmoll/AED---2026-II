#include <iostream>
#include <vector>
using namespace std;

// Definición de la estructura del nodo del árbol
class Nodo {
public:
    int dato;
    Nodo* left;
    Nodo* right;

    // Constructor del nodo
    Nodo(int valor) {
        dato = valor;
        left = right = nullptr;
    }
};

void Order(Nodo* v, vector<int> *qN) {
    if (!v) {
        return;
    }

    Order(v->left,qN);
    (*qN).push_back(v->dato);
    Order(v->right,qN);
}

bool EsSimetrico(Nodo* Raiz)
{
    Nodo* v = Raiz;
    vector<int> vecN1;
    vector<int> vecN2;

    if (v->left->dato != v->right->dato) {
        return false;
    }

    Order(v->left, &vecN1);
    Order(v->right, &vecN2);

    if (size(vecN1) != size(vecN2)) {
        return false;
    }

    for (int i = 0, j = (size(vecN2) - 1); i < size(vecN1); i++, j--) {
        if (vecN1[i] != vecN2[j]) {
            return false;
        }
    }

    return true;
};

int main() {
    // Árbol A
    Nodo* RaizA = new Nodo(1);
    // Nivel 2
    RaizA->left = new Nodo(2);
    RaizA->right = new Nodo(2);
    // Nivel 3
    RaizA->left->left = new Nodo(3);
    RaizA->left->right = new Nodo(4);
    RaizA->right->left = new Nodo(4);
    RaizA->right->right = new Nodo(3);
    // Nivel 4
    RaizA->left->left->left = new Nodo(5);
    RaizA->left->left->right = new Nodo(6);
    RaizA->left->right->left = new Nodo(7);
    RaizA->left->right->right = new Nodo(8);
    RaizA->right->left->left = new Nodo(8);
    RaizA->right->left->right = new Nodo(7);
    RaizA->right->right->left = new Nodo(6);
    RaizA->right->right->right = new Nodo(5);

    // Árbol B
    Nodo* RaizB = new Nodo(1);
    // Nivel 2
    RaizB->left = new Nodo(2);
    RaizB->right = new Nodo(2);
    // Nivel 3
    RaizB->left->right = new Nodo(4);
    RaizB->right->right = new Nodo(4);
    // Nivel 4
    RaizB->left->right->right = new Nodo(6);
    RaizB->right->right->left = new Nodo(6);

    // Árbol C
    Nodo* RaizC = new Nodo(10);
    // Nivel 2
    RaizC->left = new Nodo(5);
    RaizC->right = new Nodo(5);
    // Nivel 3
    RaizC->left->left = new Nodo(7);
    RaizC->left->right = new Nodo(8);
    RaizC->right->left = new Nodo(8);
    RaizC->right->right = new Nodo(7);
    // Nivel 4
    RaizC->left->left->left = new Nodo(1);
    RaizC->left->left->right = new Nodo(2);
    RaizC->left->right->left = new Nodo(3);
    RaizC->left->right->right = new Nodo(4);
    RaizC->right->left->left = new Nodo(4);
    RaizC->right->left->right = new Nodo(3);
    RaizC->right->right->left = new Nodo(2);
    RaizC->right->right->right = new Nodo(1);

    // Árbol D
    Nodo* RaizD = new Nodo(10);
    // Nivel 2
    RaizD->left = new Nodo(5);
    RaizD->right = new Nodo(5);
    // Nivel 3
    RaizD->left->left = new Nodo(7);
    RaizD->right->left = new Nodo(7);
    // Nivel 4
    RaizD->left->left->left = new Nodo(1);
    RaizD->left->left->right = new Nodo(2);
    RaizD->right->left->left = new Nodo(1);
    RaizD->right->left->right = new Nodo(3);

    cout << "Arbol A" << (EsSimetrico(RaizA) ? " es " : " no es ") << "simetrico \n";
    cout << "Arbol B" << (EsSimetrico(RaizB) ? " es " : " no es ") << "simetrico \n";
    cout << "Arbol C" << (EsSimetrico(RaizC) ? " es " : " no es ") << "simetrico \n";
    cout << "Arbol D" << (EsSimetrico(RaizD) ? " es " : " no es ") << "simetrico \n";

    return 0;
}