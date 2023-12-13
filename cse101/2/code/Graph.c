#include "Graph.h"
#include "List.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

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
  memset(graph->distance, 0x80, sizeof(int) * (graph->order + 1));
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
  List lu = G->adj[u];
  moveFront(lu);
  if(index(lu) < 0) {
    append(lu, v);
    return;
  }
  moveBack(lu);
  if(v > get(lu)) {
    append(lu, v);
    return;
  }
  moveFront(lu);
  while(index(lu) >= 0) {
    if(get(lu) > v) break;
    moveNext(lu);
  }
  insertBefore(lu, v);
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
