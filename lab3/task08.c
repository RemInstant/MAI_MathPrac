#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define EPS 1e-12

typedef long long ll;
typedef unsigned long long ull;

typedef enum
{
	OK,
	INVALID_INPUT,
	INVALID_FLAG,
	INVALID_NUMBER,
	FILE_OPENING_ERROR,
	FILE_CONTENT_ERROR,
	OVERFLOW,
	BAD_ALLOC
} status_codes;

void print_error(status_codes code)
{
	switch (code)
	{
		case OK:
			return;
		case INVALID_INPUT:
			printf("Invalid input\n");
			return;
		case INVALID_FLAG:
			printf("Invalid flag\n");
			return;
		case INVALID_NUMBER:
			printf("Invalid number\n");
			return;
		case FILE_OPENING_ERROR:
			printf("File cannot be opened\n");
			return;
		case FILE_CONTENT_ERROR:
			printf("Invalid content of file\n");
			return;
		case OVERFLOW:
			printf("An overflow occurred\n");
			return;
		case BAD_ALLOC:
			printf("Memory lack error occurred\n");
			return;
		default:
			printf("Unexpected error occurred\n");
			return;
	}
}

typedef struct list_elem
{
	ull pow;
	double coef;
	struct list_elem* next;
} list_elem;

typedef struct
{
	list_elem* begin;
	list_elem* end;
	ull size;
} List;

typedef struct
{
	list_elem* cur;
} List_iter;

void iter_next(List_iter* iter);
int iter_equal(const List_iter iter_l, const List_iter iter_r);

status_codes list_construct(List* list);
status_codes list_copy(const List list, List* copy);
status_codes list_destruct(List* list);
List_iter list_begin(const List list);
List_iter list_end(const List list);
status_codes list_insert(List *list, List_iter iter, ull pow, double coef);
status_codes list_remove(List *list, List_iter iter);
void list_print(const List list);

// Interpreter functions
typedef enum
{
	ADD,
	SUB,
	MULT,
	DIV,
	MOD,
	EVAL,
	DIFF,
	CMPS
} commands;

status_codes add_polynomial(const List* poly1, const List* poly2, List* summator);
status_codes sub_polynomial(const List* poly1, const List* poly2, List* summator);
status_codes mult_polynomial(const List* poly1, const List* poly2, List* summator);
status_codes div_polynomial(const List* poly1, const List* poly2, List* summator);
status_codes mod_polynomial(const List* poly1, const List* poly2, List* summator);
status_codes eval_polynomial(const List* poly, double x, List* summator);
status_codes diff_polynomial(const List* poly1, List* summator);
status_codes cmps_polynomial(const List* poly1, const List* poly2, List* summator);

int main(int argc, char** argv)
{
	// Processing inp
	
	List poly1, poly2, summator;
	list_construct(&poly1);
	list_construct(&poly2);
	list_construct(&summator);
	
	// x + 1
	list_insert(&poly1, list_end(poly1), 1, 1);
	list_insert(&poly1, list_end(poly1), 0, 1);
	
	// x - 1
	list_insert(&poly2, list_end(poly2), 1, 1);
	list_insert(&poly2, list_end(poly2), 0, -1);
	
	list_print(poly1);
	list_print(poly2);
	
	mult_polynomial(&poly1, &poly2, &summator);
	
	list_print(summator);
	
	list_destruct(&poly1);
	list_destruct(&poly2);
	list_destruct(&summator);
}

void iter_next(List_iter* iter)
{
	if (iter == NULL)
	{
		return;
	}
	iter->cur = iter->cur->next;
}

int iter_equal(const List_iter iter_l, const List_iter iter_r)
{
	return iter_l.cur == iter_r.cur;
}

status_codes list_construct(List* list)
{
	if (list == NULL)
	{
		return INVALID_INPUT;
	}
	list_elem* terminator = (list_elem*) malloc(sizeof(list_elem));
	if (terminator == NULL)
	{
		return BAD_ALLOC;
	}
	terminator->pow = 0;
	terminator->coef = 0;
	terminator->next = terminator;
	list->begin = terminator;
	list->end = terminator;
	list->size = 0;
	return OK;
}

