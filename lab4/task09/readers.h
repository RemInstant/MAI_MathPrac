#ifndef _INPUT_READER_H_
#define _INPUT_READER_H_

#include "utility.h"
#include "priority_queue/priority_queue.h"
#include "map/map.h"

status_code setup_config(const char* path, Map* dep_map, size_t* dep_cnt, char*** dep_names,
                                char st_time[21], char end_time[21], double eps);

typedef struct Input_reader
{
    size_t size;
    size_t cap;
    request** data;
    size_t front;
    int sorted_by_id;
} Input_reader;

status_code ir_set_null(Input_reader* ir);
status_code ir_construct(Input_reader* ir);
status_code ir_destruct(Input_reader* ir);

status_code ir_read_file(Input_reader* ir, const char* path); // O(n^2) !!!
status_code ir_get_up_to(Input_reader* ir, const char time[21], size_t* req_cnt, request*** reqs);

#endif // _INPUT_READER_H_