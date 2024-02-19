#include <stdlib.h>
#include "utility.h"
#include "binomial_heap.h"

int compare_key(const unsigned lhs, const unsigned rhs)
{
	return lhs > rhs;
}

int main()
{
	bm_heap bmh;
	bm_heap_construct(&bmh, compare_key);
	
	bm_heap_insert(&bmh, 5, "5");
	bm_heap_insert(&bmh, 3, "3");
	bm_heap_insert(&bmh, 8, "8");
	bm_heap_insert(&bmh, 6, "6");
	bm_heap_insert(&bmh, 1, "1");
	bm_heap_insert(&bmh, 7, "7");
	bm_heap_insert(&bmh, 2, "2");
	bm_heap_insert(&bmh, 4, "4");
	
	for (size_t i = 0; i < 8; ++i)
	{
		char* val;
		bm_heap_pop(&bmh, &val);
		printf("%s\n", val);
		free(val);
	}
	
	bm_heap_destruct(&bmh);
}