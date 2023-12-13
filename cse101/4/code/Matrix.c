#include "Matrix.h"
#include "List.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


Entry newEntry(int c, double x) {
  Entry e = calloc(1, sizeof(EntryObj));
  e->col = c;
  e->value = x;
  return e;
}

// newMatrix()
// Returns a reference to a new nXn Matrix object in the zero state.
Matrix newMatrix(int n) {
  Matrix mat = (Matrix) calloc(1, sizeof(MatrixObj)); 
  mat->n = n;
  mat->entries = (List*)calloc(1, sizeof(List) * (n + 1));
  for(int i = 1; i <= n ; i++) {
    mat->entries[i] = newList(); 
  }  
  return mat;
}

void freeListM(List* pL) {
  if(*pL == NULL) {
    fprintf(stderr, "|> %s - %d: free list is null\n", __FILE__, __LINE__);
    exit(1);
  }
  while((*pL)->head != NULL) {
    ListNode *needDelete = (*pL)->head;
    (*pL)->head = (*pL)->head->next;
    free((Entry)needDelete->data);
    free(needDelete);
  }
  free(*pL);
  *pL = NULL;
}
// freeMatrix()
// Frees heap memory associated with *pM, sets *pM to NULL.
void freeMatrix(Matrix* pM) {
  if((*pM) == NULL) return;
  for(int i = 1; i <= (*pM)->n ; i++) {
    freeListM(&(*pM)->entries[i]);
  } 
  free((*pM)->entries);
  free((*pM));
  (*pM) = NULL;
}
// Access functions
// size()
// Return the size of square Matrix M.
int size(Matrix M) {
  return M->n;
}
// NNZ()
// Return the number of non-zero elements in M.
int NNZ(Matrix M) {
  int ret = 0;
  for(int i = 1; i <= M->n; i++) ret += length(M->entries[i]);
  return ret;
}
// equals()
// Return true (1) if matrices A and B are equal, false (0) otherwise.
int equals(Matrix A, Matrix B) {
  if(A->n != B->n) return 0;
  for(int i = 1; i <= A->n; i++) {
    if(length(A->entries[i]) != length(B->entries[i])) {
      return 0;
    } else {
      moveFront(A->entries[i]);
      moveFront(B->entries[i]);
      while(index(A->entries[i]) >= 0) {
        Entry ea = (Entry)get(A->entries[i]);
        Entry eb = (Entry)get(B->entries[i]);
        if(ea->col != eb->col || ea->value != eb->value) {
          return 0;
        }
        moveNext(A->entries[i]);
        moveNext(B->entries[i]);
      }
    }
  }
  return 1;
}

