#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include"List.h"


int main(int argc, char* argv[]){
  List l = newList();
  for(int i = 0; i < 100 ; i++) {
    int *x = calloc(1, sizeof(int));
    (*x) = i;
    append(l, x);
  }   
  moveFront(l);
  while(index(l) >=0) {
    int x = *(int*)get(l);
    printf("%d ", x);
    free((int*)get(l));
    moveNext(l);
  }
  
  freeList(&l);

  return(0);
}