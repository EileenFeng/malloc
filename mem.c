#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/mman.h>
#include "list.h"
#include "mem.h"

#define HEADER_SIZE 32
#define TRUE 1
#define FALSE 0
#define SUCCESS 0
#define FAILURE -1
#define EXPAND 5
#define ALIGNED 8

// globals
static header* alloc_head = NULL;
static header* free_head = NULL;
static int init = FALSE;
static int coal_all = FALSE;

int Mem_Init(long sizeofRegion) {
  if (init == TRUE) {
    m_error = E_BAD_ARGS;
    return FAILURE;
  }
  if(sizeofRegion <= 0) {
    m_error = E_BAD_ARGS;
    return FAILURE;
  }
  
  long byte_roundup = (long) round(sizeofRegion * 1.0f / ALIGNED + 0.5f);
  long byte_num = byte_roundup * ALIGNED * EXPAND;
  long region_size = (long) round(byte_num * 1.0f / getpagesize() + 0.5f);
  long size_of_region = region_size * getpagesize();

  if((free_head = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON, -1, (off_t) 0)) == (void*) -1) {
    m_error = E_BAD_ARGS;
    return FAILURE;
  }

  new_header(free_head, NULL, NULL, FREE, NULL);
  free_head->size = size_of_region - HEADER_SIZE;
  printf(" in init free list %p\n", free_head);
  return SUCCESS;
}


void* Mem_Alloc(long size) {
  long unit_num = (long) round(size * 1.0f / ALIGNED + 0.5f);
  long actual_assigned = (long) unit_num * ALIGNED;
  header* free_target = NULL;
  header* traverse = free_head;
  while(traverse != NULL) {
    if(traverse->size > free_target->size) {
      free_target = traverse;
    }
    traverse = traverse->next;
  }
  if(free_target == NULL) {
    m_error = E_NO_SPACE;
    return NULL;
  }
  if(free_target->size < actual_assigned) {
    m_error = E_NO_SPACE;
    return NULL;
  }
  // when free space is corrupted
  if(free_target->canary_start != CSTART || free_target->canary_end != CEND) {
    m_error = E_CORRUPT_FREESPACE;
    return NULL;
  }  
  
  header* new_free = NULL;
  header* new_alloc = free_target;
  header* prev_alloc = free_target->prev_header;
  header* prev_free = free_target->prev;
  header* next_free = free_target->next;
  header* next_alloc = NULL;

  if(prev_alloc == NULL) {
    next_alloc = alloc_head;
    alloc_head = new_alloc;
  } else {
    prev_alloc->next = new_alloc;
    next_alloc = prev_alloc->next;
  }
  new_header(new_alloc, prev_alloc, next_alloc, ALLOC, prev_free);

  if(free_target->size == actual_assigned) {
    if(prev_free != NULL) {
      prev_free->next = next_free;
    } else {
      free_head = next_free;
    }

    // here before for chaning prevheaderfor next free
    // change the prev_header for next_alloc
    if(next_alloc != NULL) {
      next_alloc->prev = new_alloc;
      if(next_alloc->prev_header != NULL) {
	// next_alloc->prev_header is either free_target or below
	if(next_alloc->prev_header == free_target) {
	  next_alloc->prev_header = prev_free;
	}
      }
    }
  } else {
    new_free = (header*)((char*)free_target + actual_assigned);
    new_header(new_free, prev_free, next_free, FREE, new_alloc);
    if(prev_free != NULL) {
      prev_free->next = new_free;
    } else {
      free_head = new_free;
    }
    
    // change prev_head for next_free
    // change the prev_header for next_alloc
    // next_alloc must be below the free_target, otherwise would be the prev_alloc
    if(next_alloc != NULL) {
      next_alloc->prev = new_alloc;
      if(next_alloc->prev_header != NULL) {
	if(new_free > next_alloc->prev_header) {
	  next_alloc->prev_header = new_free;
	}
      }
    }
  }
  // change the prev_header for next_free
  if(next_free != NULL) {
    next_free->prev = prev_free;
    if(next_free->prev_header == NULL) {
      next_free->prev_header = new_alloc;
    } else if(new_alloc > next_free->prev_header) {
      next_free->prev_header = new_alloc;
    }
  } 
  printf("actually return address in malloc is %p\n", new_alloc+1);
  return (new_alloc + 1);  
}


int Mem_Free(void *ptr, int coalesce) {
  if(ptr == NULL) {
    return FAILURE;
  }
  
  header* target = (header*) ptr;
  header* prev_alloc = target->prev;
  header* next_alloc = target->next;
  header* prev_free = target->prev_header;
  header* next_free = NULL;

  if(coalesce == FALSE) {
    coal_all = TRUE;

    
  }
  
  
}


void Mem_Dump() {
  header* temp = free_head;
  int index = 1;
  while(temp != NULL) {
    printf("[%d] %ld bytes of free address at %p\n", index, temp->size, temp);
    index ++;
    temp = temp->next;
  }
}
