HW5 - Malloc
Name: Yazhe Feng

How it works:
- compile with 'make' and link the library with 'libmalloc.so'

Limitations:
- 'Mem_Init(regionsize)' will always consider the worst case when user allocate at most 8 bytes each time. Therefore the memory size initialized will always be 5 times the value of 'regionsize'  
- 'Mem_Init' and 'Mem_Alloc' all supports round up
- 'Mem_Dump' prints out the memory size includes and not includes the header size

Globals:
- there are four global variables that are used to help finding the biggest free memory region more efficiently. 'biggest' will always points to the biggest free memory region, and 'second_big' will always points to the second biggest memory region if there is any. Each time after memory allocation, the program will compare 'second_big' to 'biggest' to decide whether it is necessary to traverse the free list and find the new biggest and second biggest memory region. Since free list is a singly linked list, there are two more global variables used to store the previous free node of 'biggest' and 'second_big'


Tests Info:
- 'test_init_one_page.c' tests for bad args to Mem_Init() and initializing with size of 1 page;

- 'test_init_roundup.c' tests for bad args to Mem_Init() and initializing with size round up to aligned with page size. 

- 'test_alloc_not_aligned.c' includes tests for checking 8-byte alignment for all allocations, and several odd-sized allocations and free

- 'test_alloc_aligned.c' covers tests for simple 8-byte allocation, check for 8-byte aligment for all allocations, and several aligned allocations and frees

- 'test_worstfit_alloc.c' tests for worstfit allocation and simple frees without coalesce

- 'test_simple_free.c' tests for free NULL pointers (which is not allowed based on my implement and will fail) and double free certain region. Also tests for simple free without coalecse. 

- 'test_free_coalesce_part.c' tests for free with coalesce; the tests includes testing that coalescing does not blindly combines all memories but rather only coalesce freed memory regions.

- 'test_free_coalesce_all.c' tests for free with coalesce; all memory regions are freed in the tests and coalescing should end with on free memory region with size equal to the initial free memory region size before any allocations

- 'test_no_space.c' tests for allocation when all free spaces are occupied, and whether allocation will again be allowed if the the occupied spaces are freed