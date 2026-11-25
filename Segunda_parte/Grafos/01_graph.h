// =====================================================================
//  01_graph.h   -->  LEER PRIMERO  (es la base de todo)
// =====================================================================
//
//  ORDEN DE LECTURA DE ESTE ARCHIVO:
//    [A] Edge        : una arista (flecha)
//    [B] Node        : un nodo con su lista de aristas
//    [C] Graph       : el grafo = vector de nodos
//    [D] Operaciones : addNode -> addEdge -> hasEdge -> removeEdge
//                      -> print -> removeNode (la más difícil, al final)
//
//  IDEA CLAVE (si entiendes solo esto, entiendes el 80%):
//
//     Graph
//       └── vector<Node> nodes          <- la POSICIÓN en el vector ES el id del nodo
//              ├── [0] Node ── list<Edge> edges
//              ├── [1] Node ── list<Edge> edges
//              └── [2] Node ── list<Edge> edges
//
//  Esto se llama LISTA DE ADYACENCIA: cada nodo guarda solo SUS vecinos.
// =====================================================================

#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <cassert>
using namespace std;


// =====================================================================
// [A] EDGE: una arista
// =====================================================================
//  Dibujo:      from ───────(data)──────► to
//
//  - from / to  : son ÍNDICES de nodos (posiciones en el vector), no datos.
//  - data (E)   : lo que cuesta/pesa la arista (en los ejemplos, un int).
//
//  Template <E>: E es el TIPO del peso (int, float, string...).
template<typename E>
class Edge
{
public:
    int from;   // índice del nodo de origen
    int to;     // índice del nodo de destino
    E   data;   // peso / información de la arista

    Edge(int f, int t, E d)
    {
        from = f;
        to   = t;
        data = d;
    }
};


// =====================================================================
// [B] NODE: un nodo + su lista de aristas SALIENTES
// =====================================================================
//  Dibujo de UN nodo:
//
//      Node #0  (data = 'A')
//      ┌─────────────────────────────────────────┐
//      │ index = 0                               │
//      │ data  = 'A'                             │
//      │ edges = [ (0→1, 5) ] → [ (0→2, 3) ]     │   <- list<Edge>
//      └─────────────────────────────────────────┘
//
//  Template <N,E>: N = tipo del dato del nodo (char, Coord...),
//                  E = tipo del peso de las aristas.
template<typename N, typename E>
class Node
{
public:
    int           index;   // mi posición en el vector del grafo (mi "id")
    N             data;    // lo que guarda el nodo ('A', Coord{3,4}, ...)
    list<Edge<E>> edges;   // aristas que SALEN de mí

    Node(int i, N d)
    {
        index = i;
        data  = d;
    }

    // Agrega una arista  index ──(d)──► to
    //   Antes: edges = [ ... ]
    //   Después: edges = [ ... ] → [ (index→to, d) ]   (se agrega al final)
    void addEdge(int to, E d)
    {
        edges.push_back(Edge<E>(index, to, d));
    }

    // Borra la PRIMERA arista que vaya hacia 'to'
    // Recorre la lista con un iterador 'it' (como un dedo que avanza):
    //
    //      edges:  [→1] → [→2] → [→3]
    //               it
    //   si it->to == to  => erase(it) y break (ya terminé)
    void removeEdge(int to)
    {
        for (auto it = edges.begin(); it != edges.end(); ++it)
        {
            if (it->to == to) { edges.erase(it); break; }
        }
    }

    // ¿Existe una arista  index ──► to ?   (busca en MI lista)
    bool hasEdge(int to)
    {
        for (auto& e : edges)
            if (e.to == to) return true;
        return false;
    }
};


