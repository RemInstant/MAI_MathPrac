#ifndef _INPUT_READER_H_
#define _INPUT_READER_H_

#include "utility.h"
#include "priority_queue/priority_queue.h"

typedef struct Input_reader
{
    p_queue* data_queue;
} Input_reader;

status_code ir_set_null(Input_reader* ir);
status_code ir_construct(Input_reader* ir);
status_code ir_destruct(Input_reader* ir);

status_code ir_read_file(Input_reader* ir, const char* path);
status_code ir_get_up_to(Input_reader* ir, const char time[21], size_t* req_cnt, request** reqs);

#endif // _INPUT_READER_H_