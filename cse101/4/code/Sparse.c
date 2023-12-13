#include "Matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include "Matrix.h"

int main(int argc, char** argv) {
  if(argc != 3) {
    fprintf(stderr, "usage: ./Sparse infile outfile\n");
    exit(1);
  }
  FILE *in = fopen(argv[1], "r");
  FILE *out = fopen(argv[2], "w");
  if(in == NULL || out == NULL ) abort();
  int n, a, b;
  int x, y;
  double v;
  fscanf(in, "%d%d%d", &n, &a, &b);
  Matrix A = newMatrix(n);
  Matrix B = newMatrix(n);
  for(int i = 0; i < a ; i++) {
    fscanf(in,"%d%d%lf", &x, &y, &v);
    changeEntry(A, x, y, v);
  }
  for(int i = 0; i < b; i++) {
    fscanf(in,"%d%d%lf", &x, &y, &v);
    changeEntry(B, x, y, v);    
  }
  fprintf(out, "A has %d non-zero entries:\n", NNZ(A));
  printMatrix(out, A);
  fprintf(out, "B has %d non-zero entries:\n", NNZ(B));
  printMatrix(out, B);

  Matrix C = scalarMult(1.5, A);
  fprintf(out, "(1.5)*A =\n");
  printMatrix(out, C);

  Matrix D = sum(A, B);
  fprintf(out, "A+B =\n");
  printMatrix(out, D);

  Matrix E = sum(A, A);
  fprintf(out, "A+A =\n");
  printMatrix(out, E);

  Matrix F = diff(B, A);
  fprintf(out, "B-A =\n");
  printMatrix(out, F);

  Matrix H = diff(A, A);
  fprintf(out, "A-A =\n");
  printMatrix(out, H);

  Matrix I =  transpose(A);
  fprintf(out, "Transpose(A) =\n");
  printMatrix(out, I);

  Matrix J = product(A, B);
  fprintf(out, "A*B =\n");
  printMatrix(out, J);

  Matrix K = product(B, B);
  fprintf(out, "B*B =\n");
  printMatrix(out, K);
  // return 1;

  freeMatrix(&A);
  freeMatrix(&B);
  freeMatrix(&C);
  freeMatrix(&D);
  freeMatrix(&E);
  freeMatrix(&F);
  freeMatrix(&H);
  freeMatrix(&I);
  freeMatrix(&J);
  freeMatrix(&K);


  fclose(in);
  fclose(out);
}