#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
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
  printf("* Test for writing memory allocated\n");
  printf("* Should print out 'Hello World' \n");
  
  long regionsize = getpagesize();
  if(Mem_Init(regionsize) == FAIL) {
    printf("Init failed\n");
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  }
  Mem_Dump();
  long size = regionsize - HEADER_SIZE;
  void* region = Mem_Alloc(size);
  printf("region is at %p\n", region);
  if(region == NULL) {
    exit(EXIT_FAILURE);
  }
  char* hello = "Hello World";
  memcpy(region, hello, strlen(hello));
  printf("Now region contains %s\n", (char*)region);
  if(Mem_Free(region, TRUE) == FAIL) {
    exit(EXIT_FAILURE);
  }
  printf("*** After free ***\n");
  Mem_Dump();
  printf("Test past!\n");
}