// =====================================================================
// [C] GRAPH: el grafo completo
// =====================================================================
//  Atributos:
//    numVertices : cuántos nodos hay (== nodes.size())
//    directed    : false (por defecto) => NO dirigido: la arista va en ambos sentidos
//                  true               => dirigido:    solo u ──► v
//    nodes       : vector de nodos. nodes[u] es el nodo con id u.
//
//  Ejemplo con 4 nodos (el de 02_main_char_int.cpp):
//
//     nodes[0]  A ─► [→1 (5)] [→2 (3)]
//     nodes[1]  B ─► [→0 (5)] [→3 (7)]
//     nodes[2]  C ─► [→0 (3)] [→3 (2)]
//     nodes[3]  D ─► [→1 (7)] [→2 (2)]
//
//  Visto como dibujo (no dirigido):
//
//            5
//        A ──────── B
//        │          │
//      3 │          │ 7
//        │          │
//        C ──────── D
//            2
template<typename N, typename E>
class Graph
{
protected:                        // 'protected': visible para clases hijas (graph_finds)
    int numVertices;
    bool directed;
    vector<Node<N,E>> nodes;

public:
    Graph(bool dir = false);

    int         addNode(N data);              // [D1]
    void        addEdge(int u, int v, E data);// [D2]
    bool        hasEdge(int u, int v);        // [D3]
    void        removeEdge(int u, int v);     // [D4]
    Node<N,E>&  getNode(int u);               // [D5]
    int         vertexCount();                // [D5]
    void        print();                      // [D6]
    void        removeNode(int u);            // [D7]  <- la más difícil

protected:
    void validateVertex(int u);               // [D8] chequeo de seguridad
};


// =====================================================================
// [D] OPERACIONES (en el orden recomendado de lectura)
// =====================================================================

// Constructor: grafo vacío
template<typename N, typename E>
Graph<N,E>::Graph(bool dir)
{
    numVertices = 0;
    directed    = dir;
}


// ---------------------------------------------------------------------
// [D1] addNode: agrega un nodo AL FINAL del vector y devuelve su id
// ---------------------------------------------------------------------
//   Antes:   nodes = [A][B]            numVertices = 2
//   addNode('C')
//   Después: nodes = [A][B][C]         numVertices = 3   -> devuelve 2
//                           ^ nuevo nodo, su index = 2
template<typename N, typename E>
int Graph<N,E>::addNode(N data)
{
    nodes.push_back(Node<N,E>(numVertices, data));
    numVertices++;
    return numVertices - 1;       // el id del nodo recién creado
}


// ---------------------------------------------------------------------
// [D2] addEdge: crea la arista u ──► v  (y v ──► u si NO es dirigido)
// ---------------------------------------------------------------------
//   addEdge(0, 1, 5)  en grafo NO dirigido:
//
//      nodes[0].edges += (0→1, 5)
//      nodes[1].edges += (1→0, 5)     <- la "vuelta" se agrega sola
//
//   Por eso en el dibujo A──B hay DOS aristas guardadas.
template<typename N, typename E>
void Graph<N,E>::addEdge(int u, int v, E data)
{
    validateVertex(u);  validateVertex(v);
    nodes[u].addEdge(v, data);
    if (!directed)
        nodes[v].addEdge(u, data);
}


// ---------------------------------------------------------------------
// [D3] hasEdge: ¿existe u ──► v ?   (solo mira la lista de u)
// ---------------------------------------------------------------------
template<typename N, typename E>
bool Graph<N,E>::hasEdge(int u, int v)
{
    validateVertex(u);  validateVertex(v);
    return nodes[u].hasEdge(v);
}


// ---------------------------------------------------------------------
// [D4] removeEdge: borra u ──► v (y v ──► u si NO es dirigido)
// ---------------------------------------------------------------------
//   Es el espejo de addEdge.
template<typename N, typename E>
void Graph<N,E>::removeEdge(int u, int v)
{
    validateVertex(u);  validateVertex(v);
    nodes[u].removeEdge(v);
    if (!directed)
        nodes[v].removeEdge(u);
}


// ---------------------------------------------------------------------
// [D5] getNode / vertexCount: simples "getters"
// ---------------------------------------------------------------------
template<typename N, typename E>
Node<N,E>& Graph<N,E>::getNode(int u)
{
    validateVertex(u);
    return nodes[u];              // devuelve REFERENCIA (&): no copia, es el nodo real
}

template<typename N, typename E>
int Graph<N,E>::vertexCount()
{
    return numVertices;
}


