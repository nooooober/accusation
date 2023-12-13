/**
 * @file Lex.c
 * @author your name (you@domain.com)
 * @CruzID xx
 * @assignment pa1
 * @brief 
 * @version 0.1
 * @date 2023-01-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "List.h"
#include <stdlib.h>

int main() {
  List list = newList(); // 5 1 3 2
  append(list, 1);
  append(list, 3);
  append(list, 2);
  prepend(list, 5);
  printList(stdout, list);

  moveFront(list);

  while (index(list) >= 0) {
    printf("%d\n", get(list));
    moveNext(list);
  }

  moveBack(list);
  insertBefore(list, 6);
  insertAfter(list, 7);

  moveBack(list);
  while(index(list) >= 0) {
    printf("- %d\n", get(list));
    movePrev(list);
  }

  deleteFront(list);
  deleteBack(list);

  moveBack(list);
  set(list, 33);

  moveFront(list);
  while (index(list) >= 0) {
    printf("%d\n", get(list));
    moveNext(list);
  }

  moveFront(list);
  moveNext(list);
  delete(list);
  printList(stdout, list);

  List cl = copyList(list);
  printList(stdout, cl);

  List ct = concatList(list, cl);
  printList(stdout, ct);

  clear(list);
  printList(stdout, list);

  freeList(&list);
  freeList(&cl);
  freeList(&ct);
}