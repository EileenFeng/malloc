#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/mman.h>
#include <unistd.h>
#include "header.h"
#include "mem.h"


#define HEADER_SIZE 32
#define TRUE 1
#define FALSE 0
#define SUCCESS 0
#define FAIL -1
#define ALIGNED 8
#define EXPAND 5

int m_error;

static header* free_head = NULL;
static header* head = NULL;
static int init = FALSE;
static int coal_all = FALSE;
void* end_address;

int Mem_Init(long sizeofRegion) {
  if(init == TRUE) {
    m_error = E_BAD_ARGS;
    return FAIL;
  }
  if(sizeofRegion <= 0) {
    m_error = E_BAD_ARGS;
    return FAIL;
  }

  long byte_roundup = (long) round(sizeofRegion * 1.0f / ALIGNED + 0.5f);
  long byte_num = byte_roundup * ALIGNED * EXPAND;
  long region_size = (long) round(byte_num * 1.0f / getpagesize() + 0.5f);
  long size_of_region = region_size * getpagesize();
  if((free_head = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == (void*) -1) {
    m_error = E_BAD_ARGS;
    perror("Failed mmap:");
    return FAIL;
   }
  head = free_head;
  end_address = (void*)((long)((char*)free_head + size_of_region));
  new_header(free_head, NULL, NULL, FREE, NULL);
  init = TRUE;
  printf(" size_of_region %ld in init free list %p and end address %p\n", size_of_region, free_head, (void*)end_address);
  return SUCCESS;   
}

void *Mem_Alloc(long size) {
  if(size <= 0) {
    m_error = E_BAD_ARGS;
    return NULL;
  }

  long unit_num = (long) round(size * 1.0f / ALIGNED + 0.5f);
  long actual_assigned = (long) unit_num * ALIGNED;
  header* before_target = NULL;
  header* target = NULL;

  // get the max node
  header* before_traverse = NULL;
  header* traverse = free_head;
  long maxsize = 0;
  while(traverse != NULL) {
    header* temp = traverse->next;
    long size;
    if(temp != NULL) {
      size = (char*)temp - (char*)traverse - HEADER_SIZE;
    } else {
      size = (long)end_address - (long)(void*)traverse - HEADER_SIZE;
    }
    if(size > maxsize) {
      maxsize = size;
      before_target = before_traverse;
      target = traverse;
    } 
    before_traverse = traverse;
    traverse = traverse->next_free;
  }

  if(target == NULL) {
    m_error = E_NO_SPACE;
    return NULL;
  }
  if(maxsize < actual_assigned) {
    m_error = E_NO_SPACE;
    return NULL;
  }

  printf("Actual assigned %ld\n", actual_assigned);
  header* nexth = target->next;
  header* next_free = target->next_free;
  header* new_free = NULL;
  target->state = ALLOC;
  target->next_free = NULL;
  if(maxsize == actual_assigned) {
    if(before_target != NULL) {
      before_target->next_free = next_free;
    } else {
      free_head = next_free;
    }
  } else {
    new_free = (header*)((char*)target + actual_assigned + HEADER_SIZE);
    new_header(new_free, target, nexth, FREE, next_free);
    target->next = new_free;
    if(before_target != NULL) {
      before_target->next_free = new_free;
    } else {
      free_head = new_free;
    }
    if(nexth != NULL) { 
      nexth->prev = new_free;
    }
  }
  printf("allocated header address %p\n", target);
  return (void*)((char*)target + HEADER_SIZE);
}

int Mem_Free(void* ptr, int coalesce) {
  if(ptr == NULL) {
    m_error = E_BAD_POINTER;
    return FAIL;
  }
  header* target = (header*)((char*) ((header*)ptr) - HEADER_SIZE);
  if(target->state == FREE) {
    m_error = E_BAD_POINTER;
    return FAIL;
  }
  
  target->state = FREE;
  header* prev_free = NULL;
  header* after_free = free_head;
  while(after_free != NULL) {
    if(after_free > target) {
      break;
    } else {
      prev_free = after_free;
      after_free = after_free->next_free;
    }
  }

  if(prev_free == NULL) {
    free_head = target;
  } else {
    prev_free->next_free = target;
  }
  target->next_free = after_free;   
  
  if(coalesce == FALSE) {
    coal_all = TRUE;
  } else {
    if(coal_all == FALSE) {
      header* result = NULL;
      header* nexttarget = target->next;
      if(prev_free == NULL) {
	result = target;
	free_head = target;
      } else {
 	header* nextprev = prev_free->next;
	if((char*)nextprev == (char*) target) {
	  result = prev_free;
	  result->next = target->next;
	  if(target->next != NULL) {
	    target->next->prev = result;
	  }
	  result->next_free = target->next_free;
	  target->next_free = NULL;
	} else {
	  result = target;
	}
      }
      if((char*)nexttarget == (char*) after_free) {
	if(target->next_free != NULL) {
	  target->next_free = NULL;
	}
	result->next = after_free->next;
	if(after_free->next != NULL) {
	  after_free->next->prev = result;
	}
	result->next_free = after_free->next_free;
      }
    } else {
      header* curfreenode = free_head;
      header* cur = curfreenode;
      header* fol = curfreenode->next_free;
      
      while(fol != NULL) {
	if(cur->next == fol) {
	  curfreenode->next_free = fol->next_free;
	  curfreenode->next = fol->next;
	  if(fol->next != NULL) {
	    fol->next->prev = curfreenode;
	  }
	  cur = fol;
	  fol = fol->next_free;
	  cur->next_free = NULL;
	} else {
	  curfreenode->next_free = fol;
	  curfreenode = fol;
	  cur = fol;
	  fol = fol->next_free;
	}
      }
      coal_all = FALSE;
    }
  }
  return SUCCESS;
}


void Mem_Dump() {
  header* temp = free_head;
  int index = 1;
  while(temp != NULL) {
    printf("[%d] free address header at %p with state %c\n", index, temp, temp->state);
    index ++;
    temp = temp->next_free;
  } 
}
