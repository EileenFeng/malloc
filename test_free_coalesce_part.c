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
  printf("* Only some free are coalesce, should end up with 2 free regions, one of them with bigger size, the distance between two free regions should be '2a8' in hex\n");
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


  for(int i = 0; i < N-1; i++) {
    printf("*** Alloc and free for region %d ***\n", i);
    store[i] = Mem_Alloc(size);
    if(store[i] == NULL) {
      exit(EXIT_FAILURE);
    }
    if(i != 4) {
      if(Mem_Free(store[i], FALSE) == FAIL) {
	exit(EXIT_FAILURE);
      }
    }
    Mem_Dump();
    printf("\n");
  }
  printf("*** Alloc and free for region %d ***\n", N);
  store[N-1] = Mem_Alloc(size);
  if(store[N-1] == NULL) {
    exit(EXIT_FAILURE);
  }
  if(Mem_Free(store[N-1], TRUE) == FAIL) {
    exit(EXIT_FAILURE);
  }
  Mem_Dump();
  printf("Test passed\n");
}
