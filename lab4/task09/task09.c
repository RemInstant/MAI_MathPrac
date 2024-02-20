#include <stdlib.h>
#include "utility.h"
#include "binomial_heap.h"
#include "priority_queue.h"

int main()
{
	bm_heap bmh;
	bm_heap_construct(&bmh, compare_pq_key);
	
	bm_heap_insert(&bmh, (pair_prior_time) {5, "123"}, "5");
	bm_heap_insert(&bmh, (pair_prior_time) {3, "123"}, "3");
	bm_heap_insert(&bmh, (pair_prior_time) {8, "123"}, "8");
	bm_heap_insert(&bmh, (pair_prior_time) {6, "123"}, "6");
	bm_heap_insert(&bmh, (pair_prior_time) {1, "123"}, "1");
	bm_heap_insert(&bmh, (pair_prior_time) {7, "123"}, "7");
	bm_heap_insert(&bmh, (pair_prior_time) {2, "123"}, "2");
	bm_heap_insert(&bmh, (pair_prior_time) {4, "123"}, "4");
	
	for (size_t i = 0; i < 8; ++i)
	{
		char* val;
		bm_heap_pop(&bmh, &val);
		printf("%s\n", val);
		free(val);
	}
	
	bm_heap_destruct(&bmh);
}