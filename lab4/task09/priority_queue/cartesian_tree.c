#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cartesian_tree.h"

/*
1. Добавление данных по ключу
2. Удаление данных с максимальным приоритетом по ключу
3. Получение данных с максимальным приоритетом по ключу
4. Слияние двух очередей без разрушения исходных
5. Слияние с разрушением
*/

int treap_node_construct(treap_node** nd, request* a)
{
    *nd = (treap_node*)malloc(sizeof(treap_node));
    if (*nd == NULL) {
        return BAD_ALLOC;
    }

    (*nd)->req = a;
    (*nd)->left = NULL;
    (*nd)->right = NULL;

    (*nd)->priority = rand();

    return OK;
}

status_code treap_node_copy(treap_node** node_dest, treap_node* node_src)
{
    if (node_dest == NULL)
    {
        return NULL_ARG;
    }
    
    if (node_src == NULL)
    {
        *node_dest = NULL;
        return OK;
    }
 
    *node_dest = NULL;
    treap_node* tmp_node = (treap_node*) malloc(sizeof(treap_node));
    if (tmp_node == NULL)
    {
        return BAD_ALLOC;
    }
    
    tmp_node->req = (request*) malloc(sizeof(request));
    if (tmp_node->req == NULL)
    {
        free(tmp_node);
        return BAD_ALLOC;
    }
    
    status_code code = request_copy(tmp_node->req, node_src->req);
    if (code)
    {
        free(tmp_node);
        return code;
    }
    
    tmp_node->priority = node_src->priority;
    tmp_node->left = NULL;
    tmp_node->right = NULL;
    
    code = code ? code : treap_node_copy(&tmp_node->left, node_src->left);
    code = code ? code : treap_node_copy(&tmp_node->right, node_src->right);
    if (code)
    {
        treap_node_destruct(tmp_node);
        return OK;
    }
    code;
    
    *node_dest = tmp_node;
    
    return OK;
}

void treap_node_destruct(treap_node* node)
{
    if (node == NULL)
    {
        return;
    }
    
    treap_node_destruct(node->left);
    treap_node_destruct(node->right);
    request_destruct(node->req);
    free(node);
    
    return;
}

treap_node* treap_node_merge(treap_node* t1, treap_node* t2)
{
    if (t1 == NULL && t1 == NULL)
    {
        return NULL;
    }
    
    if (t1 == NULL)
    {
        return t2;
    }
    if (t2 == NULL)
    {
        return t1;
    }

    if (t1->priority > t2->priority)
    {
        t1->right = treap_node_merge(t1->right, t2);
        return t1;
    }
    else
    {
        t2->left = treap_node_merge(t1, t2->left);
        return t2;
    }
}





status_code treap_set_null(Treap* treap)
{
    if (treap == NULL)
    {
        return NULL_ARG;
    }
    
    treap->root = NULL;
    treap->size = 0;
    treap->compare = NULL;
    
    return OK;
}

status_code treap_construct(Treap* treap, int (*compare)(const request*, const request*))
{
    if (treap == NULL || compare == NULL)
    {
        return NULL_ARG;
    }
    
    treap->root = NULL;
    treap->size = 0;
    treap->compare = compare;
    
    return OK;
}

