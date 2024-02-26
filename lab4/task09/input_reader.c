#include <stdlib.h>
#include <string.h>

#include "input_reader.h"

// SOOO RAW CODE, DO NOT WATCH IT

status_code ir_set_null(Input_reader* ir)
{
    if (ir == NULL)
    {
        return NULL_ARG;
    }
    ir->data_queue = NULL;
    return OK;
}

status_code ir_construct(Input_reader* ir)
{
    if (ir == NULL)
    {
        return NULL_ARG;
    }
    
    ir->data_queue = (p_queue*) malloc(sizeof(p_queue));
    if (ir->data_queue == NULL)
    {
        return BAD_ALLOC;
    }
    
    p_queue_set_null(ir->data_queue);
    return p_queue_construct(ir->data_queue, PQB_BINOM, compare_request);
}

status_code ir_destruct(Input_reader* ir)
{
    
}


status_code ir_read_file(Input_reader* ir, const char* path);
status_code ir_get_up_to(Input_reader* ir, const char time[21], size_t* req_cnt, request** reqs);