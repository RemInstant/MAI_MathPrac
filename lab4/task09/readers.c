#include <stdlib.h>
#include <string.h>

#include "readers.h"

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


status_code parse_prior_queue_name(const char* name, pq_base* base)
{
    if (name == NULL || base == NULL)
    {
        return NULL_ARG;
    }
    
    if (!strcmp(name, "BinaryHeap"))
    {
        *base = PQB_BINARY;
    }
    else if (!strcmp(name, "BinomialHeap"))
    {
        *base = PQB_BINOM;
    }
    else if (!strcmp(name, "FibonacciHeap"))
    {
        *base = PQB_FIB;
    }
    else if (!strcmp(name, "SkewHeap"))
    {
        *base = PQB_SKEW;
    }
    else if (!strcmp(name, "LeftistHeap"))
    {
        *base = PQB_LEFTIST;
    }
    else if (!strcmp(name, "Treap"))
    {
        *base = PQB_TREAP;
    }
    else
    {
        return INVALID_INPUT;
    }
    
    return OK;
}

status_code parse_map_name(const char* name, map_base* base)
{
    if (name == NULL || base == NULL)
    {
        return NULL_ARG;
    }
    
    if (!strcmp(name, "HashSet"))
    {
        *base = MB_HASHSET;
    }
    else if (!strcmp(name, "DynamicArray"))
    {
        *base = MB_ARR;
    }
    else if (!strcmp(name, "BinarySearchTree"))
    {
        *base = MB_BST;
    }
    else if (!strcmp(name, "Trie"))
    {
        *base = MB_TRIE;
    }
    else
    {
        return INVALID_INPUT;
    }
    
    return OK;
}