status_codes list_copy(const List list, List* copy)
{
	if (copy == NULL)
	{
		return INVALID_INPUT;
	}
	status_codes err_code = list_construct(copy);
	if (err_code)
	{
		return err_code;
	}
	list_elem* elem = list.begin;
	while (elem != elem->next && !err_code)
	{
		err_code = list_insert(copy, list_end(*copy), elem->pow, elem->coef);
		elem = elem->next;
	}
	if (err_code)
	{
		list_destruct(copy);
		return err_code;
	}
	return OK;
}

status_codes list_destruct(List* list)
{
	if (list == NULL)
	{
		return INVALID_INPUT;
	}
	list_elem* elem = list->begin;
	list_elem* next_elem = elem->next;
	while (elem != next_elem)
	{
		free(elem);
		elem = next_elem;
		next_elem = elem->next;
	}
	free(elem);
	list->begin = NULL;
	list->end = NULL;
	list->size = 0;
	return OK;
}

List_iter list_begin(const List list)
{
	return (List_iter) { list.begin };
}

List_iter list_end(const List list)
{
	return (List_iter) { list.end };
}

status_codes list_insert(List *list, List_iter iter, ull pow, double coef)
{
	if (list == NULL)
	{
		return INVALID_INPUT;
	}
	
	list_elem* new = (list_elem*) malloc(sizeof(list_elem));
	if (new == NULL)
	{
		return BAD_ALLOC;
	}
	new->pow = pow;
	new->coef = coef;
	
	if (iter_equal(iter, list_begin(*list)))
	{
		new->next = list->begin;
		list->begin = new;
		list->size++;
		return OK;
	}
	
	list_elem* prev = list->begin;
	list_elem* cur = iter.cur;
	while (prev->next != cur)
	{
		prev = prev->next;
		if (prev == list->end)
		{
			return INVALID_INPUT;
		}
	}
	
	prev->next = new;
	new->next = cur;
	list->size++;
	return OK;
}

status_codes list_remove(List *list, List_iter iter)
{
	if (list == NULL)
	{
		return INVALID_INPUT;
	}
	
	list_elem* prev = list->begin;
	list_elem* cur = iter.cur;
	while (prev->next != cur)
	{
		prev = prev->next;
		if (prev == list->end)
		{
			return INVALID_INPUT;
		}
	}
	
	prev->next = cur->next;
	free(cur);
	list->size--;
	return OK;
}

void list_print(const List list)
{
	list_elem* elem = list.begin;
	while (elem != list.end)
	{
		printf("%lfx^%llu ", elem->coef, elem->pow);
		elem = elem->next;
	}
	printf("\n");
}



status_codes add_polynomial(const List* poly1, const List* poly2, List* summator)
{
	if (poly2 == NULL || summator == NULL)
	{
		return INVALID_INPUT;
	}
	if (poly1 == NULL)
	{
		poly1 = summator;
	}
	
	List res;
	status_codes err_code = list_construct(&res);
	
	List_iter iter1 = list_begin(*poly1);
	List_iter iter2 = list_begin(*poly2);
	
	int end_flag1 = iter_equal(iter1, list_end(*poly1));
	int end_flag2 = iter_equal(iter2, list_end(*poly2));
	
	while (!err_code && (!end_flag1 || !end_flag2))
	{
		if (!end_flag1 && !end_flag2 && iter1.cur->pow == iter2.cur->pow)
		{
			ull pow = iter1.cur->pow;
			double coef = iter1.cur->coef + iter2.cur->coef;
			err_code = list_insert(&res, list_end(res), pow, coef);
			iter_next(&iter1);
			iter_next(&iter2);
		}
		else if (end_flag2 || (!end_flag1 && iter1.cur->pow > iter2.cur->pow))
		{
			ull pow = iter1.cur->pow;
			double coef = iter1.cur->coef;
			err_code = list_insert(&res, list_end(res), pow, coef);
			iter_next(&iter1);
		}
		else
		{
			ull pow = iter2.cur->pow;
			double coef = iter2.cur->coef;
			err_code = list_insert(&res, list_end(res), pow, coef);
			iter_next(&iter2);
		}
		end_flag1 = iter_equal(iter1, list_end(*poly1));
		end_flag2 = iter_equal(iter2, list_end(*poly2));
	}
	
	list_destruct(summator);
	*summator = res;
	return OK;
}

