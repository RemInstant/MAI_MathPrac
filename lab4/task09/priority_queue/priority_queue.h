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

// Priority queue contains POINTERS to requests
typedef struct Priority_queue
{
	void* ds;
	status_code (*set_null)(void* ds);
	status_code (*construct)(void* ds, int (*compare)(const request*, const request*));
	status_code (*copy)(void* ds_dest, const void* ds_src);
	status_code (*destruct)(void* ds);
	status_code (*meld)(void* ds_res, void* ds_l, void* ds_r);
	status_code (*copy_meld)(void* ds_res, const void* ds_l, const void* ds_r);
	status_code (*size)(void* ds, size_t* size);
	status_code (*top)(void* ds, request** req); // mallocs result
	status_code (*pop)(void* ds, request** req);
	status_code (*insert)(void* ds, const request* req); // mallocs copy of input
} Priority_queue, p_queue;

status_code p_queue_set_null(p_queue* pq);
status_code p_queue_init(p_queue* pq, pq_base base);
status_code p_queue_destruct(p_queue* pq);

int compare_request(const request* lhs, const request* rhs);

#endif // _PRIORITY_QUEUE_H_