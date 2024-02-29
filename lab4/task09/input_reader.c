#include <stdlib.h>
#include <string.h>

#include "input_reader.h"

// SOOO RAW CODE, DO NOT WATCH IT

int request_ptr_compare_by_id(const void* lhs, const void* rhs)
{
    if (lhs == NULL || rhs == NULL)
    {
        return 0;
    }
    
    request** req_l = (request**) lhs;
    request** req_r = (request**) rhs;
    
    return (*req_l)->id < (*req_r)->id;
}

int request_ptr_compare_by_time(const void* lhs, const void* rhs)
{
    if (lhs == NULL || rhs == NULL)
    {
        return 0;
    }
    
    request** req_l = (request**) lhs;
    request** req_r = (request**) rhs;
    
    return strcmp((*req_l)->time, (*req_r)->time);
}

status_code ir_set_null(Input_reader* ir)
{
    if (ir == NULL)
    {
        return NULL_ARG;
    }
    
    ir->size = 0;
    ir->cap = 0;
    ir->data = NULL;
    ir->front = 0;
    ir->sorted_by_id = 0;
    
    return OK;
}

status_code ir_construct(Input_reader* ir)
{
    if (ir == NULL)
    {
        return NULL_ARG;
    }
    
    ir->size = 0;
    ir->cap = 2;
    ir->data = (request**) malloc(sizeof(request*) * 2);
    ir->front = 0;
    ir->sorted_by_id = 0;
    
    if (ir->data == NULL)
    {
        ir_set_null(ir);
        return BAD_ALLOC;
    }
    
    return OK;
}

status_code ir_destruct(Input_reader* ir)
{
    if (ir == NULL)
    {
        return NULL_ARG;
    }
    
    for (size_t i = ir->front; i < ir->size; ++i)
    {
        request_destruct(ir->data[i]);
        free(ir->data[i]);
    }
    free(ir->data);
    ir_set_null(ir);
    
    return OK;
}


// 2024-01-01T12:00:00Z 000000001 2 DEP001A "this is request text"
status_code ir_read_file_line(FILE* file, request** req)
{
    if (file == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    char* time = NULL;
    char* req_id_str = NULL;
    char* prior_str = NULL;
    char* dep_id = NULL;
    char* txt = NULL;
    char check_ch;
    ull req_id;
    ull prior;
    request* req_tmp = NULL;
    
    code = code ? code : fread_word(file, &time, 1);
    if (code == FILE_END) return code;
    
    code = code ? code : fread_word(file, &req_id_str, 1);
    code = code ? code : fread_word(file, &prior_str, 1);
    code = code ? code : fread_word(file, &dep_id, 1);
    
    code = code ? code : fread_char(file, &check_ch, 1);
    code = code ? code : fread_line(file, &txt, 0);
    
    code = code ? code : iso_time_validate(time);
    code = code ? code : parse_ullong(req_id_str, 10, &req_id);
    code = code ? code : parse_ullong(prior_str, 10, &prior);
    code = code ? code : (check_ch == '"' ? OK : FILE_INVALID_CONTENT);
    code = code ? code : (txt[strlen(txt) - 1] == '"' ? OK : FILE_INVALID_CONTENT);
    
    if (!code)
    {
        txt[strlen(txt) - 1] = '\0';
        
        req_tmp = (request*) malloc(sizeof(request));
        if (req_tmp == NULL)
        {
            code = BAD_ALLOC;
        }
        else
        {
            req_tmp->id = (unsigned) req_id;
            req_tmp->prior = (unsigned) prior;
            req_tmp->dep_id = dep_id;
            req_tmp->txt = txt;
            strcpy(req_tmp->time, time);
        }
    }
    
    free(time);
    free(req_id_str);
    free(prior_str);
    
    if (code == FILE_END || code == INVALID_INPUT)
    {
        code = FILE_INVALID_CONTENT;
    }
    
    if (code)
    {
        free(dep_id);
        free(txt);
        free(req_tmp);
        return code;
    }
    
    *req = req_tmp;
    return OK;
}

status_code ir_update_front(Input_reader* ir)
{
    if (ir == NULL)
    {
        return NULL_ARG;
    }
    
    for (size_t i = ir->front; i < ir->size; ++i)
    {
        ir->data[i - ir->front] = ir->data[i];
    }
    ir->size -= ir->front;
    ir->front = 0;
    
    return OK;
}

status_code ir_insert_req(Input_reader* ir, request* req)
{
    if (ir == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    for (size_t i = 0; i < ir->size; ++i)
    {
        if (ir->data[i]->id == req->id)
        {
            return FILE_KEY_DUPLICATE;
        }
    }
    
    if (ir->size == ir->cap)
    {
        request** data_tmp = realloc(ir->data, ir->cap * 2);
        if (data_tmp == NULL)
        {
            return BAD_ALLOC;
        }
        ir->data = data_tmp;
        ir->cap *= 2;
    }
    
    ir->data[ir->size++] = req;
    
    return OK;
}

status_code ir_read_file(Input_reader* ir, const char* path)
{
    if (ir == NULL || path == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    size_t new_data_begin;
    request* req = NULL;
    FILE* file = NULL;
    
    if ((file = fopen(path, "r")) == NULL)
    {
        code = FILE_OPENING_ERROR;
    }
    
    code = code ? code : ir_update_front(ir);
    new_data_begin = ir->size;
    
    while (!code)
    {
        code = code ? code : ir_read_file_line(file, &req);
        code = code ? code : ir_insert_req(ir, req);
        req = code ? req : NULL;
    }
    
    fclose(file);
    if (code == FILE_END)
    {
        code = OK;
    }
    
    if (code)
    {
        for (size_t i = new_data_begin; i < ir->size; ++i)
        {
            request_destruct(ir->data[i]);
            free(ir->data[i]);
        }
        free(req);
        ir->size = new_data_begin;
        return code;
    }
    
    qsort(ir->data + ir->front, ir->size - ir->front, sizeof(request*), request_ptr_compare_by_time);
    
    return OK;
}

status_code ir_get_up_to(Input_reader* ir, const char time[21], size_t* req_cnt, request*** reqs)
{
    if (ir == NULL)
    {
        return NULL_ARG;
    }
    
    *req_cnt = 0;
    size_t back = ir->front;
    
    while (back < ir->size && strcmp(ir->data[back]->time, time) <= 0)
    {
        ++back;
    }
    
    *req_cnt = back - ir->front;
    *reqs = (request**) malloc(sizeof(request*) * *req_cnt);
    if (*reqs == NULL)
    {
        return BAD_ALLOC;
    }
    
    for (size_t i = ir->front; i < back; ++i)
    {
        (*reqs)[i] = ir->data[i];
    }
    
    ir->front = back;
    
    return OK;
}
