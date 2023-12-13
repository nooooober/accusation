#include <stdio.h>
#include "Graph.h"
#include "List.h"

int main() {
  Graph g = newGraph(0);
  freeGraph(&g);  

  g = newGraph(1);
  // getDist(g, 3);
  // getParent(g, 3);
  freeGraph(&g);

  g = newGraph(4);
  addArc(g, 1, 2);
  addArc(g, 2, 1);
  addArc(g, 2, 4);
  printGraph(stdout, g);
  BFS(g, 3);
  List path = newList();
  getPath(path, g, 1);
  printList(stdout, path);
  clear(path);

  BFS(g, 1);
  List list = newList();
  for(int i = 1; i<= 4;i++) append(list, i);
  DFS(g, list);

  printGraphSCC(stdout, g);
  getPath(path, g, 4);
  printList(stdout, path);
  freeList(&path);
  freeGraph(&g);
  freeList(&list);

}