void clearM(List L) {
  L->idx = -1;
  L->cnt = 0;
  while(L->head) {
    ListNode *needDelete = L->head;
    L->head = L->head->next;
    free((Entry)needDelete->data);
    free(needDelete);
  }
  L->head = NULL;
  L->tail = NULL;
}
// Manipulation procedures
// makeZero()
// Re-sets M to the zero Matrix state.
void makeZero(Matrix M) {
  for(int i = 1; i <= M->n; i++) {
    clearM(M->entries[i]);
  }
  
}
// changeEntry()
// Changes the ith row, jth column of M to the value x.
// Pre: 1<=i<=size(M), 1<=j<=size(M)
void changeEntry(Matrix M, int i, int j, double x) {
  if(i < 1 || i > size(M) || j < 1 || j > size(M)) {
    abort();
  }
  // if(x == 0.0f) return;
  moveFront(M->entries[i]);
  while(index(M->entries[i]) >= 0) {
    int col = ((Entry)get(M->entries[i]))->col;
    if(col == j) {
      if(x == 0.0f) {
        delete(M->entries[i]);
      } else {
        Entry e = ((Entry)get(M->entries[i]));
        Entry ne = newEntry(j, x);
        set(M->entries[i], ne);
        free(e);
        return;
      }
    }
    if(col > j) break;
    moveNext(M->entries[i]);
  }
  if(x == 0.0f) return;
  if(index(M->entries[i]) < 0) {
    Entry e = newEntry(j, x);
    append(M->entries[i], e);
  } else {
    Entry ne = newEntry(j, x);
    insertBefore(M->entries[i], ne);
  }
}
// Matrix Arithmetic operations
// copy()
// Returns a reference to a new Matrix object having the same entries as A.
Matrix copy(Matrix A) {
  Matrix nx = newMatrix(A->n);
  for(int i = 1; i <= A->n; i++) {
    moveFront(A->entries[i]);
    while(index(A->entries[i]) >= 0) {
      Entry ea = (Entry)get(A->entries[i]);
      Entry e = newEntry(ea->col, ea->value);
      append(nx->entries[i], e);
      moveNext(A->entries[i]);
    }
  }
  return nx;
}
// transpose()
// Returns a reference to a new Matrix object representing the transpose
// of A.
Matrix transpose(Matrix A) {
  Matrix tx = newMatrix(A->n);
  for(int i = 1; i<= A->n; i++) {
    moveFront(A->entries[i]); 
    while(index(A->entries[i])>=0) {
      Entry e = (Entry)get(A->entries[i]);
      Entry nx = newEntry(i, e->value);

      append(tx->entries[e->col], nx);

      moveNext(A->entries[i]);
    }
  }
  return tx;
}
// scalarMult()
// Returns a reference to a new Matrix object representing xA.
Matrix scalarMult(double x, Matrix A) {
  Matrix nx = newMatrix(A->n);
  for(int i = 1; i <= A->n ; i++) {
    moveFront(A->entries[i]);
    while(index(A->entries[i]) >= 0) {
      Entry e = (Entry)get(A->entries[i]);
      Entry ne = newEntry(e->col, x * e->value);
      if(ne->value!=0.0f) append(nx->entries[i], ne);
      moveNext(A->entries[i]);
    }
  }
  return nx;
}
// sum()
// Returns a reference to a new Matrix object representing A+B.
// pre: size(A)==size(B)
Matrix sum(Matrix A, Matrix B) {
  if(A->n != B->n) {
    abort();
  }
  bool needFree = false;
  if(equals(A, B)) {
    B = copy(A);
    needFree = true;
  }
  Matrix nx = newMatrix(A->n);
  for(int i = 1;i <= A->n ;i++) {
    moveFront(A->entries[i]);
    moveFront(B->entries[i]);
    while(index(A->entries[i]) >= 0 && index(B->entries[i]) >= 0) {
      Entry ea = (Entry)get(A->entries[i]);
      Entry eb = (Entry)get(B->entries[i]);
      if(ea->col < eb->col) {
        Entry e = newEntry(ea->col, ea->value);
        append(nx->entries[i], e);
        moveNext(A->entries[i]);
      } else if(ea->col == eb->col) {
        Entry e = newEntry(ea->col, ea->value + eb->value);
        if(e->value != 0.0f) append(nx->entries[i], e);
        else free(e);
        moveNext(A->entries[i]);
        moveNext(B->entries[i]);
      } else {
        Entry e =  newEntry(eb->col, eb->value);
        append(nx->entries[i], e);
        moveNext(B->entries[i]);
      }
    } 
    while(index(A->entries[i]) >= 0) {
      Entry ea = (Entry)get(A->entries[i]);
      Entry e = newEntry(ea->col, ea->value);
      append(nx->entries[i], e);
      moveNext(A->entries[i]);
    }
    while(index(B->entries[i]) >= 0) {
      Entry eb = (Entry)get(B->entries[i]);
      Entry e = newEntry(eb->col, eb->value);
      append(nx->entries[i], e);
      moveNext(B->entries[i]);
    } 
  }
  if(needFree) freeMatrix(&B);
  return nx;
}
// diff()
// Returns a reference to a new Matrix object representing A-B.
// pre: size(A)==size(B)
Matrix diff(Matrix A, Matrix B) {
  if(A->n != B->n) {
    abort();
  }
  bool needFree = false;
  if(equals(A, B)) {
    B = copy(B);
    needFree = true;
  }
  Matrix nx = newMatrix(A->n);

  for(int i = 1; i<= A->n; i++) {
    moveFront(A->entries[i]);
    moveFront(B->entries[i]);
    while(index(A->entries[i]) >= 0 && index(B->entries[i]) >= 0) {
      Entry ea = (Entry)get(A->entries[i]);
      Entry eb = (Entry)get(B->entries[i]);
      if(ea->col < eb->col) {
        Entry e = newEntry(ea->col, ea->value);
        append(nx->entries[i], e);
        moveNext(A->entries[i]);
      } else if(ea->col == eb->col) {
        Entry e = newEntry(ea->col, ea->value - eb->value);
        if(e->value != 0.0f) {
          append(nx->entries[i], e);
        } else {
          free(e);
        }
        moveNext(A->entries[i]);
        moveNext(B->entries[i]);
      } else {
        Entry e = newEntry(eb->col, -eb->value);
        append(nx->entries[i], e);
        moveNext(B->entries[i]);
      }
    }

    while(index(A->entries[i]) >= 0) {
      Entry ea = (Entry)get(A->entries[i]);
      Entry e = newEntry(ea->col, ea->value);
      append(nx->entries[i], e);
      moveNext(A->entries[i]);
    }

    while(index(B->entries[i]) >= 0) {
      Entry eb = (Entry)get(B->entries[i]);
      Entry e = newEntry(eb->col, -eb->value);
      append(nx->entries[i], e);
      moveNext(B->entries[i]);
    }
  }
  if(needFree) freeMatrix(&B);
  return nx;
}

