#include "Graph.h"
#include "List.h"
// #include "backup/List.h"
// #include "backup/List.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <sys/types.h>

Graph newGraph(int n) {
  if(n < 0) {
    fprintf(stderr, "node size must >= 0\n");
    exit(1);
  }
  Graph graph = (Graph)calloc(1, sizeof(GraphObj));
  graph->order = n;
  n += 1;
  graph->color = (int*)calloc(1, sizeof(int) * n);
  graph->parent = (int*)calloc(1, sizeof(int) * n);
  graph->distance = (int*)calloc(1, sizeof(int) * n);
  graph->discover = (int*)calloc(1, sizeof(int) * n);
  graph->finish = (int*)calloc(1, sizeof(int) * n);
  memset(graph->distance, 0x80, sizeof(int) * (graph->order + 1));
  for(int i = 1; i < n ; i++) {
    (graph->discover)[i] = UNDEF;
    (graph->finish)[i] = UNDEF;
  }

  graph->adj = (List*)calloc(1, sizeof(List) * n);
  for(int i = 0; i < n; i++) {
    graph->adj[i] = newList();
  }
  return graph;
}

void freeGraph(Graph* pG) {
  for(int i = 0; i <= (*pG)->order ; i++) {
    freeList(&(*pG)->adj[i]);
  }
  free((*pG)->color);
  free((*pG)->parent);
  free((*pG)->discover);
  free((*pG)->finish);
  free((*pG)->distance);
  free((*pG)->adj);
  free(*pG);
  (*pG) = NULL;
}

/*** Access functions ***/
int getOrder(Graph G) {
  return G->order;
}

int getSize(Graph G) {
  return G->size;
}

int getSource(Graph G) {
  return G->source;
}

int getParent(Graph G, int u) {
  if(u > G->order || u < 1) {
    fprintf(stderr, "%s error: u must meet 1 <= u <= getOrder(G) ", __func__);
    exit(1);
  }
  return G->parent[u];
}

int getDist(Graph G, int u) {
  if(u > G->order || u < 1) {
    fprintf(stderr, "%s error: u must meet 1 <= u <= getOrder(G) ", __func__);
    exit(1);
  }
  return G->distance[u];
}

void getPath(List L, Graph G, int u) {
  if(u > G->order || u < 1) {
    fprintf(stderr, "%s error: u must meet 1 <= u <= getOrder(G)\n", __func__);
    exit(1);
  }
  if(getSource(G) == NIL) {
    fprintf(stderr, "%s error: graph source is nil\n", __func__);
    exit(1);
  }
  if(L == NULL) {
    fprintf(stderr, "%s error: path list can not be null\n", __func__);
    exit(1);
  }

  if(G->distance[u] == INF) {
    append(L, NIL);
    return;
  }
  // for(int i = 0; i <= G->order; i++) {
  //   printf("%d: %d  %d->%d\n", i, G->distance[i], G->parent[i], i);
  // }
  // printf("s: %d u: %d dis: %d\n", G->source , u, G->distance[u]);
  append(L, u);
  if(G->source == u) return;
  do {
    prepend(L, G->parent[u]);
    u = G->parent[u];
  } while(u != G->source);
}

/*** Manipulation procedures ***/
void makeNull(Graph G) {
  for(int i = 0; i <= G->order; i++) {
    freeList(&G->adj[i]);
  }
  G->order = 0;
  G->source = 0;
  G->size = 0;
  memset(G->color, 0, sizeof(int) * (G->order + 1));
  memset(G->distance, 0x80, sizeof(int) * (G->order + 1));
  memset(G->parent, 0, sizeof(int) * (G->order + 1));
}

void addHelp(Graph G, int u, int v) {
  List l = G->adj[u];
  moveFront(l);
  while(index(l) >= 0) {
    int x = get(l);
    if(x > v) break;
    moveNext(l);
  }
  if(index(l) >= 0) insertBefore(l, v);
  else append(l, v);
}

void addEdge(Graph G, int u, int v) {
  if(u > G->order || u < 1 || v > G->order || v < 1) {
    fprintf(stderr, "%s error: u or v must meet 1 <= u <= getOrder(G) ", __func__);
    exit(1);
  }
  addHelp(G, u, v);
  addHelp(G, v, u);
  G->size += 1;
}

void addArc(Graph G, int u, int v) {
  if(u > G->order || u < 1 || v > G->order || v < 1) {
    fprintf(stderr, "%s error: u or v must meet 1 <= u <= getOrder(G) ", __func__);
    exit(1);
  }
  moveFront(G->adj[u]);
  while(index(G->adj[u]) >= 0) {
    int x = get(G->adj[u]);
    if(x == v) return;
    moveNext(G->adj[u]);
  }
  addHelp(G, u, v);
  G->size += 1;
}

void BFS(Graph G, int s) {
  G->source = s;
  for(int i = 1; i <= G->order; i++) {
    moveFront(G->adj[i]);
  }
  memset(G->color, 0, sizeof(int) * (G->order + 1));
  memset(G->distance, 0x80, sizeof(int) * (G->order + 1));
  memset(G->parent, NIL, sizeof(int) * (G->order + 1));

  List q = newList();
  append(q, s);
  G->distance[s] = 0;
  moveFront(q);
  while(index(q) >= 0) {
    int cur = get(q);
    while(index(G->adj[cur]) >= 0) {
      int x = get(G->adj[cur]);
      if(G->color[x] == WHITE) { 
        if(G->distance[cur] + 1 < G->distance[x]) {
          G->parent[x] = cur;
          G->distance[x] = G->distance[cur] + 1;
        }
        if(G->distance[x] == INF) {
          G->parent[x] = cur;
          G->distance[x] = G->distance[cur] + 1;
        } 
        append(q, x); 
      }
      moveNext(G->adj[cur]);
    }
    G->color[cur] = BLACK;
    moveNext(q);
  }
  freeList(&q);
}

