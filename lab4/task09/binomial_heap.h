#ifndef _BINOMIAL_HEAP_H_
#define _BINOMIAL_HEAP_H_

#include "utility.h"

typedef struct bmh_node
{
	pair_prior_time key;
	char* value;
	
	unsigned rank;
	struct bmh_node* son;
	struct bmh_node* brother;
	
} bmh_node;

typedef struct binomial_heap
{
	bmh_node* head;
	int (*compare)(const pair_prior_time*, const pair_prior_time*);
} binomial_heap, bm_heap;


status_code bm_heap_set_null(bm_heap* bmh);
status_code bm_heap_construct(bm_heap* bmh, int (*compare)(const pair_prior_time*, const pair_prior_time*));
status_code bm_heap_copy(bm_heap* bmh_dest, const bm_heap* bmh_src);
status_code bm_heap_destruct(bm_heap* bmh);

status_code bm_heap_meld(bm_heap* bmh_res, bm_heap* bmh_l, bm_heap* bmh_r);
status_code bm_heap_copy_meld(bm_heap* bmh_res, const bm_heap* bmh_l, const bm_heap* bmh_r);

status_code bm_heap_top(const bm_heap* bmh, char** value);
status_code bm_heap_pop(bm_heap* bmh, char** value);
status_code bm_heap_insert(bm_heap* bmh, pair_prior_time key, char* value);

#endif // _BINOMIAL_HEAP_H_