// product()
// Returns a reference to a new Matrix object representing AB
// pre: size(A)==size(B)
// 1.0, 0.0 , 1.0
// 0.0, 1.0, 0.0
// 1.0, 1.0, 1.0
Matrix product(Matrix A, Matrix B) {
  if(A->n != B->n) {
    abort();
  }
  bool needFree = false;
  if(equals(A, B)) {
    B = copy(A);
    needFree = true;
  }
  Matrix nx = newMatrix(A->n);
  for(int i = 1; i <= A->n; i++) {
    moveFront(A->entries[i]);
    while(index(A->entries[i]) >= 0) {
      Entry ea = (Entry)get(A->entries[i]);
      int j = ea->col;
      moveFront(B->entries[j]);
      while(index(B->entries[j]) >= 0) {
        Entry eb = (Entry)get(B->entries[j]);
        bool seted = false;
        moveFront(nx->entries[i]);
        while(index(nx->entries[i]) >= 0) {
          Entry ex = (Entry)get(nx->entries[i]);
          if(ex->col == eb->col) {
            Entry e = newEntry(eb->col, ex->value + ea->value * eb->value);
            set(nx->entries[i], e);
            free(ex);
            seted = true;
            break;
          }
          if(ex->col > eb->col) break;
          moveNext(nx->entries[i]);
        }
        if(!seted ) {
          if(index(nx->entries[i]) < 0) {
            Entry e = newEntry(eb->col, ea->value * eb->value);
            append(nx->entries[i], e);
          } else {
            Entry e = newEntry(eb->col, ea->value * eb->value);
            insertBefore(nx->entries[i], e);
          }
        }

        moveNext(B->entries[j]);
      }

      moveNext(A->entries[i]);
    }
  }
  if(needFree) freeMatrix(&B);
  return nx;
}
// printMatrix()
// Prints a string representation of Matrix M to filestream out. Zero rows
// are not printed. Each non-zero row is represented as one line consisting
// of the row number, followed by a colon, a space, then a space separated
// list of pairs "(col, val)" giving the column numbers and non-zero values
// in that row. The double val will be rounded to 1 decimal point.
void printMatrix(FILE* out, Matrix M) {
  // fprintf(out, "%d\n", nzn);
  for(int i = 1; i <= M->n; i++) {
    if(length(M->entries[i]) > 0) {
      fprintf(out, "%d: ", i);
      moveFront(M->entries[i]);
      while(index(M->entries[i]) >= 0) {
        Entry e = (Entry)get(M->entries[i]);
        fprintf(out, "(%d, %.1f) ", e->col, e->value);
        moveNext(M->entries[i]);
      }
      fprintf(out, "\n");
    }
  }
  fprintf(out, "\n");
  
}