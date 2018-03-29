CC = gcc
FLAGS = -Wall -fpic -g

all: libmalloc.so

libmalloc.so : mem.o header.o
	$(CC) -o libmalloc.so header.o mem.o -shared

mem.o: mem.c mem.h header.c header.h
	$(CC) $(FLAGS) -c header.c mem.c

clean:
	rm *.o libmalloc.so
