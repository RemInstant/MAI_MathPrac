#ifndef _DEPARTMENT_H_
#define _DEPARTMENT_H_

#include "utility.h"
#include "priority_queue/priority_queue.h"

typedef struct department
{
	size_t staff;
	p_queue* queue;
} department;

typedef struct pair_str_department
{
	char* str;
	const department* dep;
} pair_str_department, pair_str_dep;

status_code department_set_null(department* dep);
status_code department_construct(department* dep, size_t staff, pq_base base, int (*comp)(const pair_prior_time*, const pair_prior_time*));
status_code department_destruct(department* dep);

#endif