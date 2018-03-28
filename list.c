#include <stdlib.h>
#include "list.h"

static void create_header(header* newh, header* prev, header* next, char a_or_f, header* prev_h) {
  newh->canary_start = CSTART;
  newh->canary_end = CEND;
  newh->prev = prev;
  newh->next = next;
  newh->alloc_or_free = a_or_f;
  newh->prev_header = prev_h;
  newh->padding = PADDING;
}

void (*new_header) (header*, header*, header*, char, header*) = &create_header;


