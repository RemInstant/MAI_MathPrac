#include "binomial_heap.h"
#include <stdlib.h>
#include <string.h>

status_code bmh_node_copy(bmh_node** node_dest, const bmh_node* node_src)
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
	bmh_node* tmp_node = (bmh_node*) malloc(sizeof(bmh_node));
	if (tmp_node == NULL)
	{
		return BAD_ALLOC;
	}
	
	tmp_node->value = (char*) malloc(sizeof(char) * (strlen(node_src->value) + 1));
	if (tmp_node->value == NULL)
	{
		free(tmp_node);
		return BAD_ALLOC;
	}
	
	strcpy(tmp_node->value, node_src->value);
	strcpy(tmp_node->key.time, node_src->key.time);
	tmp_node->key.prior = node_src->key.prior;
	tmp_node->son = NULL;
	tmp_node->brother = NULL;
	tmp_node->rank = node_src->rank;
	
	status_code son_code = bmh_node_copy(&tmp_node->son, node_src->son);
	if (son_code)
	{
		return son_code;
	}
	
	status_code brother_code = bmh_node_copy(&tmp_node->brother, node_src->brother);
	if (brother_code)
	{
		return brother_code;
	}
	
	*node_dest = tmp_node;
	
	return OK;
}

status_code bmh_node_destruct(bmh_node* node)
{
	if (node == NULL)
	{
		return OK;
	}
	bmh_node_destruct(node->son);
	bmh_node_destruct(node->brother);
	free(node->value);
	free(node);
	return OK;
}

status_code bm_heap_set_null(bm_heap* bmh)
{
	if (bmh == NULL)
	{
		return NULL_ARG;
	}
	bmh->head = NULL;
	bmh->compare = NULL;
	return OK;
}

status_code bm_heap_construct(bm_heap* bmh, int (*compare)(const pair_prior_time*, const pair_prior_time*))
{
	if (bmh == NULL)
	{
		return NULL_ARG;
	}
	bmh->head = NULL;
	bmh->compare = compare;
	return OK;
}

status_code bm_heap_copy(bm_heap* bmh_dest, const bm_heap* bmh_src)
{
	if (bmh_dest == NULL)
	{
		return NULL_ARG;
	}
	
	bmh_dest->compare = bmh_src->compare;
	status_code code = bmh_node_copy(&bmh_dest->head, bmh_src->head);
	if (code)
	{
		bm_heap_destruct(bmh_dest);
		return code;
	}
	return OK;
}

status_code bm_heap_destruct(bm_heap* bmh)
{
	if (bmh == NULL)
	{
		return OK; // or NULL_ARG???
	}
	status_code code = bmh_node_destruct(bmh->head);
	bmh->head = NULL;
	return code;
}

status_code bm_heap_meld(bm_heap* bmh_res, bm_heap* bmh_l, bm_heap* bmh_r)
{
	if (bmh_res == NULL || bmh_l == NULL || bmh_r == NULL)
	{
		return NULL_ARG;
	}
	if (bmh_l == bmh_r)
	{
		return INVALID_INPUT;
	}
	
	// STEP 1: GATHERING UP;
	bm_heap bmh_tmp;
	bmh_node* iter_l = bmh_l->head;
	bmh_node* iter_r = bmh_r->head;
	
	if (iter_l == NULL && iter_r == NULL)
	{
		bmh_res->head = NULL;
		bmh_res->compare = bmh_l->compare;
		return OK;
	}
	
	if (iter_l != NULL && (iter_r == NULL || iter_l->rank < iter_r->rank))
	{
		bmh_tmp.head = iter_l;
		iter_l = iter_l->brother;
	}
	else
	{
		bmh_tmp.head = iter_r;
		iter_r = iter_r->brother;
	}
	
	bmh_node* iter_res = bmh_tmp.head;
	while (iter_l != NULL && iter_r != NULL)
	{
		if (iter_l->rank < iter_r->rank)
		{
			iter_res->brother = iter_l;
			iter_l = iter_l->brother;
		}
		else
		{
			iter_res->brother = iter_r;
			iter_r = iter_r->brother;
		}
		iter_res = iter_res->brother;
	}
	
	while (iter_l != NULL)
	{
		iter_res->brother = iter_l;
		iter_l = iter_l->brother;
		iter_res = iter_res->brother;
	}
	
	while (iter_r != NULL)
	{
		iter_res->brother = iter_r;
		iter_r = iter_r->brother;
		iter_res = iter_res->brother;
	}
	
	// STEP 2: COMBINING
	
	bmh_node* prev_node = NULL;
	bmh_node* cur_node = bmh_tmp.head;
	bmh_node* nxt_node = bmh_tmp.head->brother;
	
	while (nxt_node != NULL)
	{
		bmh_node* tmp_node = nxt_node->brother;
		if (cur_node->rank == nxt_node->rank && (tmp_node == NULL || tmp_node->rank != nxt_node->rank))
		{
			if (bmh_l->compare(&cur_node->key, &nxt_node->key) != 1)
			{
				// cur > nxt -> "no swap"
				nxt_node->brother = cur_node->son;
				cur_node->son = nxt_node;
			}
			else
			{
				// nxt > cur -> "swap"
				cur_node->brother = nxt_node->son;
				nxt_node->son = cur_node;
				
				if (cur_node == bmh_tmp.head)
				{
					bmh_tmp.head = nxt_node;
				}
				cur_node = nxt_node;
			}
			if (prev_node != NULL)
			{
				prev_node->brother = cur_node;
			}
			cur_node->rank++;
			cur_node->brother = tmp_node;
			nxt_node = tmp_node;
		}
		else
		{
			prev_node = cur_node;
			cur_node = nxt_node;
			nxt_node = nxt_node->brother;
		}
	}
	
	bmh_res->head = bmh_tmp.head;
	bmh_res->compare = bmh_l->compare;
	if (bmh_l != bmh_res)
	{
		bmh_l->head = NULL;
	}
	if (bmh_r != bmh_res)
	{
		bmh_r->head = NULL;
	}
	return OK;
}

