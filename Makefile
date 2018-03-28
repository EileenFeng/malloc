CC = gcc
FLAGS = -Wall -fpic -g

all: libmalloc.so

libmalloc.so : mem.o list.o
	$(CC) -o libmalloc.so list.o mem.o -shared

mem.o: mem.c mem.h list.c list.h
	$(CC) $(FLAGS) -c list.c mem.c

clean:
	rm *.o libmalloc.so
