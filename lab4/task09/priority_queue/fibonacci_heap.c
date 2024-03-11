#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fibonacci_heap.h"

status_code fib_node_destruct(fib_node* node)
{
	if (node == NULL)
	{
		return OK;
	}

	if (node->left != NULL)
	{
		node->left->right = NULL;
	}
	
	fib_node_destruct(node->child);
	fib_node_destruct(node->right);

	request_destruct(node->req);
	free(node->req);
	free(node);

	return OK;
}

status_code fib_node_copy(fib_node** node_dest, const fib_node* node_src, const fib_node* barrier)
{
	if (node_dest == NULL)
	{
		return NULL_ARG;
	}
	
	if (node_src == NULL || node_src == barrier)
	{
		*node_dest = NULL;
		return OK;
	}
	
	*node_dest = (fib_node*) malloc(sizeof(fib_node));
	if (*node_dest == NULL)
	{
		return BAD_ALLOC;
	}
	
	(*node_dest)->rank = node_src->rank;
	(*node_dest)->req = (request*) malloc(sizeof(request));
	if ((*node_dest)->req == NULL)
	{
		free(*node_dest);
		return BAD_ALLOC;
	}
	
	status_code code = request_copy((*node_dest)->req, node_src->req);
	if (code)
	{
		free((*node_dest)->req);
		free(*node_dest);
		return code;
	}
	
	barrier = barrier == NULL ? node_src : barrier;
	
	code = code ? code : fib_node_copy(&(*node_dest)->child, node_src->child, NULL);
	code = code ? code : fib_node_copy(&(*node_dest)->right, node_src->right, barrier);
	
	if (code)
	{
		fib_node_destruct(*node_dest);
		return code;
	}
	
	if ((*node_dest)->right != NULL)
	{
		(*node_dest)->left = (*node_dest)->right->left;
		(*node_dest)->left->right = *node_dest;
		(*node_dest)->right->left = *node_dest;
	}
	else
	{
		(*node_dest)->left = (*node_dest);
	}
	
	return OK;
}


status_code fib_heap_construct(fib_heap* heap, int (*compare)(const request*, const request*))
{
	if (heap == NULL)
	{
		return NULL_ARG;
	}

	heap->head = NULL;
	heap->compare = compare;
	heap->size = 0;

	return OK;
}

status_code fib_heap_set_null(fib_heap* heap)
{
	if (heap == NULL)
	{
		return NULL_ARG;
	}

	heap->head = NULL;
	heap->compare = NULL;
	heap->size = 0;

	return OK;
}

status_code fib_heap_destruct(fib_heap* heap)
{
	if (heap == NULL)
	{
		return OK;
	}

	fib_node_destruct(heap->head);
	fib_heap_set_null(heap);

	return OK;
}

status_code fib_heap_copy(fib_heap* dest, const fib_heap* src)
{
	if (dest == NULL || src == NULL)
	{
		return NULL_ARG;
	}

	status_code status = fib_heap_construct(dest, src->compare);
	status = status ? status : fib_node_copy(&(dest->head), src->head, NULL);
	if (status != OK)
	{
		fib_heap_destruct(dest);
		return status;
	}
	
	dest->size = src->size;

	return OK;
}

status_code fib_heap_meld(fib_heap* res, fib_heap* heap_l, fib_heap* heap_r)
{
	if (res == NULL || heap_l == NULL || heap_r == NULL)
	{
		return NULL_ARG;
	}
	
	if (heap_l == heap_r)
	{
		return INVALID_INPUT;
	}
	
	if (heap_l->head == NULL || heap_r->head == NULL)
	{
		res->head = heap_l->head == NULL ? heap_r->head : heap_l->head;
		fib_heap_set_null(heap_l);
		fib_heap_set_null(heap_r);
		return OK;
	}
	
	fib_heap heap_tmp;
	
	heap_tmp.size = heap_l->size + heap_r->size;
	heap_tmp.compare = heap_l->compare;
	
	if (heap_l->head == NULL || heap_r->head == NULL)
	{
		heap_tmp.head = heap_l->head == NULL ? heap_r->head : heap_l->head;
	}
	else
	{
		heap_l->head->left->right = heap_r->head;
		heap_l->head->left = heap_r->head->left;
		heap_r->head->left = heap_l->head->left;
		heap_r->head->left->right = heap_l->head;
		
		heap_tmp.head = heap_l->compare(heap_l->head->req, heap_r->head->req) <= 0 ? heap_l->head : heap_r->head;
	}
	
	heap_l->size = heap_r->size = 0;
	heap_l->head = heap_r->head = NULL;
	*res = heap_tmp;
	
	return OK;
}

status_code fib_heap_copy_meld(fib_heap* fib_res, const fib_heap* fib_l, const fib_heap* fib_r)
{
	if (fib_res == NULL || fib_l == NULL || fib_r == NULL)
	{
		return NULL_ARG;
	}
	
	if (fib_res == fib_l || fib_res == fib_r)
	{
		return INVALID_INPUT;
	}
	
	status_code code = OK;
    fib_heap fib_lc, fib_rc;
        
    fib_heap_set_null(&fib_lc);
    fib_heap_set_null(&fib_rc);
    fib_heap_set_null(fib_res);
    
    code = code ? code : fib_heap_copy(&fib_lc, fib_l);
    code = code ? code : fib_heap_copy(&fib_rc, fib_r);
    code = code ? code : fib_heap_meld(fib_res, &fib_lc, &fib_rc);
    
    fib_heap_destruct(&fib_lc);
    fib_heap_destruct(&fib_rc);
    
    if (code)
    {
        fib_heap_destruct(fib_res);
        return code;
    }
    
    return OK;
}

