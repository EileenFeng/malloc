CC = gcc
FLAGS = -Wall -fpic -g

all: tinitrp tinitone taligned tnotaligned tworstfit t_sim_free tcoal tcall tnospace kcoal kncoal

kcoal: kyu_two_mil_coal.c libmalloc.so
	$(CC) -o kcoal -Wall kyu_two_mil_coal.c -L. -lmalloc -lm

kncoal: kyu_two_mil_no_coal.c libmalloc.so
	$(CC) -o kncoal -Wall kyu_two_mil_no_coal.c -L. -lmalloc -lm
tnospace: test_no_space.c libmalloc.so
	$(CC) -o tnospace -Wall test_no_space.c -L. -lmalloc -lm

tcall: test_free_coalesce_all.c libmalloc.so
	$(CC) -o tcall -Wall test_free_coalesce_all.c -L. -lmalloc -lm

tcoal: test_free_coalesce_part.c libmalloc.so
	$(CC) -o tcoal -Wall test_free_coalesce_part.c -L. -lmalloc -lm

t_sim_free: test_simple_free.c libmalloc.so
	$(CC) -o t_sim_free -Wall test_simple_free.c -L. -lmalloc -lm

tworstfit: test_worstfit_alloc.c libmalloc.so
	$(CC) -o tworstfit -Wall test_worstfit_alloc.c -L. -lmalloc -lm

tnotaligned: test_alloc_not_aligned.c libmalloc.so
	$(CC) -o tnotaligned -Wall test_alloc_not_aligned.c -L. -lmalloc -lm

taligned: test_alloc_aligned.c libmalloc.so
	$(CC) -o taligned -Wall test_alloc_aligned.c -L. -lmalloc -lm

tinitone: test_init_one_page.c libmalloc.so
	$(CC) -o tinitone -Wall test_init_one_page.c -L. -lmalloc -lm

tinitrp: test_init_roundup.c libmalloc.so
	$(CC) -o tinitrp -Wall test_init_roundup.c -L. -lmalloc -lm

libmalloc.so : mem.o header.o
	$(CC) -o libmalloc.so mem.o header.o -shared

mem.o: mem.c mem.h header.c header.h
	$(CC) $(FLAGS) -c mem.c header.c

clean:
	rm *.o libmalloc.so tinitrp tinitone taligned tnotaligned tworstfit t_sim_free tcoal tcall tnospace kcoal kncoal
