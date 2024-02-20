#include "priority_queue.h"
#include "binomial_heap.h"

status_code p_queue_init(p_queue* pq, p_queue_base base)
{
	if (pq == NULL)
	{
		return NULL_ARG;
	}
	
	pq->heap = NULL;
	
	switch (base)
	{
		case PQB_BINARY:
		case PQB_LEFTIST:
		case PQB_SKEW:
		case PQB_BINOM:
		case PQB_FIB:
		case PQB_TREAP:
		{
			pq->construct = bm_heap_construct;
			pq->copy = bm_heap_copy;
			pq->destruct = bm_heap_destruct;
			pq->meld = bm_heap_meld;
			pq->copy_meld = bm_heap_copy_meld;
			pq->top = bm_heap_top;
			pq->pop = bm_heap_pop;
			pq->insert = bm_heap_insert;
			break;
		}
	}
	
	return OK;
}