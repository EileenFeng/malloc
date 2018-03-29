CC = gcc
FLAGS = -Wall -fpic -g

all: tinit talloc t_sim_free tcoal tcall kcoal kncoal

kcoal: kyu_two_mil_coal.c libmalloc.so
	$(CC) -o kcoal -Wall kyu_two_mil_coal.c -L. -lmalloc -lm

kncoal: kyu_two_mil_no_coal.c libmalloc.so
	$(CC) -o kncoal -Wall kyu_two_mil_no_coal.c -L. -lmalloc -lm

tcall: test_free_coalesce_all.c libmalloc.so
	$(CC) -o tcall -Wall test_free_coalesce_all.c -L. -lmalloc -lm

tcoal: test_free_coalesce_part.c libmalloc.so
	$(CC) -o tcoal -Wall test_free_coalesce_part.c -L. -lmalloc -lm

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
	rm *.o libmalloc.so tinit talloc t_sim_free tcoal tcall kcoal kncoal
