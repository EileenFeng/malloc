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
static int totalbytes = 0;

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
  totalbytes = size_of_region;
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
    if(alloc_head > free_target) {
      alloc_head = free_target;
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
  printf("actually return address in malloc is %p\n", (char*)new_alloc+HEADER_SIZE);
  return (void*)((char*)new_alloc + HEADER_SIZE);  
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
  if(prev_free == NULL) {
    next_free = free_head;
    free_head = target;
  } else {
    next_free = prev_free->next;
    prev_free->next = target;
  }
  new_header(target, prev_free, next_free, FREE, prev_alloc);
  
  
  if(prev_alloc != NULL) {
    prev_alloc->next = next_alloc;
  } else {
    alloc_head = next_alloc;
  }
  if(free_head > target) {
    free_head = target;
  }
  
  if(next_alloc != NULL) {
    // update for next_alloc and next_free
    next_alloc->prev = prev_alloc;
    if(next_alloc->prev_header == NULL) {
      next_alloc->prev_header = target;
    } else {
      if(target > next_alloc->prev_header) {
	next_alloc->prev_header = target;
      }
    }
  }
  
  if(next_free != NULL) {
    next_free->prev = target;
    if(next_free->prev_header != NULL) {
      if(next_free->prev_header == target) {
	next_free->prev_header = prev_alloc;
      }
    }
  }
    // update for next_alloc and next_free
  
  if(coalesce == TRUE) {
    if(coal_all == FALSE) {
      header* result_free = NULL;
      if(prev_free != NULL) {
	if((char*)prev_free + HEADER_SIZE + prev_free->size == target) {
	  result_free = prev_free;
	} else {
	  result_free = target;
	}
      }
      
      
      
      
      //old
      // first get the first free header after next_alloc
      header* tempfree = target;
      while(tempfree != NULL) {
	if(tempfree > next_alloc) {
	  break;
	} else {
	  tempfree = tempfree->next;
	}
      }
      header* result_free = NULL;
      if(prev_free == NULL) {
	result_free = target;
      } else {
	result_free = prev_free;
      }
      result_free->size = (char*)result_free - (char)next_alloc - HEADER_SIZE;                                                  
      result_free->next = tempfree;
      if(tempfree != NULL) {
	tempfree->prev = result_free;
	header* tempalloc = next_alloc;
	while(tempalloc != NULL) {
	  if(tempalloc < tempfree) {
	    tempalloc->prev_header = result_free;
	  }
	  tempalloc = tempalloc->next;
	}
      } else {
	header* tempalloc = next_alloc;
	while(tempalloc != NULL) {
	  tempalloc->prev_header = result_free;
	  tempalloc = tempalloc->next;
	}
      }





      
    } else {
      // just freed a node, curfreehead and prev cannot be NULL
      header* curfreehead = free_head;
      header* prevcoal = curfreehead;
      header* nextcoal = curfreehead->next;
      while(nextcoal != NULL) {
	printf("curfreehead is %p prevfree is %p and next free %p\n", curfreehead, prevcoal, nextcoal);
	if((char*)nextcoal - (char*)prevcoal == HEADER_SIZE + prevcoal->size) {
	  prevcoal = nextcoal;
	  nextcoal = nextcoal->next;
	  curfreehead->size += prevcoal->size + HEAEDER_SIZE;
	} else {
	  curfreehead->next = nextcoal;
	  nextcoal->prev = curfreehead;
	  curfreehead = nextcoal;
	  prevcoal = nextcoal;
	  nextcoal = prevcoal->next;
	}
      }
      curfreehead->size += prevcoal->size + HEADER_SIZE;
      curfreehead->next = NULL;

      // organize the list
      header* recentfree = free_head;
      header* recentalloc = alloc_head;
      header* cur = free_head > alloc_head ? alloc_head : free_head;
      long counter = 0;
      while(counter <= totalbytes) {
	printf("in last loop free cur address is %p\n", cur);
	if(cur->alloc_or_free == FREE) {
	  // do not need to compare
	  if(cur > recentfree) {
	    recentfree = cur;
	  }
	  if(recentalloc < cur) {
	    cur->prev_header = recentalloc;
	  } else {
	    cur->prev_header = NULL;
	  }
	  
	} else {
	  if(cur > recentalloc) {
	    recentalloc = cur;
	  }
	  if(recentfree < cur) {
	    cur->prev_header = recentfree;
	  } else {
	    cur->prev_header = NULL;
	  }
	}
	header* h = cur;
	cur = (header*) ((char*)h + h->size + HEADER_SIZE);
	counter += h->size + HEADER_SIZE;
      }
      
      coal_all = FALSE; 
    }
  } // end of else for coalesce
  
  
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
