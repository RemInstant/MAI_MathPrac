CC = gcc
CFLAGS = -g -Wall -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wnull-dereference -fdiagnostics-color=always -std=c17 -lm

main: task03.o polynomial.o trie.o utility.o
	$(CC) $(CFLAGS) -o main task03.o polynomial.o trie.o utility.o

task03.o: task03.c polynomial.h trie.h utility.h
	$(CC) $(CFLAGS) -c task03.c

polynomial.o: polynomial.c polynomial.h trie.h utility.h
	$(CC) $(CFLAGS) -c polynomial.c

trie.o: trie.c trie.h utility.h
	$(CC) $(CFLAGS) -c trie.c

utility.o: utility.c utility.h
	$(CC) $(CFLAGS) -c utility.c
	
.PHONY: clean

clean:
	rm -f *.o main