#include <stdlib.h>
#include <stdio.h>
#include "mem.h"

#define SUCCESS 0                                                                           
#define FAIL -1
#define UNITSIZE 8
#define TRUE 1
#define FALSE 0
#define N 10

int main(int argc, char** argv) {
  printf("* Test for simple 8-byte allocation, several aligned allocation and free\n");
  long regionsize = 500;
  if(Mem_Init(regionsize) == FAIL) {
    printf("Init failed\n");
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  }

  void* store[N];
  long size = UNITSIZE;
  for(int i = 0; i < N; i++) {
    store[i] = Mem_Alloc(size);
    if(store[i] == NULL) {
      printf("Test failed\n");
      exit(EXIT_FAILURE);
    }
    size += UNITSIZE;
  }
  for(int i = 0; i < N; i++) {
    printf("********** Free Region %d *************\n", i);
    Mem_Free(store[i], FALSE);
    Mem_Dump();
  }
  printf("\n");
  printf("Test passed!\n");
}
