// =====================================================================
//  04_graph_finds.h   -->  LEER AL FINAL (requiere entender 01 bien)
//  (antes: graph_finds.h)
// =====================================================================
//  graph_finds HEREDA de Graph: tiene todo lo de Graph + 3 métodos nuevos
//  para BUSCAR CAMINOS. Los 3 cuerpos están vacíos (son la parte que hay
//  que implementar); aquí te dejo guías para llenarlos.
//
//      Graph<N,E>
//          ▲
//          │ hereda (public)
//      graph_finds<N,E>  ── findPath
//                        ── precalcAllPaths
//                        ── findPathFromPreCalc
//
//  ORDEN DE LECTURA/IMPLEMENTACIÓN:  [1] herencia  [2] findPath
//                                    [3] precalc   [4] fromPreCalc
// =====================================================================

#pragma once

#include "01_graph.h"

// ---------------------------------------------------------------------
// [1] LA CLASE Y LA HERENCIA
// ---------------------------------------------------------------------
template<typename N, typename E>
class graph_finds : public Graph<N,E>
{
public:
    // Pasa 'dir' al constructor de la clase base Graph
    graph_finds(bool dir = false) : Graph<N,E>(dir) {}

    vector<int> findPath(int from, int to);              // [2]
    void        precalcAllPaths();                       // [3]
    vector<int> findPathFromPreCalc(int from, int to);   // [4]
};

//  ¿POR QUÉ 'this->' ?
//  Graph<N,E> es una plantilla "dependiente": el compilador no sabe qué
//  miembros tiene hasta que se instancia. Por eso, para usar algo heredado
//  (validateVertex, nodes, numVertices) hay que escribir  this->algo.
//  Sin 'this->' da error de "no declarado".
//
//  Como validateVertex es 'protected', la clase hija SÍ puede usarlo.


// ---------------------------------------------------------------------
// [2] findPath: un camino de 'from' a 'to' recorriendo el grafo
// ---------------------------------------------------------------------
//  Devuelve los IDS de los nodos en orden:   from, ..., to
//  Ejemplo en el grafo de 02 (A=0,B=1,C=2,D=3):  findPath(0,3) -> {0,1,3}
//
//  GUÍA (BFS, camino con menos aristas):
//
//     cola = [from]            visitado[from] = true
//     mientras la cola no esté vacía:
//         u = sacar de la cola
//         para cada arista e en this->nodes[u].edges:
//             v = e.to
//             si no visitado[v]:
//                 visitado[v] = true
//                 padre[v] = u           <- "llegué a v desde u"
//                 meter v en la cola
//
//     Reconstruir: partir de 'to' y seguir padre[] hasta 'from',
//     luego invertir el vector.
//
//        padre:  [3]=1  [1]=0        to=3 -> 1 -> 0   invertido: 0,1,3
//
//  Variantes: DFS (pila/recursión) si basta cualquier camino;
//             Dijkstra si importan los PESOS (data de las aristas).
template<typename N, typename E>
vector<int> graph_finds<N,E>::findPath(int from, int to)
{
    this->validateVertex(from);  this->validateVertex(to);
    vector<int> path;

    /* algorithm to find a path between 'from' and 'to', traversing the graph */

    return path;
}


// ---------------------------------------------------------------------
// [3] precalcAllPaths: calcular TODOS los caminos y guardarlos
// ---------------------------------------------------------------------
//  Idea: pagar el costo UNA vez para que luego cada consulta sea O(1).
//
//  GUÍA (dos opciones típicas):
//    a) Correr BFS/Dijkstra desde CADA nodo.
//    b) Floyd-Warshall (3 for anidados).
//
//  En ambos casos guardas una matriz  siguiente[n][n]  (o padre[n][n]):
//
//      siguiente[i][j] = "si estoy en i y quiero ir a j,
//                         ¿a qué nodo doy el primer paso?"
//
//        para el grafo de 02:       j=0  j=1  j=2  j=3
//                         i=0        -    1    2    1
//                         i=1        0    -    0    3
//                         i=2        0    0    -    3
//                         i=3        1    1    2    -
//
//  (necesitarás un atributo nuevo en la clase para guardar esa matriz,
//   p.ej.  vector<vector<int>> siguiente;  declarado en la sección private)
template<typename N, typename E>
void graph_finds<N,E>::precalcAllPaths()
{
    /* algorithm to compute paths between all pairs of nodes and store them in a precomputed structure */
}


// ---------------------------------------------------------------------
// [4] findPathFromPreCalc: leer el camino de la matriz
// ---------------------------------------------------------------------
//  Solo sigue la matriz paso a paso (suponiendo que [3] ya se ejecutó):
//
//     camino = [from]
//     actual = from
//     mientras actual != to:
//         actual = siguiente[actual][to]
//         camino.push_back(actual)
//
//  Ejemplo  from=0, to=3:   0 --siguiente[0][3]=1--> 1 --siguiente[1][3]=3--> 3
//                           camino = {0,1,3}
//
//  Ojo con la nota original: "en O(1)" se refiere a que NO recorre el grafo
//  de nuevo; el armado del vector es proporcional al largo del camino.
template<typename N, typename E>
vector<int> graph_finds<N,E>::findPathFromPreCalc(int from, int to)
{
    vector<int> path;

    /* retrieves the precomputed path between 'from' and 'to' in O(1), assumes precalcAllPaths() was called first */

    return path;
}
