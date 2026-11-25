// =====================================================================
//  03_main_coord_int.cpp   -->  LEER TERCERO
//  (antes: graph_coord_int.cpp)
// =====================================================================
//  Graph<Coord, int>: ahora el DATO del nodo es una estructura (un punto).
//  La lógica del grafo es idéntica; solo cambia el tipo N.
//
//  LO NUEVO AQUÍ:
//   1) Coord necesita operator<< porque print() hace  cout << data.
//      Sin él, no compila.
//   2) Se prueban hasEdge, removeEdge y vertexCount.
//
//  EL GRAFO (las coordenadas son (x,y) de verdad: es un rombo):
//
//                 b [3,4]
//                ╱       ╲
//             5 ╱         ╲ 5
//              ╱           ╲
//      a [0,0]              c [6,0]
//              ╲           ╱
//             5 ╲         ╱ 5
//                ╲       ╱
//                 d [3,-4]
//
//   ids: a=0  b=1  c=2  d=3
// =====================================================================

#include "01_graph.h"

struct Coord
{
    float x;
    float y;
};

// Le "enseña" a cout cómo imprimir un Coord  ->  [x,y]
ostream& operator<<(ostream& os, const Coord& c)
{
    os << "[" << c.x << "," << c.y << "]";
    return os;
}

int main()
{
    Graph<Coord, int> g;

    int a = g.addNode({0, 0});
    int b = g.addNode({3, 4});
    int c = g.addNode({6, 0});
    int d = g.addNode({3, -4});

    g.addEdge(a, b, 5);
    g.addEdge(a, d, 5);
    g.addEdge(b, c, 5);
    g.addEdge(d, c, 5);

    cout << "Initial:\n";
    g.print();
    // [0,0] -> [3,4](5) [3,-4](5)
    // [3,4] -> [0,0](5) [6,0](5)
    // [6,0] -> [3,4](5) [3,-4](5)
    // [3,-4] -> [0,0](5) [6,0](5)

    cout << "\nhasEdge(a, b): " << g.hasEdge(a, b) << "\n";   // 1 (sí hay a──b)
    cout << "hasEdge(b, d): " << g.hasEdge(b, d) << "\n";     // 0 (b y d NO están unidos)

    g.removeEdge(a, b);   // quita a──b (en ambos sentidos)
    cout << "\nAfter removeEdge(a, b):\n";
    g.print();
    // SALIDA ESPERADA (a y b ya no están unidos; queda b ── c ── d ── a):
    //   [0,0] -> [3,-4](5)
    //   [3,4] -> [6,0](5)
    //   [6,0] -> [3,4](5) [3,-4](5)
    //   [3,-4] -> [0,0](5) [6,0](5)
    cout << "hasEdge(a, b): " << g.hasEdge(a, b) << "\n";     // 0

    g.removeNode(d);      // borra d (id 3). Es el ÚLTIMO: no hay que reindexar a nadie,
                          // solo se borran las aristas que apuntaban a él.
    cout << "\nAfter removeNode(d):\n";
    g.print();
    // SALIDA ESPERADA (a quedó sin vecinos: línea vacía tras la flecha):
    //   [0,0] ->
    //   [3,4] -> [6,0](5)
    //   [6,0] -> [3,4](5)

    cout << "\nvertexCount: " << g.vertexCount() << "\n";     // 3

    return 0;
}
