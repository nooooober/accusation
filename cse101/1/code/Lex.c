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
#include <string.h>
#include <unistd.h>

extern char* strdup(const char*);

int main(int argc, char **argv) {
  if(argc != 3) {
    fprintf(stderr, "correct format is: Lex <input file> <output file>\n");
    exit(1);
  }
  FILE *in = fopen(argv[1], "r");
  if(in == NULL) {
    fprintf(stderr, "faile to open file %s\n", argv[1]);
    exit(1);
  }
  int line = 0; 
  char buff[1024] = {0};
  while(!feof(in)) {
    fgets(buff, 1024, in);
    if(!strlen(buff)) break;
    if(buff[strlen(buff) - 1] == '\n') buff[strlen(buff) - 1] = 0;
    // printf("read: %s\n", buff);
    line+=1;
    memset(buff, 0, sizeof(buff));
  }
  // printf("line: %d\n", line);

  char **words = (char**)calloc(1, sizeof(char*) * line);
  for(int i = 0; i < line; i++) {
    words[i] = NULL;
  }

  fseek(in, 0, SEEK_SET);

  int idx = 0;
  while(!feof(in)) {
    fgets(buff, 1024, in);
    if(!strlen(buff)) break;
    if(buff[strlen(buff) - 1] == '\n') buff[strlen(buff) - 1] = 0;
    words[idx++] = strdup(buff);
    memset(buff, 0, sizeof(buff));
  }
  
  List list = newList();
  for(int i = 0; i < line; i++) {
    moveFront(list);
    if(index(list) < 0) {
      append(list, i);
      continue;
    }
    while(index(list) >= 0) {
      int idx = get(list);
      if(strcmp(words[idx], words[i]) > 0) break;
      moveNext(list);
    }
    if(index(list) < 0) {
      append(list, i);
    } else {
      insertBefore(list, i);
    }
  }

  FILE *out = fopen(argv[2], "w");
  if(out == NULL) {
    fprintf(stderr, "failed to open file %s\n", argv[2]);
    exit(1);
  }

  moveFront(list);
  while(index(list) >= 0) {
    fprintf(out, "%s\n", words[get(list)]);
    moveNext(list);
  }

  freeList(&list);
  fclose(in);
  fclose(out);
  for(int i = 0; i < line; i++) {
    free(words[i]);
  }
  free(words);

}