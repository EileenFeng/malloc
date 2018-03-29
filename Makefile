CC = gcc
FLAGS = -Wall -fpic -g

all: tinit talloc t_sim_free tcoal

tcoal: test_free_coalesce.c libmalloc.so
	$(CC) -o tcoal -Wall test_free_coalesce.c -L. -lmalloc -lm

t_sim_free: test_simple_free.c libmalloc.so
	$(CC) -o t_sim_free -Wall test_simple_free.c -L. -lmalloc -lm

talloc: test_alloc.c libmalloc.so
	$(CC) -o talloc -Wall test_alloc.c -L. -lmalloc -lm

tinit: test_init.c libmalloc.so
	$(CC) -o tinit -Wall test_init.c -L. -lmalloc -lm

libmalloc.so : mem.o header.o
	$(CC) -o libmalloc.so mem.o header.o -shared

mem.o: mem.c mem.h header.c header.h
	$(CC) $(FLAGS) -c mem.c header.c

clean:
	rm *.o libmalloc.so tinit talloc t_sim_free tcoal
