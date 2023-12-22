CC = gcc
CFLAGS = -g -Wall -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wnull-dereference -fdiagnostics-color=always -std=c17 -lm

main: task01.o hash_table.o utility.o
	$(CC) $(CFLAGS) -o main task01.o hash_table.o utility.o

task01.o: task01.c hash_table.h utility.h
	$(CC) $(CFLAGS) -c task01.c

hash_table.o: hash_table.c hash_table.h utility.h
	$(CC) $(CFLAGS) -c hash_table.c

utility.o: utility.c utility.h
	$(CC) $(CFLAGS) -c utility.c
	
.PHONY: clean

clean:
	rm -f *.o main