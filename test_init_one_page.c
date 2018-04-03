#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include "mem.h"

#define SUCCESS 0
#define FAIL -1

int main(int argc, char** argv){
  printf("* Test for bad initialization arguments and one page initialization\n");
  printf("* Based on my implementation, the actual size initialized will be 5 times the original input in consideration of worst case scenarios\n");
  printf("* Initialization should fail for the first two times and succeed for the third time\n");
  printf("* Initial address should end with digit '0' or '8'\n");
  
  long badsize1 = 0;
  long badsize2 = -.02;
  long regionsize = getpagesize();
  if(Mem_Init(badsize1) == FAIL) {
    //printf("First time initializing failed\n");
    assert(m_error == E_BAD_ARGS);
  } else {
    printf("First time success. Test failed\n");
    exit(EXIT_FAILURE);
  }
  if(Mem_Init(badsize2) == FAIL) {
    //    printf("Second time initializing failed\n");
    assert(m_error == E_BAD_ARGS);
  } else {
    printf("Second time success. Test failed\n");
    exit(EXIT_FAILURE);
  }
  
  if(Mem_Init(regionsize) == FAIL) {
    printf("Third time failed. Test failed\n");
    exit(EXIT_FAILURE);
  }
  printf("Page size is %d\n", getpagesize());
  Mem_Dump();
  printf("Test passed!\n");  
}
