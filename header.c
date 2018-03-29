#include <stdlib.h>
#include "header.h"

static void createheader (header* newh, header* prev, header* next, char state, header* next_f){
  newh->prev = prev;
  newh->next = next;
  newh->state = state;
  newh->next_free = next_f;
  newh->canary_start = CSTART;
  newh->canary_end = CEND;
  newh->padding1 = PADDING;
  newh->padding2 = PADDING;
}

void (*new_header) (header*, header*, header*, char, header*) = &createheader;
