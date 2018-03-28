#ifndef LIST_H_
#define LIST_H_
#include <stdlib.h>
#define ALLOC 65
#define FREE 70
#define CSTART 202 // canary values
#define CEND 203  // canary value
#define PADDING 80

typedef struct header {

  char canary_start;
  struct header* prev; //points to previously allocated
  struct header* next; // points to next allocated
  long size; // size of the memory not including the header (in bytes)
  struct header* prev_header; // points to previous free/allocated memory node (location)
  char alloc_or_free;
  char padding;
  char canary_end;
  
}header;

extern void (*new_header)(header* newh, header* prev, header* next, char a_or_f, header* prev_h);


#endif
