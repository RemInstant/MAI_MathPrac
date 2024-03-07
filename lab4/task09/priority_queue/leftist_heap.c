#include "leftist_heap.h"

void destroy_leftist_node_rec(Leftist_node* node)
{
    if(node == NULL)
        return;
    if(node->right != NULL)
        destroy_leftist_node_rec(node->right);
    if(node->left != NULL)
        destroy_leftist_node_rec(node->left);
    destroy_request(node->req);
    free(node);
    node = NULL;
}

status_code lft_heap_set_null(lft_heap* lft)
{
    if(lft == NULL)
        return NULL_ARG;
    lft->head = NULL;
    lft->size = 0;
    lft->compare = NULL;
    return OK;
}

status_code lft_heap_destruct(lft_heap* lft)
{
    if(lft == NULL)
        return OK;
    destroy_leftist_node_rec(lft->head);
    lft_heap_set_null(lft);
    return OK;
}

Leftist_node* merge(Leftist_node* a, Leftist_node* b, int (*compare)(const request*, const request*)) {
    if (a == NULL)
        return b;
    if (b == NULL)
        return a;
    
    //свап
    if (compare(a->req, b->req) > 0)
    {
        Leftist_node* temp = a;
        a = b;
        b = temp;
    }
    
    a->right = merge(a->right, b, compare);
    if (a->right != NULL && (a->left == NULL || a->left->distance < a->right->distance))
    {
        Leftist_node* temp = a->left;
        a->left = a->right;
        a->right = temp;
    }
    if (a->right == NULL)
        a->distance = 0;
    else
        a->distance = a->right->distance + 1;

    return a;
}

status_code get_copy_rec(Leftist_node** result, const Leftist_node* src)
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
    Leftist_node* node = (Leftist_node*) malloc(sizeof(Leftist_node));
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
    node->distance = src->distance;
    node->right = NULL;
    node->left = NULL;
    code = get_copy_rec(&node->right, src->right);
    if(code)
    {
        destroy_leftist_node_rec(node);
        return code;
    }
    code = get_copy_rec(&node->left, src->left);
    if(code)
    {
        destroy_leftist_node_rec(node);
        return code;
    }
    *result = node;
    return OK;
}

status_code lft_heap_construct(lft_heap* lft, int (*compare)(const request*, const request*))
{
    status_code code = lft_heap_set_null(lft);
    if(code)
        return code;
    lft->compare = compare;
    return OK;
}

status_code lft_heap_copy(lft_heap* lft_dest, const lft_heap* lft_src)
{
    status_code code = OK;
    if(lft_dest == NULL || lft_src == NULL)
        return NULL_ARG;
    lft_dest->compare = lft_src->compare;
    lft_dest->size = lft_src->size;
    code = get_copy_rec(&lft_dest->head, lft_src->head);
    if(code)
    {
        lft_heap_destruct(lft_dest);
        return code;
    }
    return OK;
}

status_code lft_heap_meld(lft_heap* lft_res, lft_heap* lft_l, lft_heap* lft_r)
{
    if(lft_res == NULL || lft_l == NULL || lft_r == NULL)
        return NULL_ARG;
    Leftist_node* res = merge(lft_r->head, lft_l->head, lft_l->compare);
    lft_res->head = res;
    lft_res->size = lft_l->size + lft_r->size;
    lft_res->compare = lft_l->compare;
    return OK;
}

status_code lft_heap_copy_meld(lft_heap* lft_res, const lft_heap* lft_l, const lft_heap* lft_r)
{
    if (lft_res == NULL)
    {
        return NULL_ARG;
    }
    
    if (lft_res == lft_l || lft_res == lft_l)
    {
        return INVALID_INPUT;
    }
    
    status_code code = OK;
    lft_heap lft_lc, lft_rc;
        
    lft_heap_set_null(&lft_lc);
    lft_heap_set_null(&lft_rc);
    lft_heap_set_null(lft_res);
    
    code = code ? code : lft_heap_copy(&lft_lc, lft_l);
    code = code ? code : lft_heap_copy(&lft_rc, lft_r);
    code = code ? code : lft_heap_meld(lft_res, &lft_lc, &lft_rc);
    
    if (code)
    {
        lft_heap_destruct(lft_res);
        lft_heap_destruct(&lft_lc);
        lft_heap_destruct(&lft_rc);
        return code;
    }
    return OK;
}

status_code lft_heap_size(const lft_heap* lft, size_t* size)
{
    if(lft == NULL)
        return NULL_ARG;
    (*size) = lft->size;
    return OK;
}

status_code lft_heap_top(const lft_heap* lft, request** req)
{
    if(lft == NULL)
        return NULL_ARG;
    *req = lft->head->req;
    return OK;
}

status_code lft_heap_pop(lft_heap* lft, request** req)
{
    if(lft == NULL || req == NULL || *req == NULL)
        return NULL_ARG;
    *req = lft->head->req;
    Leftist_node* old_node = lft->head;
    lft->head = merge(lft->head->right, lft->head->left, lft->compare);
    old_node->left = NULL;
    old_node->right = NULL;
    old_node->req = NULL;
    lft->size = lft->size - 1;
    return OK;
}

status_code lft_heap_insert(lft_heap* lft, request* req)
{
    if(lft == NULL || req == NULL)
        return NULL_ARG;
    lft_heap new_h;
    Leftist_node* new_node = (Leftist_node*) malloc(sizeof(Leftist_node));
    if(new_node == NULL)
    {
        return BAD_ALLOC;
    }
    status_code code = OK;

    new_node->distance = 0;
    new_node->req = req;
    new_node->right = NULL;
    new_node->left = NULL;
    
    new_h.compare = lft->compare;
    new_h.head = new_node;
    new_h.size = 1;
    code = lft_heap_meld(lft, lft, &new_h);
    return code;
}
