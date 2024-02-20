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
} priority_queue_base, p_queue_base;

typedef struct priority_queue
{
	void* heap;
	status_code (*construct)(struct priority_queue* pq, int (*compare)(unsigned, unsigned));
	status_code (*copy)(struct priority_queue* pq_dest, struct priority_queue pq_src);
	status_code (*destruct)(struct priority_queue* pq);
	status_code (*meld)(struct priority_queue* pq_res, struct priority_queue* pq_l, struct priority_queue* pq_r);
	status_code (*copy_meld)(struct priority_queue* pq_res, struct priority_queue pq_l, struct priority_queue pq_r);
	status_code (*top)(struct priority_queue pq, char** value);
	status_code (*pop)(struct priority_queue* pq, char** value);
	status_code (*insert)(struct priority_queue* pq, unsigned key, char* value);
} priority_queue, p_queue;

status_code p_queue_init(p_queue* pq, p_queue_base base);

#endif // _PRIORITY_QUEUE_H_