#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "mem.h"

#define SUCCESS 0                                                                           
#define FAIL -1
#define TRUE 1
#define FALSE 0
#define N 10

int main(int argc, char** argv) {
  printf("* A simple test for Mem_Free with coalesce\n");
  printf("* Mem_Dump should ultimately only display one free region with size equal to initializing size displayed\n");
  printf("* Also checked for invalid free pointers\n");
  printf("* Check memory dump for results\n");

  printf("* Testing Invalid free: free NULL pointers\n");
  if(Mem_Free(NULL, FALSE) != FAIL) {
    printf("Freed null pointer\n");
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  } else {
    assert(m_error == E_BAD_POINTER);
  }
  long regionsize = 5000;
  void* store[N];
  long size = 97;
  if(Mem_Init(regionsize) == FAIL) {
    printf("Init failed\n");
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  }

  printf("*** Initial free memory region before any allocations ***\n");
  Mem_Dump();
  printf("\n");
  for(int i = 0; i < N; i++) {
    printf("*** Alloc and free for region %d ***\n", i);
    store[i] = Mem_Alloc(size);
    if(store[i] == NULL) {
      exit(EXIT_FAILURE);
    }
    if(Mem_Free(store[i], FALSE) == FAIL) {
      exit(EXIT_FAILURE);
    }
    Mem_Dump();
    printf("\n");
  }
  printf("*** Now coalesce the whole list. Mem_Dump should only output one free memory region with size equal to the size of memory region before any allocations ***\n");
  if(Mem_Free(NULL, TRUE) == FAIL) {
    exit(EXIT_FAILURE);
  }
  Mem_Dump();
  printf("\n");
  printf("Test passed\n");
}
