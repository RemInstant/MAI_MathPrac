#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utility.h"
#include "department.h"
#include "readers.h"
#include "logger.h"

#include "priority_queue/binomial_heap.h"
#include "priority_queue/priority_queue.h"

#include "map/trie.h"
#include "map/map.h"

// TODO 
// MAPS HEAPS
// unify ctoi funcs in maps
// rename dep_id to dep_name
// check comparators equality before meld
// clever input reader?

int main(int argc, char** argv)
{
    status_code code = OK;
    Map dep_map;
    Input_reader ir;
    Logger logger;
    
    size_t max_priority = 10;
    size_t dep_cnt = 0;
    char** dep_names = NULL;
    char cur_time[21];
    char end_time[21];
    
    srand(time(NULL));
    
    map_set_null(&dep_map);
    ir_set_null(&ir);
    logger_set_null(&logger);
    
    code = code ? code : setup_config("config", &dep_map, &dep_cnt, &dep_names, cur_time, end_time, 1e-9);
    code = code ? code : ir_construct(&ir, cur_time, max_priority, dep_cnt, (const char**) dep_names);
    code = code ? code : logger_construct(&logger, "log");
    code = code ? code : ir_read_file(&ir, "request_1");
    
    while (!code && strcmp(cur_time, end_time) <= 0)
    {
        Department* dep = NULL;
        size_t msg_cnt = 0;
        dep_msg* msgs = NULL;
        size_t req_cnt = 0;
        request** reqs = NULL;
        
        code = code ? code : ir_get_up_to(&ir, cur_time, &req_cnt, &reqs);
        
        // HANDLE TASK FINISHING
        for (size_t i = 0; !code && i < dep_cnt; ++i)
        {
            code = code ? code : map_get(&dep_map, dep_names[i], &dep);
            code = code ? code : department_handle_finishing(dep, cur_time, &msg_cnt, &msgs);
            code = code ? code : logger_multi_log(&logger, cur_time, msg_cnt, msgs);
            free(msgs);
            msgs = NULL;
        }
        
        // HANDLE TASK READING + STARTING
        for (size_t i = 0; !code && i < req_cnt; ++i)
        {
            code = code ? code : map_get(&dep_map, reqs[i]->dep_id, &dep);
            code = code ? code : department_add_request(dep, cur_time, reqs[i], &msg_cnt, &msgs);
            
            if (!code && msg_cnt == 2 && msgs[1].code == DEPARTMENT_OVERLOADED)
            {
                size_t task_cnt = 0;
                int can_take = 0;
                Department* dep_taker = NULL;
                
                code = code ? code : department_get_task_cnt(dep, &task_cnt);
                
                for (size_t j = 0; !code && !can_take && j < dep_cnt; ++j)
                {
                    code = code ? code : map_get(&dep_map, dep_names[j], &dep_taker);
                    code = code ? code : department_can_handle_transfer(dep_taker, task_cnt, &can_take);
                }
                if (can_take)
                {
                    code = code ? code : department_transfer(dep_taker, dep);
                    msgs[1].transfer_dep_id = dep_taker->id;
                }
            }
            
            code = code ? code : logger_multi_log(&logger, cur_time, msg_cnt, msgs);
            
            free(msgs);
            msgs = NULL;
        }
        
        // HANDLE EXTRA TASK STARTING
        for (size_t i = 0; !code && i < dep_cnt; ++i)
        {
            code = code ? code : map_get(&dep_map, dep_names[i], &dep);
            code = code ? code : department_handle_starting(dep, cur_time, &msg_cnt, &msgs);
            code = code ? code : logger_multi_log(&logger, cur_time, msg_cnt, msgs);
            free(msgs);
            msgs = NULL;
        }
        
        free(reqs);
        
        iso_time_add(cur_time, 60, cur_time);
    }
    
    
    
    map_destruct(&dep_map);
    ir_destruct(&ir);
    logger_destruct(&logger);
    
    for (size_t i = 0; i < dep_cnt; ++i)
    {
        free(dep_names[i]);
    }
    free(dep_names);
    
    if (code)
    {
        print_error(code, 1);
    }
    
    return code;
}