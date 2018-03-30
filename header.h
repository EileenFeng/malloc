#ifndef LIST_H_
#define LIST_H_

#define ALLOC 65
#define FREE 70
#define CSTART 31
#define CEND 30
#define PADDING 80

typedef struct header {

  char canary_start;
  struct header* prev;
  struct header* next;
  struct header* next_free;
  char state;
  char padding1;
  char padding2;
  char canary_end;

}header;

extern void (*new_header)(header* newh, header* prev, header* next, char state, header* next_f);

#endif


