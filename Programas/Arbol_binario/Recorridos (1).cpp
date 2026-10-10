#include <iostream>
#include <vector>
using namespace std;

struct CBinTreeNode
{
    CBinTreeNode(int v);

    CBinTreeNode* nodes[2];
    int value;
};

class CBinTree
{
public:
    CBinTree();
    ~CBinTree();

    CBinTreeNode* getRoot();

    // Inserción y eliminación
    bool Insert(int x);
    bool Remove(int x);

    // Recorridos
    void Print(bool ascendente = false);

    // Altura y caminos
    int Altura();
    vector<CBinTreeNode*> MejorCamino();
    vector<CBinTreeNode*> MejorCaminoRama(bool rama);

    // Nodos de un nivel
    void ImprimirNivel(int nivel);

private:
    bool Find(int x, CBinTreeNode**& p);
    CBinTreeNode** Rep(CBinTreeNode** p);

    void InOrder(CBinTreeNode* n);
    void Reverse(CBinTreeNode* n);

    void Destruir(CBinTreeNode* n);

    void AlturaRec(CBinTreeNode* p, int nivel, int& altura);
    void CaminoRec(CBinTreeNode* p, vector<CBinTreeNode*>& actual, vector<CBinTreeNode*>& mejor, int nivel, int& altura);
    void NivelRec(CBinTreeNode* p, int objetivo, int nivel);

    CBinTreeNode* root;
};

// Imprime un camino como "a -> b -> c".
void ImprimirCamino(const vector<CBinTreeNode*>& camino);


// Crea un nodo hoja con el valor v (ambos hijos en nullptr).
CBinTreeNode::CBinTreeNode(int v) : value(v)
{
    nodes[0] = nodes[1] = nullptr;
}

// Constructor y destructor: el árbol empieza vacío y, al destruirse,
// libera todos sus nodos.
CBinTree::CBinTree()
{
    root = nullptr;
}

CBinTree::~CBinTree()
{
    Destruir(root);
}

void CBinTree::Destruir(CBinTreeNode* n)
{
    if (!n) return;
    Destruir(n->nodes[0]);
    Destruir(n->nodes[1]);
    delete n;
}

// Devuelve la raíz del árbol.
CBinTreeNode* CBinTree::getRoot()
{
    return root;
}

// Find y Rep son las herramientas internas de Insert y Remove.
// Find deja en p la dirección del puntero donde está x (o donde debería ir
// si no existe) y dice si x se encontró. Rep devuelve la dirección del
// reemplazo de un nodo con dos hijos: el menor de su subárbol derecho.
bool CBinTree::Find(int x, CBinTreeNode**& p)
{
    p = &root;

    while (*p && (*p)->value != x)
    {
        if ((*p)->value < x)
            p = &((*p)->nodes[1]);
        else
            p = &((*p)->nodes[0]);
    }

    return *p != nullptr;
}

CBinTreeNode** CBinTree::Rep(CBinTreeNode** p)
{
    CBinTreeNode** q = &(*p)->nodes[1];

    while ((*q)->nodes[0])
        q = &((*q)->nodes[0]);

    return q;
}

// Insert agrega x como hoja en su posición correcta; devuelve false si ya existía.
// Remove elimina x: si tiene un solo hijo lo reemplaza por ese hijo, y si tiene
// dos copia el valor del reemplazo (Rep) y elimina ese nodo. Devuelve false si
// x no existe.
bool CBinTree::Insert(int x)
{
    CBinTreeNode** p;

    if (Find(x, p))
        return false;

    *p = new CBinTreeNode(x);
    return true;
}

bool CBinTree::Remove(int x)
{
    CBinTreeNode** p;

    if (!Find(x, p))
        return false;

    CBinTreeNode* t = *p;

    if (!(*p)->nodes[1])
    {
        *p = (*p)->nodes[0];
    }
    else if (!(*p)->nodes[0])
    {
        *p = (*p)->nodes[1];
    }
    else
    {
        CBinTreeNode** q = Rep(p);
        (*p)->value = (*q)->value;
        t = *q;
        *q = (*q)->nodes[1];
    }

    delete t;
    return true;
}

// InOrder imprime los valores de menor a mayor y Reverse de mayor a menor.
// Print elige el orden con el parámetro ascendente (por defecto, descendente).
void CBinTree::InOrder(CBinTreeNode* n)
{
    if (!n) return;
    InOrder(n->nodes[0]);
    cout << n->value << " ";
    InOrder(n->nodes[1]);
}

void CBinTree::Reverse(CBinTreeNode* n)
{
    if (!n) return;
    Reverse(n->nodes[1]);
    cout << n->value << " ";
    Reverse(n->nodes[0]);
}

void CBinTree::Print(bool ascendente)
{
    if (ascendente)
        InOrder(root);
    else
        Reverse(root);

    cout << "\n";
}

// Altura mide cuántos niveles hay debajo de la raíz: la raíz está en el
// nivel 0, un árbol de un solo nodo tiene altura 0 y uno vacío, -1.
// AlturaRec baja por ambos lados y guarda el nivel más profundo que ve.
void CBinTree::AlturaRec(CBinTreeNode* p, int nivel, int& altura)
{
    if (!p) return;

    if (nivel > altura)
        altura = nivel;

    AlturaRec(p->nodes[0], nivel + 1, altura);
    AlturaRec(p->nodes[1], nivel + 1, altura);
}

