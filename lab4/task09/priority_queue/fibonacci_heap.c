#include <malloc.h>
#include <string.h>

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

	free(node->req->txt);
	free(node->req);
	free(node);

	return OK;
}

status_code fib_node_copy(fib_node** dest_p, const fib_node* src)
{
	if (dest_p == NULL)
	{
		return INVALID_ARG;
	}
	if (src == NULL)
	{
		*dest_p = NULL;

		return OK;
	}

	fib_node* dest, *head;
	fib_node* cur_dest = dest;
	fib_node* prev_dest = NULL;
	const fib_node* cur_src = src;
	status_code status;

	do
	{
		cur_dest = (fib_node*)malloc(sizeof(fib_node));
		if (cur_dest == NULL)
		{
			fib_node_destruct(dest);

			return MEM_NOT_ALLOC;
		}
		
		cur_dest->req = (request*)malloc(sizeof(request));
		if (cur_dest->req == NULL)
		{
			fib_node_destruct(dest);
			free(cur_dest);

			return MEM_NOT_ALLOC;
		}
		
		cur_dest->req->txt = (char*)malloc(sizeof(char) * (strlen(cur_src->req->txt) + 1));
		if (cur_dest->req->txt == NULL)
		{
			fib_node_destruct(dest);
			free(cur_dest->req);
			free(cur_dest);

			return MEM_NOT_ALLOC;
		}
		
		cur_dest->req->id = cur_src->req->id;
		cur_dest->req->prior = cur_src->req->prior;
		cur_dest->rank = cur_src->rank;
		strcpy(cur_dest->req->time, cur_src->req->time);
		strcpy(cur_dest->req->txt, cur_src->req->txt);

		cur_dest->child = NULL;
		cur_dest->right = NULL;
		cur_dest->left = prev_dest;
		if (prev_dest != NULL)
		{
			prev_dest->right = cur_dest;
		}
		else
		{
			head = cur_dest;
		}

		if (cur_src->child != NULL)
		{
			status = fib_node_copy(&(cur_dest->child), cur_src->child);
			if (status != OK)
			{
				fib_node_destruct(dest);

				return status; 
			}
		}

		cur_src = cur_src->right;
		prev_dest = cur_dest;
		cur_dest = cur_dest->right;

	} while (cur_src != src && cur_src != NULL);

	if (prev_dest != NULL)
	{
		prev_dest->right = head;
		head->left = prev_dest;
	}

	*dest_p = head;
	
	return OK;
}


status_code fib_heap_construct(fib_heap* heap, int (*compare)(const request*, const request*))
{
	if (heap == NULL)
	{
		return INVALID_ARG;
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
        return INVALID_ARG;
    }

    heap->head = NULL;
    heap->compare = NULL;
	heap->size = 0;

    return OK;
}

status_code fib_heap_destruct(fib_heap* heap)
{
	if (heap == NULL || heap->head == NULL)
	{
		return INVALID_ARG;
	}

	fib_node_destruct(heap->head);
	heap->head = NULL;
	heap->compare = NULL;
	heap->size = 0;

	return OK;
}

status_code fib_heap_copy(fib_heap* dest, const fib_heap* src)
{
	if (dest == NULL || src == NULL)
	{
		return INVALID_ARG;
	}

	fib_heap_construct(dest, src->compare);
	dest->size = src->size;

	status_code status = fib_node_copy(&(dest->head), src->head);
	if (status != OK)
	{
		fib_heap_destruct(dest);
	}

	return status;
}

