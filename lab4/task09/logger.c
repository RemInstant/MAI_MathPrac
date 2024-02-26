#include <stdlib.h>
#include <stdio.h>

#include "logger.h"

status_code logger_set_null(Logger* log)
{
    if (log == NULL)
    {
        return NULL_ARG;
    }
    
    log->file = NULL;
    
    return OK;
}

status_code logger_construct(Logger* log, const char* path)
{
    if (log == NULL || path == NULL)
    {
        return NULL_ARG;
    }
    
    log->file = fopen(path, "w");
    if (log->file == NULL)
    {
        return FILE_OPENING_ERROR;
    }
    
    return OK;
}

status_code logger_destruct(Logger* log)
{
    if (log == NULL)
    {
        return NULL_ARG;
    }
    
    fclose(log->file);
    
    return logger_set_null(log);
}


status_code logger_log(Logger* log, const char time[21], const department_message* msg)
{
    if (log == NULL || time == NULL || msg == NULL)
    {
        return NULL_ARG;
    }
    
    switch (msg->code)
    {
        case NEW_REQUEST:
        {
            if (msg->dep_id == NULL)
            {
                return INVALID_INPUT;
            }
            
            fprintf(log->file, "[%s] [NEW_REQUEST]: Request %u was received by department %s.\n",
                    time, msg->req_id, msg->dep_id);
            
            break;
        }
        case REQUEST_HANDLING_STARTED:
        {
            if (msg->oper_name == NULL)
            {
                return INVALID_INPUT;
            }
            
            fprintf(log->file, "[%s] [REQUEST_HANDLING_STARTED]: Handling of request %u was started by operator %s.\n",
                    time, msg->req_id, msg->oper_name);
            
            break;
        }
        case REQUEST_HANDLING_FINISHED:
        {
            if (msg->oper_name == NULL)
            {
                return INVALID_INPUT;
            }
            
            fprintf(log->file, "[%s] [REQUEST_HANDLING_FINISHED]: Handling of request %u was finished by operator %s in %u minutes.\n",
                    time, msg->req_id, msg->oper_name, msg->handling_time);
            
            break;
        }
        case DEPARTMENT_OVERLOADED:
        {
            if (msg->transfer_dep_id == NULL)
            {
                fprintf(log->file, "[%s] [DEPARTMENT_OVERLOADED]: Department %s was overloaded on receiving request %u. ",
                        time, msg->req_id, msg->dep_id);
                fprintf(log->file, "No transfer can be executed.\n");
            }
            else
            {
                fprintf(log->file, "[%s] [DEPARTMENT_OVERLOADED]: Department %s was overloaded on receiving request %u. ",
                        time, msg->req_id, msg->dep_id);
                fprintf(log->file, "Its request was transfered to department %s\n",
                        msg->transfer_dep_id);
            }
            break;
        }
    }
}
 