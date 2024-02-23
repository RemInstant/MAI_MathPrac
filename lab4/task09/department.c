#include <stdlib.h>
#include <string.h>

#include "department.h"

status_code operator_give_task(Department* dep, size_t op_id, const request* req)
{
	if (dep == NULL || req == NULL)
	{
		return NULL_ARG;
	}
	if (op_id >= dep->staff_size)
	{
		return INVALID_INPUT;
	}
	
	Operator* op = &(dep->staff[op_id]);
	dep->free_staff_cnt--;
	
	op->req = req;
	op->handling_time = 5;
	strcpy(op->start_time, req->time);
	iso_time_add(op->start_time, op->handling_time * 60, op->finish_time);
	
	return OK;
}

status_code department_set_null(Department* dep)
{
	if (dep == NULL)
	{
		return NULL_ARG;
	}
	dep->queue = NULL;
	dep->staff_size = 0;
	dep->staff = NULL;
	dep->free_staff_cnt = 0;
	dep->load_coef = 0;
	dep->overload_coef = 0;
	dep->eps = 0;
	dep->min_handling_time = 0;
	dep->max_handling_time = 0;
	return OK;
}

status_code department_construct(
	Department* dep,
	size_t staff_size,
	pq_base base,
	double overload_coef,
	double eps,
	unsigned min_handling_time,
	unsigned max_handling_time,
	int (*comp)(const request*, const request*))
{
	if (dep == NULL || comp == NULL)
	{
		return NULL_ARG;
	}
	
	status_code code = OK;
	
	dep->staff = (Operator*) malloc(sizeof(Operator) * staff_size);
	if (dep->staff == NULL)
	{
		return BAD_ALLOC;
	}
	for (size_t i = 0; i < staff_size; ++i)
	{
		dep->staff[i].name = NULL;
		dep->staff[i].req = NULL;
		dep->staff[i].start_time[0] = '\0';
		dep->staff[i].finish_time[0] = '\0';
		dep->staff[i].handling_time = 0;
	}
	for (size_t i = 0; i < staff_size && !code; ++i)
	{
		dep->staff[i].name = (char*) malloc(sizeof(char) * 4);
		if (dep->staff[i].name == NULL)
		{
			code = BAD_ALLOC;
		}
		else
		{
			strcpy(dep->staff[i].name, "123");
		}
	}
	
	if (!code)
	{
		dep->queue = (p_queue*) malloc(sizeof(p_queue));
		code = dep->queue == NULL ? BAD_ALLOC : OK;
	}
	
	code = code ? code : p_queue_set_null(dep->queue);
	code = code ? code : p_queue_construct(dep->queue, base, comp);
	if (code)
	{
		for (size_t i = 0; i < staff_size; ++i)
		{
			free(dep->staff[i].name);
		}
		free(dep->staff);
		p_queue_destruct(dep->queue);
		free(dep->queue);
		department_set_null(dep);
		return code;
	}
	dep->staff_size = staff_size;
	dep->free_staff_cnt = staff_size;
	dep->load_coef = 0;
	dep->overload_coef = overload_coef;
	dep->eps = eps;
	dep->min_handling_time = min_handling_time;
	dep->max_handling_time = max_handling_time;
	return OK;
}

status_code department_destruct(Department* dep)
{
	if (dep == NULL)
	{
		return NULL_ARG;
	}
	p_queue_destruct(dep->queue);
	free(dep->queue);
	free(dep->staff);
	department_set_null(dep);
	return OK;
}

status_code department_update_load_coef(Department* dep)
{
	if (dep == NULL)
	{
		return NULL_ARG;
	}
	
	size_t task_cnt = 0;
	status_code code = p_queue_size(dep->queue, &task_cnt);
	if (code)
	{
		return code;
	}
	
	dep->load_coef = 1.0 * task_cnt / dep->staff_size;
	
	return OK;
}

