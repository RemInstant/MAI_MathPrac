#ifndef _INPUT_READER_H_
#define _INPUT_READER_H_

#include "utility.h"
#include "priority_queue/priority_queue.h"
#include "map/map.h"

#define CONFIG_MIN_DEP_CNT 1
#define CONFIG_MAX_DEP_CNT 100
#define CONFIG_MIN_STAFF_CNT 1
#define CONFIG_MAX_STAFF_CNT 50
#define CONFIG_MIN_OVERLOAD_COEF 1.0

status_code setup_config(const char* path, Map* dep_map, size_t* dep_cnt, char*** dep_names,
                                char st_time[21], char end_time[21], double eps);

// no read after get
typedef struct Input_reader
{
    size_t size;
    size_t cap;
    request** data;
    size_t front;
    
    size_t max_prior;
    size_t dep_cnt;
    const char** dep_names;
} Input_reader;

status_code ir_set_null(Input_reader* ir);
status_code ir_construct(Input_reader* ir, size_t max_prior, size_t dep_cnt, const char** dep_names);
status_code ir_destruct(Input_reader* ir);

status_code ir_read_file(Input_reader* ir, const char* path); // O(n^2) !!!
status_code ir_get_up_to(Input_reader* ir, const char time[21], size_t* req_cnt, request*** reqs);

#endif // _INPUT_READER_H_