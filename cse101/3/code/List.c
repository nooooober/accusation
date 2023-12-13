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
#include <stdio.h>
#include <stdlib.h>

ListNode *newListNode(int x) {
  ListNode *node= (ListNode*)calloc(1, sizeof(ListNode));
  node->data = x;
  return node;
}


List newList(void) {
  List list = calloc(1, sizeof(ListObj));
  list->idx = -1;
  return list;
}

void freeList(List* pL) {
  if(*pL == NULL) {
    fprintf(stderr, "|> %s - %d: free list is null\n", __FILE__, __LINE__);
    exit(1);
  }
  while((*pL)->head != NULL) {
    ListNode *needDelete = (*pL)->head;
    (*pL)->head = (*pL)->head->next;
    free(needDelete);
  }
  free(*pL);
  *pL = NULL;
}
 
int length(List L) {
  return L->cnt;
}

int index(List L) {
  return L->idx;
}

int front(List L) {
  if(L->cnt > 0) {
    return L->head->data;
  }
  fprintf(stderr, "|> %s - %d: list length <= 0\n", __FILE__, __LINE__);
  exit(1);
}

int back(List L) {
  if(L->cnt > 0) {
    return  L->tail->data;
  }
  fprintf(stderr, "|> %s - %d: list length <= 0\n", __FILE__, __LINE__);
  exit(1);
}

int get(List L) {
  if(L->idx < 0 || L->idx >= L->cnt) {
    fprintf(stderr, "|> %s - %d: list cursor error\n", __FILE__, __LINE__);
    exit(1);
  }
  int k = L->idx;
  ListNode *cur = L->head;
  while(k--) {
    cur = cur->next;
  }
  return cur->data;
}

bool equals(List A, List B) {
  if(A->cnt != B->cnt) {
    return 0;
  }
  ListNode *cur1 = A->head;
  ListNode *cur2 = B->head;
  while(cur1 != NULL) {
    if(cur1->data != cur2->data) {
      return 0;
    }
    cur1 = cur1->next;
    cur2 = cur2->next;
  }
  return 1;
}

void clear(List L) {
  L->idx = -1;
  L->cnt = 0;
  while(L->head) {
    ListNode *needDelete = L->head;
    L->head = L->head->next;
    free(needDelete);
  }
  L->head = NULL;
  L->tail = NULL;
}

void set(List L, int x) {
  if(L->idx < 0 || L->idx >= L->cnt) {
    fprintf(stderr, "|> %s - %d: list cursor error\n", __FILE__, __LINE__);
    exit(1);
  }
  int k = L->idx;
  ListNode *cur = L->head;
  while(k--) {
    cur = cur->next;
  }
  cur->data = x;
}
 
void moveFront(List L) {
  if(L->cnt > 0) L->idx = 0;
}
 
void moveBack(List L) {
  if(L->cnt > 0) L->idx = L->cnt - 1;
}
 
void movePrev(List L) {
  if(L->idx >= 0 && L->idx <= L->cnt) {
    L->idx -= 1;
  } 
}
 
void moveNext(List L) {
  if(L->idx >=0 && L->idx < L->cnt - 1 ) {
    L->idx += 1;
  } else {
    L->idx = -1;
  }
}

void prepend(List L, int x) {
  if(L->head == NULL) {
    L->head = newListNode(x);
    L->tail = L->head;
  } else {
    ListNode *node = newListNode(x);
    node->next = L->head;
    L->head->pre = node;
    L->head = node;
  }
  L->cnt += 1;
  moveNext(L);
}
 
void append(List L, int x) {
  if(L->head == NULL) {
    L->head = newListNode(x);
    L->tail = L->head;
  } else {
    ListNode *node = newListNode(x);
    node->pre = L->tail;
    L->tail->next = node;
    L->tail = node;
  }
  L->cnt += 1;
}
 
void insertBefore(List L, int x) {
  if(L->idx >= 0 && L->idx < L->cnt) {
    int idx = L->idx;
    if(idx == 0) {
      prepend(L, x);
    } else {
      ListNode *cur = L->head;
      while(idx--) {
        cur = cur->next;
      }
      // printf("insert before: %d   %d\n", L->idx, cur->data);
      ListNode *node = newListNode(x);
      node->next = cur;
      node->pre = cur->pre;
      cur->pre->next = node;
      cur->pre = node;
      L->cnt += 1;
      moveNext(L);
    }
  } else {
    fprintf(stderr, "|> %s - %d: list index error!\n", __FILE__, __LINE__);
    exit(1);
  }
}
 
void insertAfter(List L, int x) {
  if(L->idx >= 0 && L->idx < L->cnt) {
    int idx = L->idx;
    if(idx == L->cnt - 1) {
      append(L, x);
    } else {
      ListNode *cur = L->head;
      while(idx--) {
        cur = cur->next;
      }
      ListNode *node = newListNode(x);
      node->next = cur->next;
      node->pre = cur;
      cur->next->pre = node;
      cur->next = node;
      L->cnt += 1;
    }
  }
}
 
void deleteFront(List L) {
  if(L->cnt > 0) {
    ListNode *needDelete = L->head;
    L->head = L->head->next;
    if(L->tail == needDelete) {
      L->tail = NULL;
    } else {
      L->head->pre = NULL;
    }
    free(needDelete);
    L->cnt -= 1;
    movePrev(L);
  } else {
    fprintf(stderr, "|> %s - %d: list is empty!\n", __FILE__, __LINE__);
    exit(1);
  }
}

void deleteBack(List L) {
  if(L->cnt > 0) {
    ListNode *needDelete = L->tail;
    L->tail = L->tail->pre;
    if(L->head == needDelete) {
      L->head = NULL;
    } else {
      L->tail->next = NULL;
    }
    free(needDelete);
    L->cnt -= 1;
    if(L->cnt == L->idx) {
      L->idx = -1;
    }
  } else {
    fprintf(stderr, "|> %s - %d: list is empty!\n", __FILE__, __LINE__);
    exit(1);
  }
}

void delete(List L) {
  int idx = L->idx;
  ListNode *cur = L->head;
  while(idx--) {
    cur = cur->next;
  }
  if(cur == L->head) {
    deleteFront(L);
  } else if(cur == L->tail) {
    deleteBack(L);
  } else {
    cur->pre->next = cur->next;
    cur->next->pre = cur->pre;
    free(cur);
    L->cnt -= 1;
  }
  L->idx = -1;
}
 

void printList(FILE* out, List L) {
  if(L == NULL) {
    return;
  }
  if(out == NULL) {
    fprintf(stderr, "|> %s - %d: out file is null\n", __FILE__, __LINE__);
    exit(1);
  }
  ListNode *cur = L->head;
  while(cur != NULL) {
    fprintf(out, " %d", cur->data);
    cur = cur->next;
  }
  fprintf(out, "\n");
}
 

List copyList(List L) {
  List nl = newList();
  ListNode *cur = L->head;
  while(cur != NULL) {
    append(nl, cur->data);
    cur = cur->next;
  } 
  return nl;
}
 

List concatList(List A, List B) {
  List l = newList();
  ListNode *cur = A->head;
  while(cur != NULL) {
    append(l, cur->data);
    cur = cur->next;
  }
  cur = B->head;
  while(cur != NULL) {
    append(l, cur->data);
    cur = cur->next;
  }
  return l;
}