status_code department_handle_finishing(Department* dep, const char time[21], size_t* msg_cnt, dep_msg** msgs)
{
	if (dep == NULL || time == NULL || msg_cnt == NULL || msgs == NULL)
	{
		return NULL_ARG;
	}
	
	size_t msg_cnt_tmp = 0;
	dep_msg* msgs_tmp = NULL;
	
	for (size_t i = 0; i < dep->staff_size; ++i)
	{
		Operator* op = &(dep->staff[i]);
		if (!strcmp(time, op->finish_time))
		{
			++msg_cnt_tmp;
		}
	}
	
	if (msg_cnt_tmp == 0)
	{
		*msg_cnt = 0;
		*msgs = NULL;
		return OK;
	}
	
	msgs_tmp = (dep_msg*) malloc(sizeof(dep_msg) * msg_cnt_tmp);
	if (msgs_tmp == NULL)
	{
		return BAD_ALLOC;
	}
	
	for (size_t i = 0, j = 0; i < dep->staff_size; ++i)
	{
		Operator* op = &(dep->staff[i]);
		if (!strcmp(time, op->finish_time))
		{
			msgs_tmp[j].code = REQUEST_HANDLING_FINISHED;
			msgs_tmp[j].req_id = op->req->id;
			msgs_tmp[j].handling_time = op->handling_time;
			msgs_tmp[j].oper_name = op->name;
			++j;
			
			op->req = NULL;
			op->start_time[0] = '\0';
			op->finish_time[0] = '\0';
			op->handling_time = 0;
		}
	}
	
	dep->free_staff_cnt += msg_cnt_tmp;
	*msg_cnt = msg_cnt_tmp;
	*msgs = msgs_tmp;
	return OK;
}

status_code department_add_request(Department* dep, const request* req, size_t* msg_cnt, dep_msg** msgs)
{
	if (dep == NULL || req == NULL || msg_cnt == NULL || msgs == NULL)
	{
		return NULL_ARG;
	}
	
	size_t msg_cnt_tmp = 1;
	dep_msg* msgs_tmp = (dep_msg*) malloc(sizeof(dep_msg) * 2);
	if (msgs_tmp == NULL)
	{
		return BAD_ALLOC;
	}
	
	msgs_tmp[0].code = NEW_REQUEST;
	msgs_tmp[0].req_id = req->id;
	
	status_code code = OK;
	size_t free_id = 0;
	if (dep->free_staff_cnt > 0)
	{
		while (dep->staff[free_id].req != NULL)
		{
			++free_id;
		}
		code = code ? code : operator_give_task(dep, free_id, req);
		msgs_tmp[1].code = REQUEST_HANDLING_STARTED;
		msgs_tmp[1].req_id = req->id;
		msgs_tmp[1].oper_name = dep->staff[free_id].name; // <---------------------------- mb copy???????????
		msg_cnt_tmp = 2;
	}
	else
	{
		code = code ? code : p_queue_insert(dep->queue, req);
		code = code ? code : department_update_load_coef(dep);
	}
	
	if (code)
	{
		free(msgs_tmp);
		return code;
	}
	
	if (dep->load_coef - dep->overload_coef > -dep->eps)
	{
		msgs_tmp[1].code = DEPARTMENT_OVERLOADED;
		msgs_tmp[1].req_id = req->id;
		msg_cnt_tmp = 2;
	}
	
	*msg_cnt = msg_cnt_tmp;
	*msgs = msgs_tmp;
	
	return OK;
}

status_code department_handle_starting(Department* dep, const char time[21], size_t* msg_cnt, dep_msg** msgs)
{
	if (dep == NULL || time == NULL || msg_cnt == NULL || msgs == NULL)
	{
		return NULL_ARG;
	}
	
	ull task_cnt = 0;
	status_code code = p_queue_size(dep->queue, &task_cnt);
	if (code)
	{
		return code;
	}
	
	size_t msg_cnt_tmp = dep->free_staff_cnt < task_cnt ? dep->free_staff_cnt : task_cnt;
	dep_msg* msgs_tmp = (dep_msg*) malloc(sizeof(dep_msg) * msg_cnt_tmp);
	if (msgs_tmp == NULL)
	{
		return BAD_ALLOC;
	}
	
	size_t free_id = 0;
	for (size_t i = 0; i < msg_cnt_tmp && !code; ++i)
	{
		while (dep->staff[free_id].req != NULL)
		{
			++free_id;
		}
		
		request* req = NULL;
		code = code ? code : p_queue_pop(dep->queue, &req);
		code = code ? code : operator_give_task(dep, free_id, req);
		
		msgs_tmp[i].code = REQUEST_HANDLING_STARTED;
		msgs_tmp[i].req_id = req->id;
		msgs_tmp[i].oper_name = dep->staff[free_id].name; // <---------------------------- mb copy???????????
	}
	
	if (code)
	{
		free(msgs_tmp);
		return code;
	}
	
	*msg_cnt = msg_cnt_tmp;
	*msgs = msgs_tmp;
	return OK;
}

status_code department_can_handle_transfer(Department* dep, unsigned extra_task_cnt, int can_handle);
status_code department_transfer(Department* dep_dest, Department* dep_src);