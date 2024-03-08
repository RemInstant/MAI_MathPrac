#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "utility.h"

int rand_comp(const void* lhs, const void* rhs)
{
    return rand() & 1;
}

int main(int argc, char** argv)
{
    
    srand(time(NULL));
    
    size_t req_cnt = rand_32() % (1 << 10);
    
    unsigned* req_ids = (unsigned*) malloc(req_cnt * sizeof(unsigned));
    if (req_ids == NULL)
    {
        return BAD_ALLOC;
    }
    
    for (size_t i = 0; i < req_cnt; ++i)
    {
        req_ids[i] = i + 1;
    }
    
    qsort(req_ids, req_cnt, sizeof(unsigned), rand_comp);
    
    for (size_t i = 0; i < req_cnt; ++i)
    {
        printf("%u\n", req_ids[i]);
    }
    
}