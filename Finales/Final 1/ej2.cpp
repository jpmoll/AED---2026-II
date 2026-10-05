// BinTree.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>

using namespace std;



// Nodo del Arbol 
class Node
{
public:
	int key;
	Node* left;
	Node* right;
	int height;
};

int minHconhijos(Node* root, int h = 1);
void cortar(Node* n, int h);
int actualizarH(Node* root);
void borrar(Node* n);

//Calcula la altura del arbol 
int height(Node* N)
{
	if (N == NULL)
		return 0;
	return N->height;
}

//maximo de 2 enteros
int max(int a, int b)
{
	return (a > b) ? a : b;
}

//Crea nuevo nodo
Node* newNode(int key)
{
	Node* node = new Node();
	node->key = key;
	node->left = NULL;
	node->right = NULL;
	node->height = 1; // nuevo nodo 
	// añadido como hoja
	return(node);
}


// Otiene el factor de balance de N 
int getBalance(Node* N)
{
	if (N == NULL)
		return 0;
	return height(N->left) - height(N->right);
}

// Función recursiva que inserta la clave
// en el subarbol con raiz en node y 
// devuelve la nueva raiz del subarbol 
Node* insert(Node* node, int key)
{
	/* 1. Insersión normal en un arbol */
	if (node == NULL)
		return(newNode(key));

	if (key < node->key)
		node->left = insert(node->left, key);
	else if (key > node->key)
		node->right = insert(node->right, key);
	else // no permite valores duplicados 
		return node;

	/* 2. Actualiza la altura desde el ancestro */
	node->height = 1 + max(height(node->left),
		height(node->right));

	return node;
}


//Impresion de arbol en Pre Orden
void preOrder(Node* root)
{
	if (root != NULL)
	{
		cout << root->key << " H: " << root->height << " B: " << getBalance(root) << " HL(" << ((root->left) ? root->left->key : 0) << "): " << height(root->left) << " HR(" << ((root->right) ? root->right->key : 0) << "): " << height(root->right) << endl;
		preOrder(root->left);
		preOrder(root->right);
	}
}

void Podar(Node* root)
{
	//TO DO
	int altura = minHconhijos(root,1);
	cortar(root, altura);
	actualizarH(root);
}

void cortar(Node* n,int h) {
	if (h > 1) {
		cortar(n->left, h - 1);
		cortar(n->right, h - 1);
	}
	else {
		n->height=1;
		Node* tmp0 = n->left;
		Node* tmp1 = n->right;
		if (n->left) n->left = 0;
		if (n->right) n->right = 0;
		borrar(tmp0);
		borrar(tmp1);
	}
}

void borrar(Node* n) {
	if (n) {
		borrar(n->left);
		borrar(n->right);
		delete n;
	}
}

int minHconhijos(Node* root,int h) {
	if (root->left && root->right) {
		h = min(minHconhijos(root->right, h + 1), minHconhijos(root->left,h+1));
	}
	else {
		return h;
	}
}

int actualizarH(Node* root) {
	if (root) {
		root->height = max(actualizarH(root->left) + 1, actualizarH(root->right) + 1);
	}
	else
		return 0;
}

int main()
{
	Node* root = NULL;
	int A[] = { 50,30,80,20,40,60,90,10,25,5,15,28,38,53,65,85,95 };
	int n = sizeof(A) / sizeof(int);


	for (int i = 0; i < n; i++)
	{
		root = insert(root, A[i]);
	}

	cout << "Recorrido preorden del arbol contruido \n";
	preOrder(root);

	Podar(root);

	cout << "\n \n Recorrido preorden del arbol podado \n";
	preOrder(root);


	return 0;
}
