#pragma once
#include <stdlib.h>
#include <sys/types.h>
#include "List.h"

#define INF (int)0x80808080
#define NIL (int)0
#define UNDEF -1

enum Color {
  WHITE,
  GRAY, 
  BLACK
};

typedef struct GraphObj {
  int order;  // n node
  int size;   // n edge
  int source; // start node
  List *adj;
  int *color;
  int *parent;
  int *distance;
  int *discover;     // find time
  int *finish;       // leave time
  u_int32_t timestamp;
}GraphObj;

typedef GraphObj* Graph;

Graph newGraph(int n);
void freeGraph(Graph* pG);
/*** Access functions ***/
int getOrder(Graph G);
int getSize(Graph G);
int getSource(Graph G);
int getParent(Graph G, int u);
int getDist(Graph G, int u);
void getPath(List L, Graph G, int u);
/*** Manipulation procedures ***/
void makeNull(Graph G);
void addEdge(Graph G, int u, int v);
void addArc(Graph G, int u, int v);
void BFS(Graph G, int s);
/*** Other operations ***/
void printGraph(FILE* out, Graph G);


// pa3
int getDiscover(Graph G, int u);
int getFinish(Graph G, int u); 
void visit(Graph g, int u, int *time_stamp);
void DFS(Graph G, List S); /* Pre: length(S)==getOrder(G) */
Graph transpose(Graph G);
Graph copyGraph(Graph G);
List getSccStack(Graph g);
void printGraph(FILE* out , Graph G);
void printGraphSCC(FILE *out, Graph G);

void logG(Graph g);