int CBinTree::Altura()
{
    int altura = -1;
    AlturaRec(root, 0, altura);
    return altura;
}

// MejorCamino devuelve el camino de la raíz a la hoja más profunda; si hay
// empate gana el de más a la izquierda. MejorCaminoRama hace lo mismo pero solo
// dentro de una rama (0 = izquierda, 1 = derecha), incluyendo la raíz al inicio.
// Ambos usan CaminoRec, que va armando el camino actual mientras baja y lo
// copia a mejor cuando encuentra uno más profundo.
void CBinTree::CaminoRec(CBinTreeNode* p, vector<CBinTreeNode*>& actual, vector<CBinTreeNode*>& mejor, int nivel, int& altura)
{
    if (!p) return;

    actual.push_back(p);

    if (nivel > altura)
    {
        altura = nivel;
        mejor = actual;
    }

    CaminoRec(p->nodes[0], actual, mejor, nivel + 1, altura);
    CaminoRec(p->nodes[1], actual, mejor, nivel + 1, altura);

    actual.pop_back();
}

vector<CBinTreeNode*> CBinTree::MejorCamino()
{
    vector<CBinTreeNode*> actual;
    vector<CBinTreeNode*> mejor;
    int altura = -1;

    CaminoRec(root, actual, mejor, 0, altura);
    return mejor;
}

vector<CBinTreeNode*> CBinTree::MejorCaminoRama(bool rama)
{
    vector<CBinTreeNode*> actual;
    vector<CBinTreeNode*> mejor;
    int altura = -1;

    if (!root) return mejor;

    actual.push_back(root);
    mejor.push_back(root);

    CaminoRec(root->nodes[rama], actual, mejor, 1, altura);
    return mejor;
}

// ImprimirNivel muestra, de izquierda a derecha, los nodos que están en el
// nivel indicado (la raíz es el nivel 0). NivelRec baja por el árbol y se
// detiene al pasarse del nivel pedido.
void CBinTree::NivelRec(CBinTreeNode* p, int objetivo, int nivel)
{
    if (!p || nivel > objetivo) return;

    if (nivel == objetivo)
        cout << p->value << " ";

    NivelRec(p->nodes[0], objetivo, nivel + 1);
    NivelRec(p->nodes[1], objetivo, nivel + 1);
}

void CBinTree::ImprimirNivel(int nivel)
{
    NivelRec(root, nivel, 0);
    cout << "\n";
}

void ImprimirCamino(const vector<CBinTreeNode*>& camino)
{
    for (size_t i = 0; i < camino.size(); i++)
    {
        cout << camino[i]->value;
        if (i + 1 < camino.size())
            cout << " -> ";
    }
    cout << "\n";
}


int main()
{
    CBinTree t;

    int nums[] = {30, 15, 45, 10, 52, 5, 12, 49, 13, 14, 50, 51, 57, 81};
    int total = sizeof(nums) / sizeof(nums[0]);

    cout << "=== Insercion ===" << endl;
    cout << "Insertando: ";
    for (int i = 0; i < total; i++)
    {
        cout << nums[i] << " ";
        t.Insert(nums[i]);
    }
    cout << endl;
    cout << "Insertar 30 de nuevo (repetido): " << (t.Insert(30) ? "se inserto" : "rechazado") << endl;

    cout << "\n=== Recorridos ===" << endl;
    cout << "Descendente: ";
    t.Print();
    cout << "Ascendente:  ";
    t.Print(true);

    cout << "\n=== Altura ===" << endl;
    cout << "Altura del arbol: " << t.Altura() << endl;

    cout << "\n=== Caminos ===" << endl;
    cout << "Mejor camino (raiz a la hoja mas profunda):" << endl;
    ImprimirCamino(t.MejorCamino());
    cout << "Mejor camino por la rama izquierda:" << endl;
    ImprimirCamino(t.MejorCaminoRama(0));
    cout << "Mejor camino por la rama derecha:" << endl;
    ImprimirCamino(t.MejorCaminoRama(1));

    cout << "\n=== Niveles ===" << endl;
    for (int nivel = 0; nivel <= t.Altura(); nivel++)
    {
        cout << "Nivel " << nivel << ": ";
        t.ImprimirNivel(nivel);
    }

    cout << "\n=== Eliminacion ===" << endl;
    cout << "Eliminar 99 (no existe): " << (t.Remove(99) ? "eliminado" : "no encontrado") << endl;
    cout << "Eliminar 5 (hoja): " << (t.Remove(5) ? "eliminado" : "no encontrado") << endl;
    cout << "Eliminar 45 (dos hijos): " << (t.Remove(45) ? "eliminado" : "no encontrado") << endl;
    cout << "Eliminar 30 (la raiz): " << (t.Remove(30) ? "eliminado" : "no encontrado") << endl;
    cout << "Arbol despues de eliminar: ";
    t.Print(true);
    cout << "Nueva altura: " << t.Altura() << endl;

    return 0;
}
