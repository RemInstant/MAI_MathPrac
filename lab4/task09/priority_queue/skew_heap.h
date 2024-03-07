#ifndef SKEW_HEAP
#define SKEW_HEAP

typedef struct Skew_node
{
    request* req;
    struct Skew_node* left;
    struct Skew_node* right;
} Skew_node, skw_node;

typedef struct Skew_heap
{
    Skew_node* head;
    unsigned int size;
    int (*compare)(const request*, const request*);
} Skew_heap, skw_heap;

status_code skw_heap_set_null(skw_heap* skw);

status_code skw_heap_destruct(skw_heap* skw);

status_code get_copy_rec(Skew_node** result, const Skew_node* src);

status_code skw_heap_construct(skw_heap* skw, int (*compare)(const request*, const request*));

status_code skw_heap_copy(skw_heap* skw_dest, const skw_heap* skw_src);

status_code skw_heap_meld(skw_heap* skw_res, skw_heap* skw_l, skw_heap* skw_r);

status_code skw_heap_copy_meld(skw_heap* skw_res, const skw_heap* skw_l, const skw_heap* skw_r);

status_code skw_heap_size(const skw_heap* skw, size_t* size);

status_code skw_heap_top(const skw_heap* skw, request** req);

status_code skw_heap_pop(skw_heap* skw, request** req);

status_code skw_heap_insert(skw_heap* skw, request* req);

#endif