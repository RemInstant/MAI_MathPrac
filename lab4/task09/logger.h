#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "utility.h"
#include "department.h"

typedef struct Logger
{
    FILE* file;
} Logger;

status_code logger_set_null(Logger* log);
status_code logger_construct(Logger* log, const char* path);
status_code logger_destruct(Logger* log);

status_code logger_log(Logger* log, const char time[21], const department_message* msg);

#endif // _LOGGER_H_