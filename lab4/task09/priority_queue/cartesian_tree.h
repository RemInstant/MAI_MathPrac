#ifndef TREAP_H
#define TREAP_H

#include "../utility.h"

typedef struct treap_node
{
    request* req;
    unsigned priority;
    struct Treap_node* left;
    struct Treap_node* right;
} treap_node;

typedef struct Cartesian_tree
{
    treap_node* root;
    size_t size;
    int (*compare)(const request*, const request*);
} Cartesian_tree, Treap;


status_code treap_set_null(Treap* treap);
status_code treap_construct(Treap* treap, int (*compare)(const request*, const request*));
status_code treap_copy(Treap* treap_dest, const Treap* treap_src);
status_code treap_destruct(Treap* treap);

status_code treap_meld(Treap* treap_res, Treap* treap_l, Treap* treap_r);
status_code treap_copy_meld(Treap* treap_res, const Treap* treap_l, const Treap* treap_r);

status_code treap_size(const Treap* treap, size_t* size);
status_code treap_top(const Treap* treap, request** req);
status_code treap_pop(Treap* treap, request** req);
status_code treap_insert(Treap* treap, request* req);

status_code Treap_init(Treap* t);

status_code Treap_get_max(Treap t, request** res);
status_code Treap_del_max(Treap* t);



status_code Treap_merge(Treap* res, Treap t1, Treap t2);
status_code Treap_merge_no_destruction(Treap* res, Treap t1, Treap t2);

status_code Treap_insert(Treap* t, request* key);
status_code Treap_erase(Treap* t, request* key);

void Treap_print(FILE* stream, const Treap t);

void Treap_destroy(Treap t);

#endif