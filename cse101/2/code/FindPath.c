#include "Graph.h"
#include "List.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv) {
  if(argc != 3) {
    fprintf(stderr, "correct format should be: FindPath input_file output_file\n");
    exit(1);
  }
  FILE *in = fopen(argv[1], "r");
  if(in == NULL) {
    fprintf(stderr, "failed to open file %s\n", argv[1]);
    exit(1);
  }
  FILE *out = fopen(argv[2], "w");
  if(out == NULL) {
    fprintf(stderr, "failed to write file %s\n", argv[2]);
    exit(2);
  }
  int n;
  fscanf(in, "%d", &n);
  Graph g = newGraph(n);
  int u, v;
  int swo = 0;
  while(!feof(in)) {
    fscanf(in, "%d%d", &u, &v);
    if(u == 0 && v == 0) {
      swo++;
      if(swo == 1) {
        printGraph(out, g);
        continue;
      }
      if(swo == 2) break;
    }
    if(swo == 0) {
      addEdge(g, u, v);
    } else if(swo == 1) {
      fprintf(out, "\n");
      List path = newList();
      BFS(g, u);
      getPath(path, g, v);
      int dis = getDist(g, v);
      if(dis == INF) {
        fprintf(out, "The distance from %d to %d is infinity\n", u, v);
        fprintf(out, "No %d-%d path exists\n", u, v);
      } else {
        fprintf(out, "The distance from %d to %d is %d\n", u, v, dis);
        fprintf(out, "A shortest %d-%d path is:", u, v);
        moveFront(path);
        while(index(path) >=0) {
          fprintf(out, " %d", get(path));
          moveNext(path);
        }
        fprintf(out, "\n");
      }
     
      freeList(&path);
    } 
  }
  fclose(in);
  fclose(out);
  freeGraph(&g);
}