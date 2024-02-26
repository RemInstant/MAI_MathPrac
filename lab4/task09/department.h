#ifndef _DEPARTMENT_H_
#define _DEPARTMENT_H_

#include "utility.h"
#include "priority_queue/priority_queue.h"

typedef struct Operator
{
    char* name;
    const request* req;
    char start_time[21];
    char finish_time[21];
    unsigned handling_time;
} Operator;

typedef struct Department
{
    char* id;
    p_queue* queue;
    size_t staff_size;
    Operator* staff;
    size_t free_staff_cnt;
    double load_coef;
    double overload_coef;
    double eps;
    unsigned min_handling_time;
    unsigned max_handling_time;
} Department;

typedef struct pair_str_department
{
    char* str;
    const Department* dep;
} pair_str_department, pair_str_dep;

typedef enum department_code
{
    NEW_REQUEST,
    REQUEST_HANDLING_STARTED,
    REQUEST_HANDLING_FINISHED,
    DEPARTMENT_OVERLOADED,
} department_code, dep_code;

// no copies here
typedef struct department_message
{
    dep_code code;
    unsigned req_id;
    const char* dep_id;
    const char* transfer_dep_id;
    const char* oper_name;
    unsigned handling_time;
} department_message, dep_msg;

status_code department_set_null(Department* dep);
status_code department_construct(
    Department* dep,
    size_t staff_size,
    pq_base base,
    double overload_coef,
    double eps,
    unsigned min_handling_time,
    unsigned max_handling_time,
    int (*comp)(const request*, const request*));
status_code department_destruct(Department* dep);

status_code department_handle_finishing(Department* dep, const char time[21], size_t* msg_cnt, dep_msg** msgs);
status_code department_add_request(Department* dep, request* req, size_t* msg_cnt, dep_msg** msgs);
status_code department_handle_starting(Department* dep, const char time[21], size_t* msg_cnt, dep_msg** msgs);
status_code department_can_handle_transfer(Department* dep, unsigned extra_task_cnt, int can_handle);
status_code department_transfer(Department* dep_dest, Department* dep_src);

#endif