#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include "utility.h"

typedef enum priority_queue_base
{
	PQB_BINARY,
	PQB_LEFTIST,
	PQB_SKEW,
	PQB_BINOM,
	PQB_FIB,
	PQB_TREAP,
} priority_queue_base, pq_base;

typedef struct priority_queue
{
	void* heap;
	status_code (*set_null)(void* pq);
	status_code (*construct)(void* pq, int (*compare)(const pair_prior_time*, const pair_prior_time*));
	status_code (*copy)(void* pq_dest, const void* pq_src);
	status_code (*destruct)(void* pq);
	status_code (*meld)(void* pq_res, void* pq_l, void* pq_r);
	status_code (*copy_meld)(void* pq_res, const void* pq_l, const void* pq_r);
	status_code (*top)(void* pq, char** value);
	status_code (*pop)(void* pq, char** value);
	status_code (*insert)(void* pq, pair_prior_time key, char* value);
} priority_queue, p_queue;

status_code p_queue_set_null(p_queue* pq);
status_code p_queue_init(p_queue* pq, pq_base base);
status_code p_queue_destruct(p_queue* pq);

int compare_pq_key(const pair_prior_time* lhs, const pair_prior_time* rhs);

#endif // _PRIORITY_QUEUE_H_