status_code fib_heap_meld(fib_heap* res, fib_heap* heap_l, fib_heap* heap_r)
{
	if (res == NULL || heap_l == NULL || heap_r == NULL || heap_l == heap_r)
	{
		return INVALID_ARG;
	}

	res->compare = heap_l->compare;
	res->size = heap_l->size + heap_r->size;

	fib_node* second;

	if (heap_l->compare(heap_l->head->req, heap_r->head->req) == -1)
	{
		res->head = heap_l->head;
		second = heap_r->head;
	}
	else
	{
		res->head = heap_r->head;
		second = heap_l->head;
	}

	if (res->head->right != NULL)
	{
		if (second->left != NULL)
		{
			second->left->right = res->head->right;
			res->head->right->left = second->left;
		}
		else
		{
			second->right = res->head->right;
			res->head->right->left = second;
		}

		res->head->right = second;
		second->left = res->head;
	}
	else
	{
		if (second->right != NULL)
		{
			res->head->right = second->right;
			second->right->left = res->head;
		}
		else
		{
			res->head->right = second;
			second->left = res->head;
		}

		res->head->left = second;
		second->right = res->head;
	}

	heap_l->head = NULL;
	heap_r->head = NULL;

	fib_heap_destruct(heap_l);
	fib_heap_destruct(heap_r);

	return OK;
}

status_code fib_heap_copy_meld(fib_heap* res, fib_heap* heap_l, fib_heap* heap_r)
{
	fib_heap copy_heap_l, copy_heap_r;

	status_code status = fib_heap_copy(&copy_heap_l, heap_l);
	if (status != OK)
	{
		return status;
	}

	status = fib_heap_copy(&copy_heap_r, heap_r);
	if (status != OK)
	{	
		fib_heap_destruct(&copy_heap_l);
		return status;
	}

	status = fib_heap_meld(res, &copy_heap_l, &copy_heap_r);
	if (status != OK)
	{
		fib_heap_destruct(&copy_heap_l);
		fib_heap_destruct(&copy_heap_r);
	}

	return status;
}

status_code fib_heap_size(fib_heap* heap, size_t* size)
{
	if (heap == NULL)
	{
		return INVALID_ARG;
	}

	*size = heap->size;

	return OK;
}

status_code fib_heap_merge(fib_heap* heap, fib_node* node_1, fib_node* node_2, fib_node** res)
{
	if (node_1 == node_2 || node_1 == NULL || node_2 == NULL || node_1->rank != node_2->rank)
	{
		return INVALID_ARG;
	}

	if (heap->compare(node_1->req, node_2->req) <= 0)
	{
		if (node_2->left == node_2->right)
		{
			node_1->left = NULL;
			node_1->right = NULL;
		}
		else
		{
			node_2->left->right = node_2->right;
			node_2->right->left = node_2->left;
		}

		if (node_1->child != NULL)
		{
			if (node_1->child->right == NULL)
			{
				node_1->child->right = node_2;
				node_1->child->left = node_2;

				node_2->left = node_1->child;
				node_2->right = node_1->child;
			}
			else
			{
				node_1->child->left->right = node_2;
				node_2->left = node_1->child->left;

				node_1->child->left = node_2;
				node_2->right = node_1->child;
			}
		}
		else
		{
			node_1->child = node_2;
			node_2->left = NULL;
			node_2->right = NULL;
		}

		*res = node_1;
	}
	else
	{
		if (node_1->left == node_1->right)
		{
			node_2->left = NULL;
			node_2->right = NULL;
		}
		else
		{
			node_1->left->right = node_1->right;
			node_1->right->left = node_1->left;
		}

		if (node_2->child != NULL)
		{
			if (node_2->child->right == NULL)
			{
				node_2->child->right = node_1;
				node_2->child->left = node_1;

				node_1->left = node_2->child;
				node_1->right = node_2->child;
			}
			else
			{
				node_2->child->left->right = node_1;
				node_1->left = node_2->child->left;

				node_2->child->left = node_1;
				node_1->right = node_2->child;
			}
		}
		else
		{
			node_2->child = node_1;
			node_1->left = NULL;
			node_1->right = NULL;
		}

		*res = node_2;
	}

	(*res)->rank++;

	return OK;
}