/*** Other operations ***/
void printGraph(FILE* out, Graph G) {
  for(int i = 1; i <= G->order; i++) {
    fprintf(out, "%d:", i);
    List list = G->adj[i];
    moveFront(list);
    if(index(list) < 0) {
      fprintf(out, " ");
    } else {
      while(index(list) >= 0) {
        fprintf(out, " %d", get(list));
        moveNext(list);
      }
    }
    fprintf(out, "\n");
  }
}

int getDiscover(Graph G, int u) {
  if(u > G->order || u < 1) {
    fprintf(stderr, "%s error: u must meet 1 <= u <= getOrder(G) ", __func__);
    exit(1);
  }
  return G->discover[u];
}

int getFinish(Graph G, int u) {
  if(u > G->order || u < 1) {
    fprintf(stderr, "%s error: u must meet 1 <= u <= getOrder(G) ", __func__);
    exit(1);
  }
  return G->finish[u];
}

void visit(Graph g, int u, int *time_stamp) {
  // printf("visit: %d\n", u);
  g->discover[u] = ++(*time_stamp);
  g->color[u] = GRAY;
  List l = g->adj[u];
  moveFront(l);
  while(index(l) >= 0 ) {
    int v = get(l);
    if(g->color[v] == WHITE) {
      g->parent[v] = u;
      visit(g, v, time_stamp);
    }
    moveNext(l);
  }
  g->color[u] = BLACK;
  g->finish[u] = ++(*time_stamp);
}

 /* Pre: length(S)==getOrder(G) */
void DFS(Graph G, List S) {
  if( G== NULL || S == NULL) {
    fprintf(stderr, "%s error: %s cannot be null\n",__func__, (G==NULL) ? "G" : "S");
    exit(1);
  }
  if(length(S) != getOrder(G)) {
    fprintf(stderr, "%s error: length(S) != getOrder(G}", __func__);
    exit(1);
  }
  moveFront(S);
  while(index(S) >= 0) {
    int x = get(S);
    (G->color)[x] = WHITE;
    (G->discover)[x] = UNDEF;
    (G->parent)[x] = NIL;
    (G->finish)[x] = UNDEF;
    moveNext(S);
  }
  moveFront(S);
  int time_stamp = 0;
  while(index(S) >= 0) {
    int x = get(S);
    if(G->color[x] == WHITE) {
      visit(G, x, &time_stamp);
    }
    moveNext(S);
  }
  clear(S);
  List st = getSccStack(G);
  moveFront(st);
  while(index(st) >= 0) {
    int x = get(st);
    append(S, x);
    moveNext(st);
  }
  freeList(&st);
  // S = getSccStack(G);
}

List getSccStack(Graph g) {
  List st = newList();
  for(int i = 1; i <= getOrder(g); i++) {
    moveFront(st);
    while(index(st) >= 0) {
      int x = get(st);
      if(g->finish[i] > g->finish[x]) break;
      moveNext(st);
    }
    if(index(st) >= 0) {
      insertBefore(st, i);
    } else {
      append(st, i);
    }
  }
  return st;
}

Graph transpose(Graph G) {
  Graph g = newGraph(getOrder(G));
  for(int i = 1;i <= getOrder(G); i++) {
    List l = G->adj[i];
    moveFront(l);
    while(index(l) >= 0) {
      int u = get(l);
      addArc(g, u, i);
      moveNext(l);
    }
  }
  return g;
}

Graph copyGraph(Graph G) {
  Graph g = newGraph(getOrder(G));
  for(int i = 1;i <= getOrder(G); i++) {
    g->adj[i] = copyList(G->adj[i]);
  }

  return g;
}

void printGraphSCC(FILE *out, Graph g) {
  
  List st = getSccStack(g);
  moveFront(st);
  int cnt = 0;
  while(index(st) >= 0) {
    int x = get(st);
    if(g->parent[x] == NIL) cnt++;
    moveNext(st);
  }
  fprintf(out, "G contains %d strongly connected components:\n", cnt);
  moveFront(st);
  int k = -1;
  List *ret = (List *)calloc(1, sizeof(List) * cnt);
  for(int i = 0; i < cnt; i++) {
    ret[i] = newList();
  }
  while(index(st) >= 0) {
    int x = get(st);
    if(g->parent[x] == NIL) {
      ++k;
    }
    append(ret[k], x);
    moveNext(st);
  }
  for(int i = cnt - 1; i >= 0; i--) {
    fprintf(out, "Component %d:", cnt - i);
    moveFront(ret[i]);
    printList(out, ret[i]);
    freeList(&ret[i]);
  } 
  // free(ret);
  freeList(&st);
}

void logG(Graph g) {
  printf("\n");
  for(int i = 1; i <= getOrder(g); i++) {
    printf("vertex: %2d", i);
    printf("   discover: %2d   finish: %2d   parent: %2d   adj:", g->discover[i], g->finish[i], g->parent[i]);
    printList(stdout, g->adj[i]);
  }
    printf("\n");
}