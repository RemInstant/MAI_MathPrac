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

status_code p_queue_construct(p_queue* pq, pq_base base, int (*compare)(const request*, const request*))
{
	if (pq == NULL)
	{
		return NULL_ARG;
	}
	
	pq->base = base;
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
	
	pq->set_null(pq->ds);
	status_code code = pq->construct(pq->ds, compare);
	
	if (code)
	{
		free(pq->ds);
		p_queue_set_null(pq);
		return code;
	}
	
	return OK;
}

status_code p_queue_copy(p_queue* pq_dest, const p_queue* pq_src)
{
	if (pq_dest == NULL || pq_src == NULL)
	{
		return NULL_ARG;
	}
	
	status_code code = p_queue_init(pq_dest, pq_src->base);
	code = code ? code : pq_src->copy(pq_dest->ds, pq_src->ds);
	if (code)
	{
		p_queue_destruct(pq_dest);
	}
	
	return OK;
}

status_code p_queue_destruct(p_queue* pq)
{
	if (pq == NULL)
	{
		return OK;
	}
	if (pq->ds != NULL)
	{
		pq->destruct(pq->ds);
		free(pq->ds);
	}
	p_queue_set_null(pq);
	return OK;
}


status_code p_queue_meld(p_queue* pq_res, p_queue* pq_l, p_queue* pq_r)
{
	if (pq_res == NULL || pq_l == NULL || pq_r == NULL)
	{
		return NULL_ARG;
	}
	if (pq_l->base != pq_r->base)
	{
		return INVALID_INPUT;
	}
	
	status_code code = OK;
	p_queue pq_tmp;
	
	p_queue_set_null(&pq_tmp);
	code = code ? code : p_queue_init(&pq_tmp, pq_l->base);
	code = code ? code : pq_l->meld(pq_tmp.ds, pq_l->ds, pq_r->ds);
	if (code)
	{
		p_queue_destruct(&pq_tmp);
		return code;
	}
	
	if (pq_res != pq_l)
	{
		p_queue_set_null(pq_l);
	}
	if (pq_res != pq_r)
	{
		p_queue_set_null(pq_r);
	}
	
	*pq_res = pq_tmp;
	return OK;
}

status_code p_queue_copy_meld(p_queue* pq_res, const p_queue* pq_l, const p_queue* pq_r)
{
	if (pq_res == NULL || pq_l == NULL || pq_r == NULL)
	{
		return NULL_ARG;
	}
	if (pq_l->base != pq_r->base)
	{
		return INVALID_INPUT;
	}
	
	status_code code = OK;
	p_queue pq_tmp;
	
	p_queue_set_null(&pq_tmp);
	code = code ? code : p_queue_init(&pq_tmp, pq_l->base);
	code = code ? code : pq_l->copy_meld(pq_tmp.ds, pq_l->ds, pq_r->ds);
	if (code)
	{
		p_queue_destruct(&pq_tmp);
		return code;
	}
	
	*pq_res = pq_tmp;
	return OK;
}


status_code p_queue_size(p_queue* pq, size_t* size)
{
	if (pq == NULL || size == NULL)
	{
		return NULL_ARG;
	}
	return pq->size(pq->ds, size);
}

status_code p_queue_top(p_queue* pq, request** req) // mallocs result
{
	if (pq == NULL || req == NULL)
	{
		return NULL_ARG;
	}
	return pq->top(pq->ds, req);
}

status_code p_queue_pop(p_queue* pq, request** req)
{
	if (pq == NULL || req == NULL)
	{
		return NULL_ARG;
	}
	return pq->pop(pq->ds, req);
}

status_code p_queue_insert(p_queue* pq, const request* req) // mallocs copy of input
{
	if (pq == NULL || req == NULL)
	{
		return NULL_ARG;
	}
	return pq->insert(pq->ds, req);
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
