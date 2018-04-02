#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "mem.h"

#define SUCCESS 0                                                                           
#define FAIL -1
#define UNITSIZE 8
#define TRUE 1
#define FALSE 0
#define N 10
#define EXPAND 5
#define HEADER_SIZE 32

int main(int argc, char** argv) {
  printf("Test for no memory left to assign\n");
  long regionsize = getpagesize();
  if(Mem_Init(regionsize) == FAIL) {
    printf("Init failed\n");
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  }
  printf("*** Before any allocations *** \n");
  Mem_Dump();
  long size = (long)(regionsize * EXPAND / 2 - HEADER_SIZE);
  void* region_one = Mem_Alloc(size);
  if(region_one == NULL) {
    exit(EXIT_FAILURE);
  }
  void* region_two = Mem_Alloc(size);
  if(region_two == NULL) {
    exit(EXIT_FAILURE);
  }
  printf("*** Mem_Dump now should not have any output ***\n");
  Mem_Dump();
  void* invalid_region = Mem_Alloc(1);
  if(invalid_region == NULL) {
    assert(m_error == E_NO_SPACE);
  } else {
    exit(EXIT_FAILURE);
  }
  if(Mem_Free(region_one, TRUE) == FAIL) {
    exit(EXIT_FAILURE);
  }
  printf("*** After freeing and coalesce some memory ***\n");
  Mem_Dump();
  region_one = Mem_Alloc(UNITSIZE);
  if(region_one == NULL) {
    exit(EXIT_FAILURE);
  }
  if(Mem_Free(region_one, FALSE) == FAIL) {
    exit(EXIT_FAILURE);
  }
  printf("*** Mem_Dump should give two free memory regions ***\n");
  Mem_Dump();
  printf("Test passed\n");
}
