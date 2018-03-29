#include <stdlib.h>
#include <stdio.h>
#include "mem.h"

#define SUCCESS 0                                                                           
#define FAIL -1

int main(int argc, char** argv) {
  long regionsize = 100;
  if(Mem_Init(regionsize) == FAIL) {
    printf("Init failed\n");
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  }

  long size_one = 1;
  long size_two = 80;
  long size_three = 3;
  void* region_one = Mem_Alloc(size_one);
  void* region_two = Mem_Alloc(size_two);
  void* region_three = Mem_Alloc(size_three);
  if(region_one == NULL) {
    printf("Alloc region_one failed. Test failed\n");
    exit(EXIT_FAILURE);
  }
  if(region_two == NULL) {
    printf("Alloc region_two failed. Test failed\n");
    exit(EXIT_FAILURE);
  } 
  if(region_three == NULL) {
    printf("Alloc region_three failed. Test failed\n");
    exit(EXIT_FAILURE);
  } 
  printf("The memory region one starts at %p\n", region_one);
  printf("The memory region two starts at %p\n", region_two); 
  printf("The memory region three starts at %p\n", region_three);
  Mem_Dump();
  printf("Test passed!\n");
}
