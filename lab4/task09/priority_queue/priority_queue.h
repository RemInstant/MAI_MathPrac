#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include "../utility.h"

typedef enum priority_queue_base
{
	PQB_BINARY,
	PQB_LEFTIST,
	PQB_SKEW,
	PQB_BINOM,
	PQB_FIB,
	PQB_TREAP,
} priority_queue_base, pq_base;

typedef struct Priority_queue
{
	void* ds;
	status_code (*set_null)(void* ds);
	status_code (*construct)(void* ds, int (*compare)(const pair_prior_time*, const pair_prior_time*));
	status_code (*copy)(void* ds_dest, const void* ds_src);
	status_code (*destruct)(void* ds);
	status_code (*meld)(void* ds_res, void* ds_l, void* ds_r);
	status_code (*copy_meld)(void* ds_res, const void* ds_l, const void* ds_r);
	status_code (*size)(void* ds, size_t* size);
	status_code (*top)(void* ds, char** value);
	status_code (*pop)(void* ds, char** value);
	status_code (*insert)(void* ds, pair_prior_time key, char* value);
} Priority_queue, p_queue;

status_code p_queue_set_null(p_queue* pq);
status_code p_queue_init(p_queue* pq, pq_base base);
status_code p_queue_destruct(p_queue* pq);

int compare_pq_key(const pair_prior_time* lhs, const pair_prior_time* rhs);

#endif // _PRIORITY_QUEUE_H_