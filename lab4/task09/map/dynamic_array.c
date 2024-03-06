#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_array.h"

int arr_comparator (const void* e_1, const void* e_2) 
{
    arr_elem* e1 = *(arr_elem**) e_1;
    arr_elem* e2 = *(arr_elem**) e_2;

    return strcmp(e1->key, e2->key);
}


status_code arr_elem_construct(arr_elem** elem, const char* key, Department* dep)
{
    if (key == NULL || dep == NULL)
    {
        return INVALID_ARG;
    }

    *elem = (arr_elem*)malloc(sizeof(arr_elem));
    if (*elem == NULL) 
    {
        return MEM_NOT_ALLOC;
    }

    (*elem)->key = (char*)malloc(sizeof(char) * (strlen(key) + 1));
    if ((*elem)->key == NULL) 
    {
        return MEM_NOT_ALLOC;
    }

    strcpy((*elem)->key, key);
    (*elem)->dep = dep;

    return OK;
}

status_code arr_elem_destruct(arr_elem* elem)
{
    //department_destruct(elem->dep);
    free(elem->key);
    free(elem);

    return OK;
}


status_code arr_construct (Array* arr) 
{
    arr->size = 0;
    arr->capacity = 8;
    arr->sorted = 1;

    arr->elems = (arr_elem**)malloc(sizeof(arr_elem*) * arr->capacity);
    if (arr->elems == NULL) 
    {
        return MEM_NOT_ALLOC;
    }

    return OK;
}

status_code arr_destruct (Array* arr)
{
    for (int i = 0; i < arr->size; ++i)
    {
        arr_elem_destruct(arr->elems[i]);
    }

    free(arr->elems);
    free(arr);
}

status_code arr_set_null (Array* arr)
{
    if (arr == NULL)
    {
        return INVALID_ARG;
    }

    arr->elems = NULL;
    arr->size = 0;
    arr->capacity = 0;
    arr->sorted = 0;

    return OK;
}

status_code arr_insert (Array* arr, const char* key, Department* dep) 
{
    arr_elem* elem;
    status_code status = arr_elem_construct(&elem, key, dep);
    if (status != OK)
    {
        return status;
    }   

    if (arr->size >= arr->capacity) 
    {
        arr->capacity *= 2;
        arr_elem** tmp;
        
        tmp = realloc(arr->elems, sizeof(arr_elem*) * arr->capacity);
        if (tmp == NULL) 
        {
            free(arr->elems);
            return MEM_NOT_ALLOC;
        }
        arr->elems = tmp;
    }
    arr->elems[arr->size++] = elem;

    arr->sorted = 0;

    return OK;
}

status_code arr_print (Array* arr)
{
    for (int i = 0; i < arr->size; i++)
    {
        printf("%s\n", arr->elems[i]->key);
    }

    return OK;
}

status_code arr_dich_search (Array* arr, const char* key, int* res)
{
    if (arr->size == 0)
    {
        return INVALID_ARG;
    }

    if (!arr->sorted)
    {
        qsort(arr->elems, arr->size, sizeof(arr_elem*), arr_comparator);
        arr->sorted = 1;
    }
    
    int a = 0, b = arr->size - 1, c, cmp;
    c = b / 2;
    while (b > a)
    {
        cmp = strcmp(key, arr->elems[c]->key);
        if (cmp == 0)
        {
            *res = c;
            return OK;
        }
        else if (cmp > 0)
        {
            a = c + 1;
        }
        else
        {
            b = c;
        }
        c = (a + b) / 2;
    }
    if (strcmp(key, arr->elems[c]->key) == 0)
    {
        *res = c;
        return OK;
    }

    return INVALID_ARG;
}

status_code arr_contains(Array* arr, const char* key, int* is_contained)
{
    if (arr == NULL || key == NULL || is_contained == NULL)
    {
        return INVALID_ARG;
    }

    int ind;
    if (arr_dich_search(arr, key, &ind) == OK)
    {
        *is_contained = 1;
    }
    else
    {
        *is_contained = 0;
    }

    return OK;
}

status_code arr_get(Array* arr, const char* key, Department** dep)
{
    if (arr == NULL || key == NULL || dep == NULL)
    {
        return INVALID_ARG;
    }

    int ind;
    if (arr_dich_search(arr, key, &ind) == INVALID_ARG)
    {
        return INVALID_ARG;
    }

    *dep = arr->elems[ind]->dep;

    return OK;
}

status_code arr_erase(Array* arr, const char* key)
{
    if (arr == NULL || key == NULL)
    {
        return INVALID_ARG;
    }

    int ind;
    if (arr_dich_search(arr, key, &ind) == INVALID_ARG)
    {
        return INVALID_ARG;
    }

    status_code status = OK;

    for (int i = ind; i < arr->size - 1; ++i)
    {
        arr_elem_destruct(arr->elems[i]);

        status = arr_elem_construct(&arr->elems[i], arr->elems[i + 1]->key, arr->elems[i + 1]->dep);
        if (status != OK)
        {
            return status;
        }
    }

    arr_elem_destruct(arr->elems[arr->size - 1]);
    arr->size--;

    return OK;
}