status_code fib_heap_consolidation (fib_heap* heap)
{
	if (heap == NULL || heap->head == NULL || heap->head->right == NULL)
	{
		return INVALID_ARG;
	}

	fib_node* cur = heap->head;
	fib_node* top = heap->head;

	size_t size = cur->rank + 1;

	fib_node** arr = (fib_node**)calloc(size, sizeof(fib_node*));
	if (arr == NULL)
	{
		return MEM_NOT_ALLOC;
	}

	arr[cur->rank] = cur;
	cur = cur->right;

	int flag = 0;
	fib_node* next;
	fib_node* merge_res;

	while (cur != NULL && (cur != heap->head || flag))
	{
		if (heap->compare(cur->req, top->req) == -1)
		{
			top = cur;
		}

		if (cur->rank >= size)
		{
			fib_node** tmp = (fib_node**)realloc(arr, sizeof(fib_node*) * (cur->rank + 1));
			if (tmp == NULL)
			{
				free(arr);
				return MEM_NOT_ALLOC;
			}

			arr = tmp;

			for (size_t i = size; i <= cur->rank; ++i)
			{
				arr[i] = NULL;
			}
			size = cur->rank + 1;
		}

		if (arr[cur->rank] == NULL)
		{
			arr[cur->rank] = cur;

			if (flag)
			{
				cur = next;
				flag = 0;
			}
			else
			{
				cur = cur->right;
			}
		}
		else
		{
			unsigned int ind = cur->rank;
			
			if (!flag)
			{
				next = cur->right;
			}

			fib_heap_merge(heap, cur, arr[ind], &merge_res);
			cur = merge_res;

			arr[ind] = NULL;
			flag = 1;

			cur = cur->right;
		}
	}

	heap->head = top;
	free(arr);

	return OK;
}

status_code fib_heap_top (fib_heap* heap, request** req)
{
	if (heap == NULL || heap->head == NULL || req == NULL)
	{
		return INVALID_ARG;
	}

	*req = heap->head->req;

	return OK;
}

status_code fib_heap_pop(fib_heap* heap, request** req)
{
	if (fib_heap_top(heap, req) == INVALID_ARG)
	{
		return INVALID_ARG;
	}

	if (heap->head->right != NULL)
	{
		if (heap->head->left == heap->head->right)
		{
			if (heap->head->child == NULL)
			{
				heap->head = heap->head->left;

				free(heap->head->left);

				heap->head->left = NULL;
				heap->head->right = NULL;

				return OK;
			}
		}
		
		if (heap->head->child == NULL)
		{
			heap->head->left->right = heap->head->right;
			heap->head->right->left = heap->head->left;
		}
		else if (heap->head->child->left != NULL)
		{
			heap->head->child->left->right = heap->head->right;
			heap->head->right->left = heap->head->child->left;

			heap->head->child->left = heap->head->left;
			heap->head->left->right = heap->head->child;
		}
		else
		{	heap->head->child->right = heap->head->right;
			heap->head->child->left = heap->head->left;

			heap->head->right->left = heap->head->child;
			heap->head->left->right = heap->head->child;
		}

		fib_node* tmp_top = heap->head->left;

		free(heap->head);

		heap->head = tmp_top;

	}
	else
	{
		if (heap->head->child != NULL)
		{
			fib_node* new_top = heap->head->child;

			free(heap->head);

			heap->head = new_top;
		}
		else
		{
			free(heap->head);
			heap->head = NULL;

			return OK;
		}
	}

	status_code status = fib_heap_consolidation(heap);
	
	return status;
}

status_code fib_heap_insert(fib_heap* heap, request* req)
{
	if (heap == NULL || req == NULL)
	{
		return INVALID_ARG;
	}
	
	fib_node* node = (fib_node*)malloc(sizeof(fib_node));
	if (node == NULL)
	{
		return MEM_NOT_ALLOC;
	}

	node->req = req;
	node->child = NULL;
	node->right = NULL;
	node->left = NULL;
	node->rank = 0;
	
	if (heap->head == NULL)
	{
		heap->head = node;
	}
	else
	{
		node->right = heap->head;

		if (heap->head->left != NULL)
		{
			heap->head->left->right = node;
			node->left = heap->head->left;
		}
		else
		{
			node->left = heap->head;
			heap->head->right = node;
		}

		heap->head->left = node;

		if (heap->compare(heap->head->left->req, heap->head->req) == -1)
		{
			heap->head = heap->head->left;
		}
	}

	return OK;
}