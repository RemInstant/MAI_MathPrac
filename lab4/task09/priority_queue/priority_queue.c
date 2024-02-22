#include <stdlib.h>
#include <string.h>

#include "priority_queue.h"
#include "binomial_heap.h"

status_code p_queue_set_null(p_queue* pq)
{
	if (pq == NULL)
	{
		return NULL_ARG;
	}
	pq->ds = NULL;
	pq->construct = NULL;
	pq->copy = NULL;
	pq->destruct = NULL;
	pq->meld = NULL;
	pq->copy_meld = NULL;
	pq->top = NULL;
	pq->pop = NULL;
	pq->insert = NULL;
	return OK;
}

status_code p_queue_init(p_queue* pq, pq_base base)
{
	if (pq == NULL)
	{
		return NULL_ARG;
	}
	
	pq->ds = NULL;
	
	switch (base)
	{
		case PQB_BINARY:
		case PQB_LEFTIST:
		case PQB_SKEW:
		case PQB_BINOM:
		case PQB_FIB:
		case PQB_TREAP:
		{
			pq->ds = malloc(sizeof(bm_heap));
			pq->set_null = bm_heap_set_null;
			pq->construct = bm_heap_construct;
			pq->copy = bm_heap_copy;
			pq->destruct = bm_heap_destruct;
			pq->meld = bm_heap_meld;
			pq->copy_meld = bm_heap_copy_meld;
			pq->size = bm_heap_size;
			pq->top = bm_heap_top;
			pq->pop = bm_heap_pop;
			pq->insert = bm_heap_insert;
			break;
		}
	}
	if (pq->ds == NULL)
	{
		p_queue_set_null(pq);
		return BAD_ALLOC;
	}
	return OK;
}

status_code p_queue_destruct(p_queue* pq)
{
	if (pq == NULL)
	{
		return NULL_ARG;
	}
	if (pq->ds != NULL)
	{
		pq->destruct(pq->ds);
		free(pq->ds);
	}
	p_queue_set_null(pq);
	return OK;
}

int compare_request(const request* lhs, const request* rhs)
{
	if (lhs->prior == rhs->prior)
	{
		return strcmp(lhs->time, rhs->time);
	}
	// OR MAYBE rhs->prior - lhs->prior;
	return lhs->prior > rhs->prior ? -1 : 1;
}