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
  printf("* Test for worst fit allocation\n");
  printf("* The number of free regions should increments each time of Mem_Dump(), with size decreasing 8 bytes each time\n");
  long regionsize = 500;
  long alloc_size = UNITSIZE * N;
  if(Mem_Init(regionsize) == FAIL) {
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  }
  void* store[10];
  for(int i = 0; i < N; i++) {
    printf("*** allocating and free without coalesce %d region with size input %ld ***\n", i, alloc_size);
    store[i] = Mem_Alloc(alloc_size);
    Mem_Free(store[i], FALSE);
    Mem_Dump();
    alloc_size -= UNITSIZE;
    printf("\n");
  }
}
