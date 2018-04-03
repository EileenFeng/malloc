#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "mem.h"

#define SUCCESS 0                                                                           
#define FAIL -1
#define TRUE 1
#define FALSE 0
#define UNITSIZE 8
#define N 10

int main(int argc, char** argv) {
  printf("* A simple test for Mem_Free without coalesce\n");
  printf("* Also checked for invalid free pointers: NULL pointers and already-freed regions\n");
  printf("* Mem_Dump should prints out one more free region during each iteration\n");

  printf("* Testing Invalid free: free NULL pointers\n");
  if(Mem_Free(NULL, FALSE) == FAIL) {
    assert(m_error == E_BAD_POINTER);
  } else {
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  }
  long regionsize = 4000;
  if(Mem_Init(regionsize) == FAIL) {
    printf("Mem_Init failed\n");
    exit(EXIT_FAILURE);
  }

  long size = 130;
  void* store[N];
  for(int i = 0; i < N; i++) {
    printf("*** Alloc and Free without coalesce, double free check for region %d ***\n", i);
    store[i] = Mem_Alloc(size);
    if(store[i] == NULL) {
      exit(EXIT_FAILURE);
    }
    if(Mem_Free(store[i], FALSE) == FAIL) {
      exit(EXIT_FAILURE);
    }
    if(Mem_Free(store[i], FALSE) == TRUE) {
      exit(EXIT_FAILURE);
    } else {
      assert(m_error == E_BAD_POINTER);
    }
    Mem_Dump();
    printf("\n");
  }
  printf("* Test Past!\n");
}