status_code setup_config(const char* path, Map* dep_map, size_t* dep_cnt, char*** dep_names,
                                char st_time[21], char end_time[21], double eps)
{
    if (path == NULL || dep_map == NULL || dep_cnt == NULL || dep_names == NULL || st_time == NULL || end_time == NULL)
    {
        return NULL_ARG;
    }
    
    FILE* file = fopen(path, "r");
    if (file == NULL)
    {
        return FILE_OPENING_ERROR;
    }
    
    status_code code = OK;
    Map dep_map_tmp;
    char* raw_queue_base = NULL;
    char* raw_mp_base = NULL;
    char* raw_st_time = NULL;
    char* raw_end_time = NULL;
    char* raw_min_handle_time = NULL;
    char* raw_max_handle_time = NULL;
    char* raw_dep_cnt = NULL;
    char* raw_dep_names = NULL;
    char* raw_staff_cnts = NULL;
    char* raw_overload_coef = NULL;
    
    pq_base queue_base;
    map_base mp_base;
    ull min_handle_time = 0;
    ull max_handle_time = 0;
    ull dep_cnt_tmp = 0;
    double overload_coef = 0;
    
    // READ CONFIG
    code = code ? code : fread_line(file, &raw_queue_base, 0);
    code = code ? code : fread_line(file, &raw_mp_base, 0);
    code = code ? code : parse_prior_queue_name(raw_queue_base, &queue_base);
    code = code ? code : parse_map_name(raw_mp_base, &mp_base);
    free(raw_queue_base);
    free(raw_mp_base);
    
    code = code ? code : fread_line(file, &raw_st_time, 0);
    code = code ? code : fread_line(file, &raw_end_time, 0);
    code = code ? code : iso_time_validate(raw_st_time);
    code = code ? code : iso_time_validate(raw_end_time);
    if (!code)
    {
        strcpy(st_time, raw_st_time);
        strcpy(end_time, raw_end_time);
    }
    free(raw_st_time);
    free(raw_end_time);
    
    code = code ? code : fread_line(file, &raw_min_handle_time, 0);
    code = code ? code : fread_line(file, &raw_max_handle_time, 0);
    code = code ? code : parse_ullong(raw_min_handle_time, 10, &min_handle_time);
    code = code ? code : parse_ullong(raw_max_handle_time, 10, &max_handle_time);
    free(raw_min_handle_time);
    free(raw_max_handle_time);
    
    code = code ? code : fread_line(file, &raw_dep_cnt, 0);
    code = code ? code : parse_ullong(raw_dep_cnt, 10, &dep_cnt_tmp);
    code = code ? code : fread_line(file, &raw_dep_names, 0);
    code = code ? code : fread_line(file, &raw_staff_cnts, 0);
    code = code ? code : fread_line(file, &raw_overload_coef, 0);
    code = code ? code : parse_double(raw_overload_coef, &overload_coef);
    free(raw_dep_cnt);
    free(raw_overload_coef);
    
    code = code ? code : (feof(file) ? OK : FILE_INVALID_CONFIG);
    fclose(file);
    
    if (min_handle_time > max_handle_time || overload_coef - CONFIG_MIN_OVERLOAD_COEF < -eps ||
            dep_cnt_tmp < CONFIG_MIN_DEP_CNT || dep_cnt_tmp > CONFIG_MAX_DEP_CNT)
    {
        code = code ? code : FILE_INVALID_CONFIG;
    }
    
    if (code == INVALID_INPUT)
    {
        code = FILE_INVALID_CONFIG;
    }
    
    // CONSTRUCT DEPARTMENT MAP
    map_set_null(&dep_map_tmp);
    code = code ? code : map_construct(&dep_map_tmp, mp_base, calc_default_str_hash);
    
    char** dep_names_tmp = (char**) calloc(dep_cnt_tmp, sizeof(char*));
    if (dep_names_tmp == NULL)
    {
        code = code ? code : BAD_ALLOC;
    }
    
    const char* ch_ptr1 = raw_dep_names;
    const char* ch_ptr2 = raw_staff_cnts;
    
    for (size_t iter = 0; iter < dep_cnt_tmp && !code; ++iter)
    {
        char* dep_name = NULL;
        char* raw_staff_cnt = NULL;
        ull staff_cnt = 0;
        
        code = code ? code : sread_until(ch_ptr1, " ", 0, &ch_ptr1, &dep_name);
        code = code ? code : sread_until(ch_ptr2, " ", 0, &ch_ptr2, &raw_staff_cnt);
        code = code ? code : parse_ullong(raw_staff_cnt, 10, &staff_cnt);
        dep_names_tmp[iter] = code ? NULL : dep_name;
        
        if (staff_cnt < CONFIG_MIN_STAFF_CNT || staff_cnt > CONFIG_MAX_STAFF_CNT)
        {
            code = code ? code : FILE_INVALID_CONFIG;
        }
        
        char check_char = iter + 1 < dep_cnt_tmp ? ' ' : '\0';
        code = code ? code : (*(ch_ptr1++) == check_char ? OK : FILE_INVALID_CONFIG);
        code = code ? code : (*(ch_ptr2++) == check_char ? OK : FILE_INVALID_CONFIG);
        
        Department* dep = (Department*) (malloc(sizeof(Department)));;
        if (dep == NULL)
        {
            code = code ? code : BAD_ALLOC;
        }
        
        code = code ? code : department_construct(dep, dep_name, staff_cnt, queue_base, overload_coef, eps,
                                                        min_handle_time, max_handle_time, compare_request);
        code = code ? code : map_insert(&dep_map_tmp, dep_name, dep);
        code = code == BAD_ACCESS ? FILE_INVALID_CONFIG : code;
        
        free(raw_staff_cnt);
        
        if (code)
        {
            free(dep);
        }
    }
    
    free(raw_dep_names);
    free(raw_staff_cnts);
    
    if (code)
    {
        for (size_t i = 0; i < dep_cnt_tmp; ++i)
        {
            free(dep_names_tmp[i]);
        }
        free(dep_names_tmp);
        map_destruct(&dep_map_tmp);
        return code;
    }
    
    *dep_map = dep_map_tmp;
    *dep_cnt = dep_cnt_tmp;
    *dep_names = dep_names_tmp;
    
    return OK;
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
    ir->max_prior = 0;
    ir->dep_cnt = 0;
    ir->dep_names = NULL;
    memset(ir->st_time, 0, 21 * sizeof(char));
    
    return OK;
}

