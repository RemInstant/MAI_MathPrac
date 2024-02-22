#include <stdlib.h>

#include "department.h"

status_code department_set_null(department* dep)
{
	if (dep == NULL)
	{
		return NULL_ARG;
	}
	dep->staff = 0;
	dep->queue = NULL;
	return OK;
}

status_code department_construct(department* dep, size_t staff, pq_base base, int (*comp)(const request*, const request*))
{
	if (dep == NULL)
	{
		return NULL_ARG;
	}
	
	dep->queue = (p_queue*) malloc(sizeof(p_queue));
	if (dep->queue == NULL)
	{
		return BAD_ALLOC;
	}
	
	p_queue_set_null(dep->queue);
	status_code code = p_queue_init(dep->queue, base);
	code = code ? code : dep->queue->construct(dep->queue->ds, comp);
	if (code)
	{
		p_queue_destruct(dep->queue);
		free(dep->queue);
		return code;
	}
	dep->staff = staff;
	return OK;
}

status_code department_destruct(department* dep)
{
	if (dep == NULL)
	{
		return NULL_ARG;
	}
	p_queue_destruct(dep->queue);
	free(dep->queue);
	return OK;
}
