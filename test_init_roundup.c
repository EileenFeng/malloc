#include <stdlib.h>
#include <stdio.h>
#include "mem.h"
#include <assert.h>

#define SUCCESS 0
#define FAIL -1

int main(int argc, char** argv){
  printf("* Test for bad initialization arguments and page round up\n");
  printf("* Initialization should fail for the first two times and succeed for the third time\n");
  printf("* Initial address should end with digit '0' or '8'\n");
  long badsize1 = 0;
  long badsize2 = -.02;
  long regionsize = 10;
  if(Mem_Init(badsize1) == FAIL) {
    assert(m_error == E_BAD_ARGS);
  } else {
    printf("First time success. Test failed\n");
    exit(EXIT_FAILURE);
  }
  if(Mem_Init(badsize2) == FAIL) {
    assert(m_error == E_BAD_ARGS);
  } else {
    printf("Second time success. Test failed\n");
    exit(EXIT_FAILURE);
  }
  
  if(Mem_Init(regionsize) == FAIL) {
    printf("Third time failed. Test failed\n");
    exit(EXIT_FAILURE);
  }
  Mem_Dump();
  printf("Test passed!\n");  
}