status_code ir_construct(Input_reader* ir, const char st_time[21], size_t max_prior, size_t dep_cnt, const char** dep_names)
{
    if (ir == NULL || st_time == NULL || dep_names == NULL)
    {
        return NULL_ARG;
    }
    
    ir->size = 0;
    ir->cap = 2;
    ir->data = (request**) malloc(sizeof(request*) * 2);
    ir->front = 0;
    
    if (ir->data == NULL)
    {
        ir_set_null(ir);
        return BAD_ALLOC;
    }
    
    strcpy(ir->st_time, st_time);
    ir->max_prior = max_prior;
    ir->dep_cnt = dep_cnt;
    ir->dep_names = dep_names;
    
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
status_code ir_read_file_line(Input_reader* ir, FILE* file, request** req)
{
    if (ir == NULL || file == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    char* time = NULL;
    char* req_id_str = NULL;
    char* prior_str = NULL;
    char* dep_name = NULL;
    char* txt = NULL;
    char check_ch;
    ull req_id;
    ull prior;
    request* req_tmp = NULL;
    
    code = code ? code : fread_word(file, &time, 1);
    if (code == FILE_END) return code;
    
    code = code ? code : fread_word(file, &req_id_str, 1);
    code = code ? code : fread_word(file, &prior_str, 1);
    code = code ? code : fread_word(file, &dep_name, 1);
    
    code = code ? code : fread_char(file, &check_ch, 1);
    code = code ? code : fread_line(file, &txt, 0);
    
    code = code ? code : iso_time_validate(time);
    code = code ? code : (strcmp(time, ir->st_time) >= 0 ? OK : FILE_INVALID_CONTENT);
    code = code ? code : parse_ullong(req_id_str, 10, &req_id);
    code = code ? code : parse_ullong(prior_str, 10, &prior);
    code = code ? code : (check_ch == '"' ? OK : FILE_INVALID_CONTENT);
    code = code ? code : (txt[strlen(txt) - 1] == '"' ? OK : FILE_INVALID_CONTENT);
    
    code = code ? code : (prior <= ir->max_prior ? OK : FILE_INVALID_CONTENT);
    
    int is_dep_exist = 0;
    for (size_t i = 0; !code && i < ir->dep_cnt; ++i)
    {
        if (!strcmp(ir->dep_names[i], dep_name))
        {
            is_dep_exist = 1;
            break;
        }
    }
    code = code ? code : (is_dep_exist ? OK : FILE_INVALID_CONTENT);
    
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
            req_tmp->dep_id = dep_name;
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
        free(dep_name);
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
    
    request** tmp = (request**) realloc(ir->data, sizeof(request*) * ir->size);
    if (tmp == NULL)
    {
        return BAD_ALLOC;
    }
    
    ir->cap = ir->size;
    ir->data = tmp;
    
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
        request** data_tmp = (request**) realloc(ir->data, sizeof(request*) * ir->cap * 2);
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
    size_t new_data_begin = ir->size;
    request* req = NULL;
    FILE* file = NULL;
    
    if ((file = fopen(path, "r")) == NULL)
    {
        code = FILE_OPENING_ERROR;
    }
    
    while (!code)
    {
        code = code ? code : ir_read_file_line(ir, file, &req);
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
    
    qsort(ir->data, ir->size, sizeof(request*), request_ptr_compare_by_time);
    
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
        (*reqs)[i - ir->front] = ir->data[i];
    }
    
    ir->front = back;
    
    return OK;
}