status_code fib_heap_size(const fib_heap* heap, size_t* size)
{
	if (heap == NULL)
	{
		return NULL_ARG;
	}

	*size = heap->size;

	return OK;
}

status_code fib_node_merge(fib_heap* heap, fib_node* node_1, fib_node* node_2, fib_node** res)
{
	if (heap == NULL || node_1 == NULL || node_2 == NULL || res == NULL)
	{
		return NULL_ARG;
	}
	
	if (node_1 == node_2 || node_1->rank != node_2->rank)
	{
		return INVALID_INPUT;
	}
	
	if (heap->compare(node_1->req, node_2->req) > 0)
	{
		fib_node* tmp = node_1;
		node_1 = node_2;
		node_2 = tmp;
	}
	
	// hang node_2 on node_1
	if (node_1->child != NULL)
	{
		node_2->right = node_1->child;
		node_2->left = node_1->child->left;
		node_2->left->right = node_2;
		node_2->right->left = node_2;
	}
	else
	{
		node_2->left = node_2;
		node_2->right = node_2;
	}
	
	node_1->child = node_2;
	node_1->rank++;
	
	*res = node_1;

	return OK;
}

status_code fib_heap_consolidation(fib_heap* heap)
{
	if (heap == NULL)
	{
		return NULL_ARG;
	}
	
	if (heap->head == NULL || heap->head->right == heap->head)
	{
		return OK;
	}

	status_code code = OK;
	fib_node* cur = heap->head;
	fib_node* nxt = heap->head->right;
	size_t size = log2(1.0 * heap->size) + 3;

	fib_node** arr = (fib_node**) calloc(size, sizeof(fib_node*));
	if (arr == NULL)
	{
		return BAD_ALLOC;
	}
	
	heap->head->left->right = NULL;
	
	while (!code && cur != NULL)
	{
		nxt = cur->right;
		
		if (cur->right == cur)
		{
			cur->right = NULL;
		}
		
		while (!code && arr[cur->rank] != NULL)
		{
			fib_node* node_tmp = arr[cur->rank];
			arr[cur->rank] = NULL;
			code = fib_node_merge(heap, cur, node_tmp, &cur);
			
			if (code)
			{
				arr[size - 1] = cur;
			}
		}
		
		arr[cur->rank] = cur;
		cur = nxt;
	}
	
	fib_node* top = NULL;
	fib_node* start = NULL;
	fib_node* prev = NULL;
	
	for (size_t i = 0; i < size; ++i)
	{
		if (arr[i] == NULL)
		{
			continue;
		}
		
		start = start != NULL ? start : arr[i];
		top = top != NULL ? top : arr[i];
		
		if (heap->compare(arr[i]->req, top->req) < 0)
		{
			top = arr[i];
		}
		
		if (prev != NULL)
		{
			prev->right = arr[i];
		}
		
		arr[i]->left = prev;
		prev = arr[i];
	}
	
	prev->right = start;
	start->left = prev;
	heap->head = top;
	free(arr);
	
	return code;
}

status_code fib_heap_top(const fib_heap* heap, request** req)
{
	if (heap == NULL || req == NULL)
	{
		return NULL_ARG;
	}
	
	if (heap->head == NULL)
	{
		req = NULL;
		return OK;
	}

	*req = heap->head->req;

	return OK;
}

status_code fib_heap_pop(fib_heap* heap, request** req)
{
	if (heap == NULL || req == NULL)
	{
		return NULL_ARG;
	}
	
	if (heap->head == NULL)
	{
		*req = NULL;
		return OK;
	}
	
	fib_node* top = heap->head;
	*req = heap->head->req;
	
	if (heap->head->right == heap->head && heap->head->child == NULL)
	{
		heap->head = NULL;
	}
	else if (heap->head->right == heap->head)
	{
		heap->head = heap->head->child;
	}
	else if (heap->head->child == NULL)
	{
		heap->head->left->right = heap->head->right;
		heap->head->right->left = heap->head->left;
		heap->head = heap->head->left;
	}
	else
	{
		heap->head->left->right = heap->head->child;
		heap->head->right->left = heap->head->child->left;
		heap->head->left->right->left = heap->head->left;
		heap->head->right->left->right = heap->head->right;
		heap->head = heap->head->left;
	}
	
	free(top);
	heap->size--;
	
	return fib_heap_consolidation(heap);
}

status_code fib_heap_insert(fib_heap* heap, request* req)
{
	if (heap == NULL || req == NULL)
	{
		return NULL_ARG;
	}
	
	fib_node* node = (fib_node*)malloc(sizeof(fib_node));
	if (node == NULL)
	{
		return BAD_ALLOC;
	}
	
	node->req = req;
	node->child = NULL;
	node->rank = 0;
	
	if (heap->head == NULL)
	{
		node->left = node->right = node;
		heap->head = node;
	}
	else
	{	
		node->left = heap->head->left;
		node->right = heap->head;
		node->left->right = node;
		node->right->left = node;

		if (heap->compare(node->req, heap->head->req) < 0)
		{
			heap->head = node;
		}
	}
	
	heap->size++;
	
	return OK;
}