// ---------------------------------------------------------------------
// [D6] print: imprime la lista de adyacencia
// ---------------------------------------------------------------------
//   Formato de cada línea:   DATO_NODO -> VECINO(peso) VECINO(peso) ...
//
//   OJO: imprime 'nodes[e.to].data' (el DATO del vecino), no el índice.
//        Por eso el tipo N necesita operator<< (ver Coord en el 03).
template<typename N, typename E>
void Graph<N,E>::print()
{
    for (int u = 0; u < numVertices; u++)
    {
        cout << nodes[u].data << " ->";
        for (auto& e : nodes[u].edges)
            cout << " " << nodes[e.to].data << "(" << e.data << ")";
        cout << "\n";
    }
}


// ---------------------------------------------------------------------
// [D7] removeNode: LA MÁS DIFÍCIL. Entiéndela con este ejemplo.
// ---------------------------------------------------------------------
//  PROBLEMA: el id de un nodo es su POSICIÓN en el vector. Si borro uno
//  del medio, todos los de la derecha se corren una posición y TODOS los
//  números que los referencian quedan desfasados.
//
//  EJEMPLO: borrar B (u = 1) de  [A=0][B=1][C=2][D=3]
//
//   ANTES:                         aristas guardadas (to)
//     nodes[0] A : →1  →2
//     nodes[1] B : →0  →3          <- este es el que se va
//     nodes[2] C : →0  →3
//     nodes[3] D : →1  →2
//
//   PASO 1 (el for de arriba): en TODOS los demás nodos...
//       - borrar las aristas que apuntan a u      (→1 desaparece)
//       - las que apuntan a un id MAYOR que u     (to > u)  hacen to--
//         porque ese nodo se va a correr una posición a la izquierda.
//
//     nodes[0] A : →2 pasa a →1                   (→1 borrada)
//     nodes[2] C : →0 igual,  →3 pasa a →2
//     nodes[3] D : →1 borrada, →2 pasa a →1
//
//   PASO 2: nodes.erase(...)  quita B del vector y numVertices--
//
//     [A][C][D]    posiciones 0,1,2
//
//   PASO 3 (el último for): actualizar el campo 'index' de los que se
//          movieron (desde u hasta el final):  C.index=1, D.index=2
//
//   DESPUÉS:
//     nodes[0] A : →1            (A──C)
//     nodes[1] C : →0  →2        (C──A, C──D)
//     nodes[2] D : →1            (D──C)
//
//   ⚠ DETALLE: el campo Edge.from de las aristas de C y D NO se
//     actualiza (queda con el índice viejo). Solo se corrigen
//     Node.index y Edge.to. Es una limitación del código; si te
//     preguntan "¿qué falta?", esa es una respuesta.
template<typename N, typename E>
void Graph<N,E>::removeNode(int u)
{
    validateVertex(u);

    // PASO 1: limpiar/ajustar las aristas de todos los demás nodos
    for (int i = 0; i < numVertices; i++)
    {
        if (i == u) continue;                         // el nodo u se borrará completo
        for (auto it = nodes[i].edges.begin(); it != nodes[i].edges.end(); )
        {
            if (it->to == u)
                it = nodes[i].edges.erase(it);        // erase devuelve el SIGUIENTE
            else
            {
                if (it->to > u)
                    it->to--;                         // ese nodo se corre 1 a la izquierda
                ++it;                                 // (solo avanzo si NO borré)
            }
        }
    }

    // PASO 2: sacar el nodo del vector
    nodes.erase(nodes.begin() + u);
    numVertices--;

    // PASO 3: reasignar 'index' a los nodos que se movieron
    for (int i = u; i < numVertices; i++)
        nodes[i].index = i;
}


// ---------------------------------------------------------------------
// [D8] validateVertex: red de seguridad
// ---------------------------------------------------------------------
//   assert detiene el programa si u no es un id válido (0 .. n-1).
template<typename N, typename E>
void Graph<N,E>::validateVertex(int u)
{
    assert(u >= 0 && u < numVertices);
}