status_code treap_copy(Treap* treap_dest, const Treap* treap_src)
{
    if (treap_dest == NULL || treap_src == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = treap_node_copy(&treap_dest->root, treap_src->root);
    if (code)
    {
        return code;
    }
    
    treap_dest->size = treap_src->size;
    treap_dest->compare = treap_src->size;
    
    return OK;
}

status_code treap_destruct(Treap* treap)
{
    if (treap == NULL)
    {
        return OK;
    }
    
    treap_node_destruct(treap->root);
    treap_set_null(treap);
    
    return OK;
}


status_code treap_meld(Treap* treap_res, Treap* treap_l, Treap* treap_r)
{
    if (treap_res == NULL || treap_l == NULL || treap_r == NULL)
    {
        return NULL_ARG;
    }
    
    Treap treap_tmp;
    
    treap_tmp.root = treap_node_merge(treap_l->root, treap_r->root);
    treap_tmp.size = treap_l->size + treap_r->size;
    treap_tmp.compare = treap_l->compare;
    
    treap_l->root = treap_r->root = NULL;
    treap_l->size = treap_r->size = 0;
    *treap_res = treap_tmp;
    
    return OK;
}

status_code treap_copy_meld(Treap* treap_res, const Treap* treap_l, const Treap* treap_r)
{
    if (treap_res == NULL || treap_l == NULL || treap_r == NULL)
    {
        return NULL_ARG;
    }
    
    if (treap_res == treap_l || treap_res == treap_l)
    {
        return INVALID_INPUT;
    }
    
    status_code code = OK;
    Treap treap_lc, treap_rc;
        
    treap_heap_set_null(&treap_lc);
    treap_heap_set_null(&treap_rc);
    treap_heap_set_null(treap_res);
    
    code = code ? code : treap_heap_copy(&treap_lc, treap_l);
    code = code ? code : treap_heap_copy(&treap_rc, treap_r);
    code = code ? code : treap_heap_meld(treap_res, &treap_lc, &treap_rc);
    
    treap_heap_destruct(&treap_lc);
    treap_heap_destruct(&treap_rc);
    
    if (code)
    {
        treap_heap_destruct(treap_res);
        return code;
    }
    
    return OK;
}



int Treap_split(treap_node* t, request* key, treap_node** t1, treap_node** t2) {
    if (key == NULL) {
        return NULL_ARG;
    }   

    if (t == NULL) {
        *t1 = NULL;
        *t2 = NULL;
        return OK;
    }

    int st = OK;
    if (compare_requests(t->key, key) == -1) {
        st = Treap_split(t->right, key, &(t->right), t2);
        if (st != OK) {
            return st;
        }
        *t1 = t;
    }
    else {
        st = Treap_split(t->left, key, t1, &(t->left));
        if (st != OK) {
            return st;
        }
        *t2 = t;
    }
    return OK;
}

int Treap_insert(Treap* t, request* key) {
    treap_node* new_nd = NULL;
    int st = OK;

    st = treap_node_init(&new_nd, key);
    if (st != OK) {
        free_request(key);
        return st;
    }

    treap_node* less, *greater;
    st = Treap_split(t->root, key, &less, &greater);
    if (st != OK) {
        treap_node_free(new_nd);
        return st;
    }

    t->root = Treap_merge_from_root(Treap_merge_from_root(less, new_nd), greater);

    return OK;
}

int Treap_erase(Treap* t, request* key) {
    treap_node *less, *equal, *greater;
    int st = OK;
    st = Treap_split(t->root, key, &less, &greater);
    if (st != OK) {
        return st;
    }

    key->id += 1;
    st = Treap_split(greater, key, &equal, &greater);
    if (st != OK) {
        return st;
    }
    
    t->root = Treap_merge_from_root(less, greater);
    key->id -= 1;

    treap_node_free(equal);

    return OK;
}

void Treap_print_from_root(FILE* stream, treap_node* root, int n) {
    if (root != NULL) {
        Treap_print_from_root(stream, root->right, n + 1);
        for (int i = 0; i < n; i++) fprintf(stream, "\t");
        //print_request(stream, root->key);
        printf("I:%d P:%d\n", root->key->id, root->priority);
        Treap_print_from_root(stream, root->left, n + 1);
    }
}

void Treap_print(FILE* stream, const Treap t) {
    if (t.root == NULL) {
        fprintf(stream, "The tree is empty!\n");
        return;
    }
    Treap_print_from_root(stream, t.root, 0);
}

void Treap_destroy(Treap t) {
    Treap_destroy_from_root(t.root);
}

void Treap_destroy_from_root(treap_node* root) {
    if (root == NULL) {
        return;
    }
    free_request(root->key);
    Treap_destroy_from_root(root->left);
    Treap_destroy_from_root(root->right);

    free(root);
}

// int Treap_get_max_from(Tre)

int Treap_get_max(Treap t, request** res) {
    treap_node* root = t.root;

    treap_node* prev = root;
    treap_node* tmp_node = root;

    while (tmp_node != NULL) {
        prev = tmp_node;
        tmp_node = tmp_node->right;
    }

    *res = prev->key;

    return OK;
}

int Treap_del_max(Treap* t) {
    if (t->root == NULL) {
        return OK;
    }

    treap_node* root = t->root;

    if (root->right == NULL) {
        t->root = root->left;
        treap_node_free(root);
        return OK;
    }

    treap_node* prev = root;
    treap_node* tmp_node = root->right;

    while (tmp_node->right != NULL) {
        prev = tmp_node;
        tmp_node = tmp_node->right;
    }

    treap_node_free(tmp_node);
    prev->right = NULL;

    return OK;
}