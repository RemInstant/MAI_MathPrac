#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "../utility.h"
#include "../department.h"

typedef struct arr_elem
{
    char* key;
    Department* dep;
} arr_elem;

typedef struct Array
{
    arr_elem** elems;
    size_t size;
    size_t capacity;
    int sorted;
} Array;


status_code arr_construct (Array* arr);
status_code arr_destruct (Array* arr);
status_code arr_set_null (Array* arr);

status_code arr_insert (Array* arr, const char* key, Department* dep);
status_code arr_print (Array* arr);
status_code arr_dich_search (Array* arr, const char* key, int* res);
status_code arr_contains(Array* arr, const char* key, int* is_contained);
status_code arr_get(Array* arr, const char* key, Department** dep);
status_code arr_erase(Array* arr, const char* key);

#endif // _ARRAY_H_