CC = gcc
CFLAGS = -g -Wall -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wnull-dereference -fdiagnostics-color=always -std=c17 -lm

main: task04.o boolean_vector.o utility.o
	$(CC) $(CFLAGS) -o main task04.o boolean_vector.o utility.o

task04.o: task04.c boolean_vector.h utility.h
	$(CC) $(CFLAGS) -c task04.c

boolean_vector.o: boolean_vector.c boolean_vector.h utility.h
	$(CC) $(CFLAGS) -c boolean_vector.c

utility.o: utility.c utility.h
	$(CC) $(CFLAGS) -c utility.c
	
.PHONY: clean

clean:
	rm -force *.o main*