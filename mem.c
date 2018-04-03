#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
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
static header* prev_biggest = NULL;
static header* biggest = NULL;
static header* second_big = NULL;
static header* prev_second_big = NULL;
static int init = FALSE;
static int coal_all = FALSE;
void* end_address;

static void set_biggest() {
  header* before_traverse = NULL;
  header* traverse = free_head;
  long maxsize = 0;
  while(traverse != NULL) {
    header* temp = traverse->next;
    long size = temp != NULL ? (char*)temp - (char*)traverse - HEADER_SIZE : (char*)end_address - (char*)traverse - HEADER_SIZE;
    
    if(size > maxsize) {
      maxsize = size;
      prev_second_big = prev_biggest;
      second_big = biggest;
      prev_biggest = before_traverse;;
      biggest = traverse;
    }
    before_traverse = traverse;
    traverse = traverse->next_free;
  }
}

static void combine_freelist() {
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

int Mem_Init(long sizeofRegion) {
  if(init == TRUE) {
    m_error = E_BAD_ARGS;
    return FAIL;
  }
  if(sizeofRegion <= 0) {
    m_error = E_BAD_ARGS;
    return FAIL;
  }
  
  long byte_roundup = sizeofRegion % ALIGNED == 0 ? (long)(sizeofRegion / ALIGNED) : (long) round(sizeofRegion * 1.0f / ALIGNED + 0.5f);
  long byte_num = byte_roundup * ALIGNED * EXPAND;
  long region_size = byte_num % getpagesize() == 0 ? (long)(byte_num / getpagesize()) : (long) round(byte_num * 1.0f / getpagesize() + 0.5f);
  long size_of_region = region_size * getpagesize();
  if((free_head = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == (void*) -1) {
    m_error = E_BAD_ARGS;
    perror("Failed mmap:");
    return FAIL;
  }
  bzero(free_head, size_of_region);
  end_address = (void*)((long)((char*)free_head + size_of_region));
  new_header(free_head, NULL, NULL, FREE, NULL);
  init = TRUE;
  return SUCCESS;   
}

void *Mem_Alloc(long size) {
  if(size <= 0) {
    m_error = E_BAD_ARGS;
    return NULL;
  }

  long unit_num = size % ALIGNED == 0 ? (long)(size / ALIGNED) : (long) round(size * 1.0f / ALIGNED + 0.5f);
  long actual_assigned = (long) unit_num * ALIGNED;
  header* before_target = NULL;
  header* target = NULL;

  // get the max node
  if(biggest == NULL) {
    set_biggest();   
  }
  
  target = biggest;
  before_target = prev_biggest;

  if(target == NULL) {
    m_error = E_NO_SPACE;
    return NULL;
  }
    
  if(target->state == ALLOC) {
    m_error = E_CORRUPT_FREESPACE;
    return NULL;
  }
  if(target->canary_start != CSTART || target->canary_end != CEND) {
    m_error = E_CORRUPT_FREESPACE;
    return NULL;
  }   
  
  header* nexth = target->next;
  header* next_free = target->next_free;
  header* new_free = NULL;
  long maxsize = nexth == NULL ? (char*)end_address - (char*)target - HEADER_SIZE : (char*)nexth - (char*)target - HEADER_SIZE;
  target->state = ALLOC;
  target->next_free = NULL;      
  
  if(maxsize < actual_assigned) {
    m_error = E_NO_SPACE;
    return NULL;
  }

  if(maxsize == actual_assigned) {
    if(before_target != NULL) {
      before_target->next_free = next_free;
    } else {
      free_head = next_free;
    }
    set_biggest();
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
  target->canary_start = CSTART;
  target->canary_end = CEND;
  
  if(second_big != NULL) {                                                                                                
      header* snext = second_big->next;
      long ssize = snext == NULL ? (char*)end_address - (char*)second_big - HEADER_SIZE : (char*)snext - (char*)second_big - HEADER_SIZE;
      long bsize = maxsize - actual_assigned;
      if(bsize < ssize) {
	set_biggest();
      } else {
	biggest = new_free;
      }
  } else {                                                                                                                
      biggest = new_free;
  }
  return (void*)((char*)target + HEADER_SIZE);
}

int Mem_Free(void* ptr, int coalesce) {
  if(ptr == NULL && coalesce == FALSE) {
    m_error = E_BAD_POINTER;
    return FAIL;
  }

  if(ptr == NULL && coalesce == TRUE) {
    combine_freelist();
    return SUCCESS;
  }
  
  header* target = (header*)((char*) ((header*)ptr) - HEADER_SIZE);
  if(target->state == FREE) {
    m_error = E_BAD_POINTER;
    return FAIL;
  }
  if(target->canary_end != CEND || target->canary_start != CSTART) {
    m_error = E_PADDING_OVERWRITTEN;
    return FAIL;
  }
  
  target->state = FREE;
  header* prev_free = NULL;
  header* after_free = free_head;

  if(free_head == NULL) {
    free_head = target;
    target->next_free = NULL;
    return SUCCESS;
  }
  
  if(target > free_head) {
    header* temp = target->prev;
    while(temp != NULL) {
      if(temp->state == FREE){
	prev_free = temp;
	after_free = temp->next_free;
	break;
      }
      temp = temp->prev;
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
      combine_freelist();
    }
  }
  biggest = NULL;
  prev_biggest = NULL;
  second_big = NULL;
  prev_second_big = NULL;
  return SUCCESS;
}


void Mem_Dump() {
  if(free_head == NULL) {
    printf("No free memory available\n");
    return;
  }
  header* temp = free_head;
  int index = 1;
  while(temp != NULL) {
    header* hnext = temp->next;
    long size = hnext != NULL ? (char*)hnext - (char*)temp: (char*)end_address - (char*)temp;
    printf("[%d] free address header at %p and state %c, free size counting  header: %ld; free size not counting header: %ld\n", index, (char*)temp, temp->state, size, size - HEADER_SIZE);
    index ++;
    temp = temp->next_free;
  } 
}
