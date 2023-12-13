#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Graph.h"
#include "List.h"


extern char *strtok_r(char *restrict __s, const char *restrict __delim, char **restrict __save_ptr);

int main(int argc, char **argv) {
  if(argc != 3) {
    fprintf(stderr, "usage: %s infile outfile\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  FILE *in = fopen(argv[1], "r");
  FILE *out = fopen(argv[2], "w");
  if(in == NULL || out == NULL) {
    fprintf(stderr, "failed to open or write file %s\n", (in == NULL) ? argv[1] : argv[2]);
    exit(EXIT_FAILURE);
  }
  int n;
  fscanf(in, "%d", &n);
  Graph g = newGraph(n);
  while(!feof(in)) {
    char buff[1024] = {0};
    fgets(buff, 1024, in);
    if(strlen(buff) >= 3) {
      int u, v;
      sscanf(buff, "%d%d", &u, &v);
      if(u == 0 && v == 0) break;
      addArc(g, u, v);
    }
  }
  fprintf(out, "Adjacency list representation of G:\n");
  printGraph(out, g);

  List l = newList();
  for(int i = 1;i <= n;  i++) {
    append(l, i);
  }
  DFS(g, l);
  List st = getSccStack(g);
  Graph rg = transpose(g);
  DFS(rg, st);

  fprintf(out, "\n");
  printGraphSCC(out, rg);
  freeGraph(&g);
  freeGraph(&rg);
  fclose(in);
  fclose(out);
}