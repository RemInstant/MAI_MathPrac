#include "skew_heap.h"
//include utility stuff

void destroy_skew_node_rec(Skew_node* node)
{
    if(node == NULL)
        return;
    if(node->right != NULL)
        destroy_skew_node_rec(node->right);
    if(node->left != NULL)
        destroy_skew_node_rec(node->left);
    destroy_request(node->req);
    free(node);
    node = NULL;
}

status_code skw_heap_set_null(skw_heap* skw)
{
    if(skw == NULL)
        return NULL_ARG;
    skw->head = NULL;
    skw->size = 0;
    skw->compare = NULL;
    return OK;
}

status_code skw_heap_destruct(skw_heap* skw)
{
    if(skw == NULL)
        return OK;
    destroy_skew_node_rec(skw->head);
    skw_heap_set_null(skw);
    return OK;
}

Skew_node* merge(Skew_node* a, Skew_node* b, int (*compare)(const request*, const request*)) {
    if (a == NULL)
        return b;
    if (b == NULL)
        return a;
    
    //свап
    if (compare(a->req, b->req) > 0)
    {
        Skew_node* temp = a;
        a = b;
        b = temp;
    }
    {
        Skew_node* temp = a->left;
        a->left = a->right;
        a->right = temp;
    }
    a->left = merge(b, a->left, compare);

    return a;
}

status_code get_copy_rec(Skew_node** result, const Skew_node* src)
{
    if(src == NULL)
    {
        *result = NULL;
        return OK;
    }
    if(result == NULL)
    {
        return NULL_ARG;
    }
    *result = NULL;
    Skew_node* node = (Skew_node*) malloc(sizeof(Skew_node));
    if(node == NULL)
        return BAD_ALLOC;
    node->req = (request*) malloc(sizeof(request));
    if(node->req == NULL)
    {
        free(node);
        return BAD_ALLOC;
    }
    status_code code = request_copy(node->req, src->req);
    if(code)
    {
        free(node);
        return BAD_ALLOC;
    }
    node->right = NULL;
    node->left = NULL;
    code = get_copy_rec(&node->right, src->right);
    if(code)
    {
        destroy_skew_node_rec(node);
        return code;
    }
    code = get_copy_rec(&node->left, src->left);
    if(code)
    {
        destroy_skew_node_rec(node);
        return code;
    }
    *result = node;
    return OK;
}

status_code skw_heap_construct(skw_heap* skw, int (*compare)(const request*, const request*))
{
    status_code code = skw_heap_set_null(skw);
    if(code)
        return code;
    skw->compare = compare;
    return OK;
}

status_code skw_heap_copy(skw_heap* skw_dest, const skw_heap* skw_src)
{
    status_code code = OK;
    if(skw_dest == NULL || skw_src == NULL)
        return NULL_ARG;
    skw_dest->compare = skw_src->compare;
    skw_dest->size = skw_src->size;
    code = get_copy_rec(&skw_dest->head, skw_src->head);
    if(code)
    {
        skw_heap_destruct(skw_dest);
        return code;
    }
    return OK;
}

status_code skw_heap_meld(skw_heap* skw_res, skw_heap* skw_l, skw_heap* skw_r)
{
    if(skw_res == NULL || skw_l == NULL || skw_r == NULL)
        return NULL_ARG;
    Skew_node* res = merge(skw_r->head, skw_l->head, skw_l->compare);
    skw_res->head = res;
    skw_res->size = skw_l->size + skw_r->size;
    skw_res->compare = skw_l->compare;
    return OK;
}

status_code skw_heap_copy_meld(skw_heap* skw_res, const skw_heap* skw_l, const skw_heap* skw_r)
{
    if (skw_res == NULL)
    {
        return NULL_ARG;
    }
    
    if (skw_res == skw_l || skw_res == skw_l)
    {
        return INVALID_INPUT;
    }
    
    status_code code = OK;
    skw_heap skw_lc, skw_rc;
        
    skw_heap_set_null(&skw_lc);
    skw_heap_set_null(&skw_rc);
    skw_heap_set_null(skw_res);
    
    code = code ? code : skw_heap_copy(&skw_lc, skw_l);
    code = code ? code : skw_heap_copy(&skw_rc, skw_r);
    code = code ? code : skw_heap_meld(skw_res, &skw_lc, &skw_rc);
    
    if (code)
    {
        skw_heap_destruct(skw_res);
        skw_heap_destruct(&skw_lc);
        skw_heap_destruct(&skw_rc);
        return code;
    }
    return OK;
}

status_code skw_heap_size(const skw_heap* skw, size_t* size)
{
    if(skw == NULL)
        return NULL_ARG;
    (*size) = skw->size;
    return OK;
}

status_code skw_heap_top(const skw_heap* skw, request** req)
{
    if(skw == NULL)
        return NULL_ARG;
    *req = skw->head->req;
    return OK;
}

status_code skw_heap_pop(skw_heap* skw, request** req)
{
    if(skw == NULL || req == NULL || *req == NULL)
        return NULL_ARG;
    *req = skw->head->req;
    Skew_node* old_node = skw->head;
    skw->head = merge(skw->head->right, skw->head->left, skw->compare);
    old_node->left = NULL;
    old_node->right = NULL;
    old_node->req = NULL;
    skw->size = skw->size - 1;
    return OK;
}

status_code skw_heap_insert(skw_heap* skw, request* req)
{
    if(skw == NULL || req == NULL)
        return NULL_ARG;
    skw_heap new_h;
    Skew_node* new_node = (Skew_node*) malloc(sizeof(Skew_node));
    if(new_node == NULL)
    {
        return BAD_ALLOC;
    }
    status_code code = OK;

    new_node->req = req;
    new_node->right = NULL;
    new_node->left = NULL;
    
    new_h.compare = skw->compare;
    new_h.head = new_node;
    new_h.size = 1;
    code = skw_heap_meld(skw, skw, &new_h);
    return code;
}