status_codes sub_polynomial(const List* poly1, const List* poly2, List* summator)
{
	if (poly2 == NULL || summator == NULL)
	{
		return INVALID_INPUT;
	}
	if (poly1 == NULL)
	{
		poly1 = summator;
	}
	
	List res;
	status_codes err_code = list_construct(&res);
	
	List_iter iter1 = list_begin(*poly1);
	List_iter iter2 = list_begin(*poly2);
	
	int end_flag1 = iter_equal(iter1, list_end(*poly1));
	int end_flag2 = iter_equal(iter2, list_end(*poly2));
	
	while (!err_code && (!end_flag1 || !end_flag2))
	{
		if (!end_flag1 && !end_flag2 && iter1.cur->pow == iter2.cur->pow)
		{
			ull pow = iter1.cur->pow;
			double coef = iter1.cur->coef - iter2.cur->coef;
			err_code = list_insert(&res, list_end(res), pow, coef);
			iter_next(&iter1);
			iter_next(&iter2);
		}
		else if (end_flag2 || (!end_flag1 && iter1.cur->pow > iter2.cur->pow))
		{
			ull pow = iter1.cur->pow;
			double coef = iter1.cur->coef;
			err_code = list_insert(&res, list_end(res), pow, coef);
			iter_next(&iter1);
		}
		else
		{
			ull pow = iter2.cur->pow;
			double coef = -iter2.cur->coef;
			err_code = list_insert(&res, list_end(res), pow, coef);
			iter_next(&iter2);
		}
		end_flag1 = iter_equal(iter1, list_end(*poly1));
		end_flag2 = iter_equal(iter2, list_end(*poly2));
	}
	
	list_destruct(summator);
	*summator = res;
	return OK;
}

status_codes mult_polynomial(const List* poly1, const List* poly2, List* summator)
{
	if (poly2 == NULL || summator == NULL)
	{
		return INVALID_INPUT;
	}
	if (poly1 == NULL)
	{
		poly1 = summator;
	}
	
	List res;
	status_codes err_code = list_construct(&res);
	if (err_code)
	{
		return BAD_ALLOC;
	}
	
	List_iter iter1 = list_begin(*poly1);
	while(!err_code && !iter_equal(iter1, list_end(*poly1)))
	{
		List tmp;
		err_code = list_copy(*poly2, &tmp);
		if (err_code)
		{
			list_destruct(&res);
			return BAD_ALLOC;
		}
		List_iter iter2 = list_begin(*poly2);
		while (!iter_equal(iter2, list_end(*poly2)))
		{
			iter2.cur->pow += iter1.cur->pow;
			iter2.cur->coef *= iter1.cur->coef;
			iter_next(&iter2);
		}
		err_code = add_polynomial(&res, &tmp, &res);
		list_destruct(&tmp);
		iter_next(&iter1);
	}
	if (err_code)
	{
		list_destruct(&res);
		return err_code;
	}
		
	list_destruct(summator);
	*summator = res;
	return OK;
}

status_codes div_polynomial(const List* poly1, const List* poly2, List* summator)
{
	if (poly2 == NULL || summator == NULL)
	{
		return INVALID_INPUT;
	}
	if (poly1 == NULL)
	{
		poly1 = summator;
	}
	
	List res, tmp;
	status_codes err_code = list_construct(&res);
	if (err_code)
	{
		return BAD_ALLOC;
	}
	err_code = list_copy(*poly1, &tmp);
	if (err_code)
	{
		list_destruct(&res);
		return BAD_ALLOC;
	}
	
	List_iter iter1 = list_begin(*poly1);
	while(!err_code && !iter_equal(iter1, list_end(*poly1)))
	{
		List tmp;
		err_code = list_copy(*poly2, &tmp);
		if (err_code)
		{
			list_destruct(&res);
			return BAD_ALLOC;
		}
		List_iter iter2 = list_begin(*poly2);
		while (!iter_equal(iter2, list_end(*poly2)))
		{
			iter2.cur->pow += iter1.cur->pow;
			iter2.cur->coef *= iter1.cur->coef;
			iter_next(&iter2);
		}
		err_code = add_polynomial(&res, &tmp, &res);
		list_destruct(&tmp);
		iter_next(&iter1);
	}
	if (err_code)
	{
		list_destruct(&res);
		return err_code;
	}
		
	list_destruct(summator);
	*summator = res;
	return OK;
}

status_codes mod_polynomial(const List* poly1, const List* poly2, List* summator);
status_codes eval_polynomial(const List* poly, double x, List* summator);
status_codes diff_polynomial(const List* poly1, List* summator);
status_codes cmps_polynomial(const List* poly1, const List* poly2, List* summator);
