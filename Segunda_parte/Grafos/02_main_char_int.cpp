// =====================================================================
//  02_main_char_int.cpp   -->  LEER SEGUNDO
//  (antes: graph_char_int.cpp)
// =====================================================================
//  Graph<char, int>  =  nodos que guardan un char, aristas con peso int.
//
//  Úsalo para PRACTICAR: antes de compilar, predice a mano la salida.
//
//  EL GRAFO QUE SE CONSTRUYE (no dirigido):
//
//            5
//        A ──────── B          ids:  A=0  B=1  C=2  D=3
//        │          │
//      3 │          │ 7
//        │          │
//        C ──────── D
//            2
// =====================================================================

#include "01_graph.h"

int main()
{
    Graph<char, int> g;

    // addNode devuelve el id (0,1,2,3). Los guardamos en a,b,c,d.
    int a = g.addNode('A');   // a = 0
    int b = g.addNode('B');   // b = 1
    int c = g.addNode('C');   // c = 2
    int d = g.addNode('D');   // d = 3

    g.addEdge(a, b, 5);       // A──B peso 5
    g.addEdge(a, c, 3);       // A──C peso 3
    g.addEdge(b, d, 7);       // B──D peso 7
    g.addEdge(c, d, 2);       // C──D peso 2

    cout << "Initial:\n";
    g.print();
    // SALIDA ESPERADA (cada arista aparece 2 veces por ser no dirigido):
    //   A -> B(5) C(3)
    //   B -> A(5) D(7)
    //   C -> A(3) D(2)
    //   D -> B(7) C(2)

    g.removeNode(b);          // borra B (id 1). Ver el ejemplo paso a paso en
                              // 01_graph.h sección [D7]
    cout << "\nAfter removeNode(b):\n";
    g.print();
    // SALIDA ESPERADA (queda A, C, D con ids 0,1,2):
    //
    //        A ──── C ──── D
    //           3       2
    //
    //   A -> C(3)
    //   C -> A(3) D(2)
    //   D -> C(2)

    return 0;
}
