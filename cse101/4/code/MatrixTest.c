#include "Matrix.h"

int main() {
  Matrix mat = newMatrix(3);
  freeMatrix(&mat);

  Matrix A = newMatrix(1000);
  changeEntry(A, 1, 1, 1.0);
  changeEntry(A, 2, 1, 1.0);
  Matrix C = sum(A, A);
  Matrix D = diff(A, C);
  Matrix E = product(D, C);
  Matrix H = copy(E);
  printMatrix(stdout, C);
  printMatrix(stdout, D);
  printMatrix(stdout, E);
  printMatrix(stdout, H);
  makeZero(H);
  printMatrix(stdout, H);


  freeMatrix(&A);
  freeMatrix(&C);
  freeMatrix(&D);
  freeMatrix(&E);
  freeMatrix(&H);

}