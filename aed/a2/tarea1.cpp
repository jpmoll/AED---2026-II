#include <iostream>
using namespace std;

// PROGRAMACION MODULAR

int* gpVect = NULL; 

struct Vector
{
    int* m_pVect, 
        m_tam_act, 
        m_tam, 
        m_nDelta; 
};


void InitVector(Vector* p, int tam_ini, int delta) {
    p->m_tam_act = 0;         
    p->m_tam = tam_ini; 
    p->m_nDelta = delta;    
    p->m_pVect = (int*)malloc(sizeof(int) * p->m_tam); 
}


void Resize(Vector* p) {
    p->m_pVect = (int*)realloc(p->m_pVect, sizeof(int) * (p->m_tam + p->m_nDelta));
    p->m_tam += p->m_nDelta;
}


void Insert(Vector* p, int elem)
{
    if (p->m_tam_act == p->m_tam) 
        Resize(p); 
    p->m_pVect[p->m_tam_act++] = elem;
}

void Print(Vector* p) {
    for (int i = 0; i < p->m_tam_act; ++i) {
        cout << p->m_pVect[i] << " ";
    }
    cout << endl;
}



// PROGRAMACION ORIENTADA A OBJETOS
class CVector
{
private:
    int* m_pVect; 
    int tam_act;  
    int tam;   
    int m_nDelta;  

    void Resize() {
        int nuevo_tam = tam + m_nDelta;
        int* copia = new int[nuevo_tam];

        for (int i = 0; i < tam_act; i++) {
            copia[i] = m_pVect[i];
        }

        delete[] m_pVect;
        m_pVect = copia;
        tam = nuevo_tam;
    }

public:
    CVector(int delta = 10) {
        tam_act = 0;
        tam = delta;
        m_nDelta = delta;
        m_pVect = new int[tam];
    }

    void Insert(int elem) {
        if (tam_act == tam)
            Resize();
        m_pVect[tam_act++] = elem;
    }

    void Print() {
        for (int i = 0; i < tam_act; i++) {
            cout << m_pVect[i] << " ";
        }
        cout << endl;
    }
};




//PROGRAMACIÓN CON TEMPLATES


#include <iostream>
using namespace std;

template <class T> 
class CTVector {
private:
    T* m_pVect;
    int tam_act, 
        tam, 
        m_nDelta;
    void Resize();

public:
    CTVector(int delta = 10);
    void Insert(T elem);
    void Print() const;
};

// Implementación
template <class T>
CTVector<T>::CTVector(int delta) {
    tam_act = 0;
    tam = delta;
    m_nDelta = delta;
    m_pVect = new T[tam];
}

template <class T>
void CTVector<T>::Resize() {
    int nuevo_tam = tam + m_nDelta;
    T* copia = new T[nuevo_tam];

    for (int i = 0; i < tam_act; i++)
        copia[i] = m_pVect[i];

    delete[] m_pVect;
    m_pVect = copia;
    tam = nuevo_tam;
}

template <class T>
void CTVector<T>::Insert(T elem) {
    if (tam_act == tam)
        Resize();
    m_pVect[tam_act++] = elem;
}

template <class T>
void CTVector<T>::Print() const {
    for (int i = 0; i < tam_act; i++)
        cout << m_pVect[i] << " ";
    cout << endl;
}








int main() {

    //-------------VECTOR CON PROGRAMACION MODULAR-------------------
    Vector myVector;

    InitVector(&myVector, 5, 5);

    Insert(&myVector, 1);
    Insert(&myVector, 2);
    Insert(&myVector, 3);
    Insert(&myVector, 4);
    Insert(&myVector, 5);
    Insert(&myVector, 6); 

    cout << "Vector A : ";
    Print(&myVector);


    //-------------VECTOR CON PROGRAMACION ORIENTADA A OBJETOS-------------------

    CVector Vec2(5);  // Vector con capacidad inicial de 5

    // Insertando elementos
    Vec2.Insert(7);
    Vec2.Insert(8);
    Vec2.Insert(15);
    Vec2.Insert(26);
    Vec2.Insert(2);
    Vec2.Insert(0);

    cout << "Vector B : ";
    Vec2.Print();


    //-------------VECTOR CON PROGRAMACION CON TEMPLATES-------------------

    CTVector<char> Vec3(5);

    Vec3.Insert('a');
    Vec3.Insert('C');
    Vec3.Insert('x');
    Vec3.Insert('Y');
    Vec3.Insert('a');
    Vec3.Insert('L');

    cout << "Vector C : ";
    Vec3.Print();

    return 0;
}
