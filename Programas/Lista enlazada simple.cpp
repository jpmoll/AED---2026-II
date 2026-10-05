#include <iostream>
using namespace std;

template<class T>
struct asc {
	bool operator()(T a, T b) {
		return a > b;
	}
};

template<class T>
struct des {
	bool operator()(T a, T b) {
		return a < b;
	}
};

template <class T>
struct nodo {
	T valor;
	nodo<T>* next;
	nodo(T v, nodo<T>* n = nullptr) {
		valor = v;
		next = n;
	}
};

template <class T, class O>
struct LE {
	nodo<T>* head = nullptr;
	void add(T v);
	void del(T v);
	bool find(T v, nodo<T>*& pos);
	void print();
};

template <class T, class O>
void LE<T, O>::add(T v) {
	nodo<T>* p;
	if (!find(v, p)) {
		if (p == nullptr) {
			head = new nodo<T>(v, head);
		}
		else {
			p->next = new nodo<T>(v, p->next);
		}
	}
	else {
		cout << v << " ya existe" << endl;
	}
}

template <class T, class O>
void LE<T, O>::del(T v) {
	nodo<T>* p;
	if (find(v, p)) {
		nodo<T>* q = p->next;
		p->next = q->next;
		delete q;
	}
	else {
		cout << v << " no existe" << endl;
	}
}

template <class T, class O>
bool LE<T, O>::find(T v, nodo<T>*& pos) {
	O op;
	pos = NULL;
	nodo<T>* p = head;
	for (; p && op(v, p->valor); pos = p, p = p->next);
	if (p && p->valor == v)
		return true;
	return false;
}

template <class T, class O>
void LE<T, O>::print() {
	nodo<T>* p = head;
	cout << "HEAD -> ";
	while (p) {
		cout << p->valor << " -> ";
		p = p->next;
	}
	cout << "NULL" << endl;
}

int main() {

	cout << "ascendente int" << endl;
	LE<int, asc<int>> l1;
	l1.print();
	l1.add(5);
	l1.print();
	l1.add(1);
	l1.print();
	l1.add(3);
	l1.print();
	l1.add(7);
	l1.print();
	l1.del(3);
	l1.print();
	l1.del(15);
	l1.print();
	/*l1.add(15);
	l1.print();
	l1.add(5);
	l1.print();
	l1.add(2);
	l1.print();
	l1.del(1);
	l1.print();
	l1.del(3);
	l1.print();
	l1.del(15);
	l1.print();
	l1.del(1);
	l1.print();*/

	/*
	cout << "descendente int" << endl;
	LE<int, des<int>> l2;
	l2.print();
	l2.add(5);
	l2.print();
	l2.add(1);
	l2.print();
	l2.add(3);
	l2.print();
	l2.add(7);
	l2.print();
	l2.add(15);
	l2.print();
	l2.add(5);
	l2.print();
	l2.add(2);
	l2.print();
	l2.del(1);
	l2.print();
	l2.del(3);
	l2.print();
	l2.del(15);
	l2.print();
	l2.del(1);
	l2.print();
	l2.del(7);
	l2.print();
	l2.del(5);
	l2.print();
	l2.del(2);
	l2.print();

	cout << "ascendente char" << endl;
	LE<char, asc<char>> l3;
	l3.print();
	l3.add('f');
	l3.print();
	l3.add('b');
	l3.print();
	l3.add('z');
	l3.print();
	l3.add('a');
	l3.print();
	l3.del('a');
	l3.print();*/
	return 0;
}