status_code bm_heap_copy_meld(bm_heap* bmh_res, const bm_heap* bmh_l, const bm_heap* bmh_r)
{
	if (bmh_res == NULL)
	{
		return NULL_ARG;
	}
	
	status_code code = OK;
	bm_heap bmh_lc, bmh_rc;
	bm_heap_set_null(&bmh_lc);
	bm_heap_set_null(&bmh_rc);
	bm_heap_set_null(bmh_res);
	
	code = code ? code : bm_heap_copy(&bmh_lc, bmh_l);
	code = code ? code : bm_heap_copy(&bmh_rc, bmh_r);
	code = code ? code : bm_heap_meld(bmh_res, &bmh_lc, &bmh_rc);
	
	if (code)
	{
		bm_heap_destruct(bmh_res);
		bm_heap_destruct(&bmh_lc);
		bm_heap_destruct(&bmh_rc);
		return code;
	}
	return OK;
}

status_code bm_heap_find(const bm_heap* bmh, bmh_node** prev, bmh_node** node)
{
	// bad O(logn) alg 
	if (node == NULL)
	{
		return NULL_ARG;
	}
	if (bmh->head == NULL)
	{
		*node = NULL;
		return OK;
	}
	
	bmh_node* prev_node = NULL;
	bmh_node* target_node = bmh->head;
	bmh_node* cur_node = bmh->head;
	while (cur_node->brother != NULL)
	{
		if (bmh->compare(&cur_node->brother->key, &target_node->key) == -1)
		{
			prev_node = cur_node;
			target_node = cur_node->brother;
		}
		cur_node = cur_node->brother;
	}
	
	if (prev != NULL)
	{
		*prev = prev_node;
	}
	*node = target_node;
	return OK;
}

status_code bm_heap_top(const bm_heap* bmh, char** value)
{
	if (value == NULL)
	{
		return NULL_ARG;
	}
	
	bmh_node* node = NULL;
	bm_heap_find(bmh, NULL, &node);
	if (node == NULL)
	{
		*value = NULL;
		return OK;
	}
	
	*value = (char*) malloc(sizeof(char) * (strlen(node->value) + 1));
	if (*value == NULL)
	{
		return BAD_ALLOC;
	}
	strcpy(*value, node->value);
	return OK;
}

status_code bm_heap_pop(bm_heap* bmh, char** value)
{
	if (bmh == NULL)
	{
		return NULL_ARG;
	}
	
	bmh_node* prev = NULL;
	bmh_node* node = NULL;
	bm_heap_find(bmh, &prev, &node);
	if (node == NULL)
	{
		if (value != NULL)
		{
			*value = NULL;
		}
		return OK;
	}
	
	if (value != NULL)
	{
		*value = node->value;
	}
	
	if (prev != NULL)
	{
		prev->brother = node->brother;
	}
	if (node == bmh->head)
	{
		bmh->head = node->brother;
	}
	
	bm_heap tmp_bmh, add_bmh;
	add_bmh.head = node->son;
	add_bmh.compare = bmh->compare;
	free(node);
	
	// reverse add bmh
	prev = NULL;
	node = add_bmh.head;
	while (node != NULL)
	{
		bmh_node* nxt = node->brother;
		node->brother = prev;
		prev = node;
		node = nxt;
	}
	add_bmh.head = prev;
	
	bm_heap_meld(&tmp_bmh, bmh, &add_bmh);
	bmh->head = tmp_bmh.head;
	return OK;
}

status_code bm_heap_insert(bm_heap* bmh, pair_prior_time key, char* value)
{
	if (bmh == NULL || value == NULL)
	{
		return NULL_ARG;
	}
	
	bmh_node* node = (bmh_node*) malloc(sizeof(bmh_node));
	if (node == NULL)
	{
		return BAD_ALLOC;
	}
	
	node->value = (char*) malloc(sizeof(char) * (strlen(value) + 1));
	if (node->value == NULL)
	{
		free(node);
		return BAD_ALLOC;
	}
	
	strcpy(node->value, value);
	node->key = key;
	node->son = NULL;
	node->brother = NULL;
	node->rank = 0;
	
	bm_heap bmh_tmp;
	bmh_tmp.head = node;
	bmh_tmp.compare = bmh->compare;
	bm_heap_meld(bmh, bmh, &bmh_tmp);
	return OK;
}