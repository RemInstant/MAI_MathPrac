CC = gcc
CFLAGS = -g -Wall -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wnull-dereference -fdiagnostics-color=always -std=c17 -lm

PQ_H = priority_queue/priority_queue.h priority_queue/binomial_heap.h
PQ_O = gcc_obj/priority_queue.o gcc_obj/binomial_heap.o
PQ_O_TRG = priority_queue.o binomial_heap.o

MAP_H = map/map.h map/trie.h
MAP_O = gcc_obj/map.o gcc_obj/trie.o
MAP_O_TRG = map.o trie.o

all: main pq_test map_test

# MAP TESTS
map_test: map_test.o $(MAP_O_TRG) $(PQ_O_TRG) department.o utility.o
	$(CC) $(CFLAGS) -o map_test gcc_obj/map_test.o $(MAP_O) $(PQ_O) gcc_obj/department.o gcc_obj/utility.o

map_test.o: map/map_test.c map/map.h department.h utility.h
	$(CC) $(CFLAGS) -c map/map_test.c -o gcc_obj/map_test.o

# PRIORITY QUEUE TESTS
pq_test: pq_test.o $(PQ_O_TRG) utility.o
	$(CC) $(CFLAGS) -o pq_test gcc_obj/pq_test.o $(PQ_O) gcc_obj/utility.o

pq_test.o: priority_queue/pq_test.c priority_queue/priority_queue.h utility.h
	$(CC) $(CFLAGS) -c priority_queue/pq_test.c -o gcc_obj/pq_test.o


# MAIN EXECUTABLE
main: task09.o $(MAP_O_TRG) $(PQ_O_TRG) department.o utility.o
	$(CC) $(CFLAGS) -o main gcc_obj/task09.o $(MAP_O) $(PQ_O) gcc_obj/department.o gcc_obj/utility.o

task09.o: task09.c priority_queue/priority_queue.h priority_queue/binomial_heap.h utility.h
	$(CC) $(CFLAGS) -c task09.c -o gcc_obj/task09.o

# MAP
map.o: map/map.c map/map.h department.h utility.h
	$(CC) $(CFLAGS) -c map/map.c -o gcc_obj/map.o

trie.o: map/trie.c map/trie.h department.h utility.h
	$(CC) $(CFLAGS) -c map/trie.c -o gcc_obj/trie.o

# PRIORITY QUEUE
priority_queue.o: priority_queue/priority_queue.c priority_queue/priority_queue.h utility.h
	$(CC) $(CFLAGS) -c priority_queue/priority_queue.c -o gcc_obj/priority_queue.o

binomial_heap.o: priority_queue/binomial_heap.c priority_queue/binomial_heap.h utility.h
	$(CC) $(CFLAGS) -c priority_queue/binomial_heap.c -o gcc_obj/binomial_heap.o


# OTHER
department.o: department.c department.h priority_queue/priority_queue.h
	$(CC) $(CFLAGS) -c department.c -o gcc_obj/department.o

utility.o: utility.c utility.h
	$(CC) $(CFLAGS) -c utility.c -o gcc_obj/utility.o
	
.PHONY: clean

clean:
	rm -force gcc_obj/*.o
	rm -force main
	rm -force pq_test
	rm -force map_test