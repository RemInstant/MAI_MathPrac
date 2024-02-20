CC = gcc
CFLAGS = -g -Wall -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wnull-dereference -fdiagnostics-color=always -std=c17 -lm

pq_test: pq_test.o priority_queue.o binomial_heap.o utility.o
	$(CC) $(CFLAGS) -o pq_test pq_test.o priority_queue.o binomial_heap.o utility.o

pq_test.o: pq_test.c priority_queue.h utility.h
	$(CC) $(CFLAGS) -c pq_test.c

main: task09.o priority_queue.o binomial_heap.o utility.o
	$(CC) $(CFLAGS) -o main task09.o priority_queue.o binomial_heap.o utility.o

task09.o: task09.c priority_queue.h binomial_heap.h utility.h
	$(CC) $(CFLAGS) -c task09.c

priority_queue.o: priority_queue.c priority_queue.h utility.h
	$(CC) $(CFLAGS) -c priority_queue.c

binomial_heap.o: binomial_heap.c binomial_heap.h utility.h
	$(CC) $(CFLAGS) -c binomial_heap.c

utility.o: utility.c utility.h
	$(CC) $(CFLAGS) -c utility.c
	
.PHONY: clean

clean:
	rm -f *.o main