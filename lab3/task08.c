#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <errno.h>

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
	DIVISION_BY_ZERO,
	BAD_ALLOC,
	CORRUPTED_MEMORY
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
		case DIVISION_BY_ZERO:
			printf("Division by zero occurred\n");
			return;
		case BAD_ALLOC:
			printf("Memory lack error occurred\n");
			return;
		case CORRUPTED_MEMORY:
			printf("Memory was corrupted\n");
			return;
		default:
			printf("Unexpected error occurred\n");
			return;
	}
}

typedef struct list_elem
{
	ull pow;
	ll coef;
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

status_codes list_set_null(List* list);
status_codes list_construct(List* list);
status_codes list_copy(const List list, List* copy);
status_codes list_destruct(List* list);
List_iter list_begin(const List list);
List_iter list_end(const List list);
status_codes list_insert(List *list, List_iter iter, ull pow, ll coef);
status_codes list_remove(List *list, List_iter iter);
void list_print(const List list);

typedef enum
{
	NONE,
	ADD,
	SUB,
	MULT,
	DIV,
	MOD,
	EVAL,
	DIFF,
	CMPS
} polynomial_command;

status_codes polynomial_construct(const char* str, List* poly);
status_codes polynomial_add(List poly1, List poly2, List* summator);
status_codes polynomial_sub(List poly1, List poly2, List* summator);
status_codes polynomial_mult(List poly1, List poly2, List* summator);
status_codes polynomial_div(List poly1, List poly2, List* summator);
status_codes polynomial_mod(List poly1, List poly2, List* summator);
status_codes polynomial_eval(List poly, ll x, ll* value);
status_codes polynomial_diff(List poly, List* summator);
status_codes polynomial_cmps(List poly1, List poly2, List* summator);

status_codes sread_until(const char* src, const char* delims, const char** end_ptr, char** str);
status_codes fread_cmd(FILE* file, char** str);
status_codes parse_cmd(const char* src, polynomial_command* cmd, ull* arg_cnt, char** arg1, char** arg2);

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: cmd_path <input>\n");
		return OK;
	}
	if (argc != 2)
	{
		print_error(INVALID_INPUT);
		return INVALID_INPUT;
	}
	
	FILE* input = fopen(argv[1], "r");
	if (input == NULL)
	{
		print_error(FILE_OPENING_ERROR);
		return FILE_OPENING_ERROR;
	}
	
	List summator;
	list_set_null(&summator);
	status_codes err_code = list_construct(&summator);
	
	while(!err_code && !feof(input))
	{
		char* cmd = NULL;
		err_code = fread_cmd(input, &cmd);
		
		polynomial_command cmd_type;
		ull arg_cnt;
		char* arg1 = NULL;
		char* arg2 = NULL;
		
		err_code = err_code ? err_code : parse_cmd(cmd, &cmd_type, &arg_cnt, &arg1, &arg2);
		free(cmd);
		
		int full_arg_cmd = 1;
		ll x = 0, value = 0;
		List poly1, poly2, res;
		list_set_null(&poly1);
		list_set_null(&poly2);
		list_set_null(&res);
		
		switch (cmd_type)
		{
			case NONE:
				break;
			case ADD:
			case SUB:
			case MULT:
			case DIV:
			case MOD:
			case CMPS:
			{
				if (arg_cnt == 2)
				{
					err_code = err_code ? err_code : polynomial_construct(arg1, &poly1);
					err_code = err_code ? err_code : polynomial_construct(arg2, &poly2);
				}
				else
				{
					poly1 = summator;
					err_code = err_code ? err_code : polynomial_construct(arg1, &poly2);
					full_arg_cmd = 0;
				}
				err_code = err_code ? err_code : list_construct(&res);
				break;
			}
			case EVAL:
			{
				char* check_ptr;
				x = strtod(arg1, &check_ptr);
				err_code = err_code ? err_code : (*check_ptr == '\0' ? OK : INVALID_INPUT);
				break;
			}
			case DIFF:
			{
				if (arg_cnt == 1)
				{
					err_code = err_code ? err_code : polynomial_construct(arg1, &poly1);
				}
				else
				{
					poly1 = summator;
					full_arg_cmd = 0;
				}
				err_code = err_code ? err_code : list_construct(&res);
				break;
			}
		}
		
		free(arg1);
		free(arg2);
		
		switch (cmd_type)
		{
			case NONE:
				break;
			case ADD:
				err_code = err_code ? err_code : polynomial_add(poly1, poly2, &summator);
				break;
			case SUB:
				err_code = err_code ? err_code : polynomial_sub(poly1, poly2, &summator);
				break;
			case MULT:
				err_code = err_code ? err_code : polynomial_mult(poly1, poly2, &summator);
				break;
			case DIV:
				err_code = err_code ? err_code : polynomial_div(poly1, poly2, &summator);
				break;
			case MOD:
				err_code = err_code ? err_code : polynomial_mod(poly1, poly2, &summator);
				break;
			case CMPS:
				err_code = err_code ? err_code : polynomial_cmps(poly1, poly2, &summator);
				break;
			case EVAL:
				err_code = err_code ? err_code : polynomial_eval(summator, x, &value);
				break;
			case DIFF:
				err_code = err_code ? err_code : polynomial_diff(poly1, &summator);
				break;
		}
		
		if (!err_code)
		{
			if (cmd_type == EVAL)
			{
				printf("value of summator at %lld is %lld\n", x, value);
			}
			else if (cmd_type != NONE)
			{
				list_print(summator);
			}
		}
		
		if (full_arg_cmd)
		{
			list_destruct(&poly1);
		}
		list_destruct(&poly2);
		list_destruct(&res);
	}
	
	fclose(input);
	list_destruct(&summator);
	
	if (err_code)
	{
		print_error(err_code);
		return err_code;
	}
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

status_codes list_set_null(List* list)
{
	if (list == NULL)
	{
		return INVALID_INPUT;
	}
	list->begin = NULL;
	list->end = NULL;
	list->size = 0;
	return OK;
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
	if (list->begin == NULL || list->end == NULL)
	{
		return list->begin == list->end ? OK : CORRUPTED_MEMORY;
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

status_codes list_insert(List *list, List_iter iter, ull pow, ll coef)
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
	
	list_elem* cur = iter.cur;
	if (list->begin == cur)
	{
		list->begin = cur->next;
	}
	else
	{
		list_elem* prev = list->begin;
		while (prev->next != cur)
		{
			prev = prev->next;
			if (prev == list->end)
			{
				return INVALID_INPUT;
			}
		}
		prev->next = cur->next;
	}
	free(cur);
	list->size--;
	return OK;
}

void list_print(const List list)
{
	list_elem* elem = list.begin;
	if (list.size == 0)
	{
		printf("0");
	}
	while (elem != list.end)
	{
		if (elem->coef < 0)
		{
			printf("-");
		}
		else if (elem != list.begin)
		{
			printf("+");
		}
		if ((elem->coef != 1 && elem->coef != -1) || elem->pow == 0)
		{
			printf("%lld", elem->coef < 0 ? -elem->coef : elem->coef);
		}
		if (elem->pow != 0)
		{
			if (elem->pow != 1)
			{
				printf("x^%llu", elem->pow);
			}
			else
			{
				printf("x");
			}
		}
		elem = elem->next;
	}
	printf("\n");
}

status_codes polynomial_construct(const char* src, List* poly)
{
	if (src == NULL || poly == NULL)
	{
		return INVALID_INPUT;
	}
	
	list_set_null(poly);
	status_codes err_code = list_construct(poly);
	int state = 0; // 0 - coef, 1 - var skip, 2 - pow, 3 - appending
	
	const char* ptr = src;
	ull pow = 0;
	ll coef = 0;
	while (!err_code && (*ptr != '\0' || state == 3))
	{
		char* str = NULL;
		// READ COEF
		if (state == 0)
		{
			int sign = *ptr == '-' ? -1 : 1;
			char* check_ptr;
			if (*ptr == '-' || *ptr == '+')
			{
				++ptr;
			}
			err_code = sread_until(ptr, "x+-", &ptr, &str);
			int wait_variable = 0;
			if (str[0] == '\0')
			{
				coef = sign;
				wait_variable = 1;
			}
			else
			{
				coef = strtoll(str, &check_ptr, 10);
				coef *= sign;
				err_code = err_code ? err_code : (*check_ptr == '\0' ? OK : INVALID_INPUT);
			}
			if (*ptr == 'x')
			{
				wait_variable = 0;
				state = 1; // skip variable
			}
			else
			{
				pow = 0;
				state = 3; // save data
			}
			if (!err_code && wait_variable)
			{
				err_code = INVALID_INPUT;
			}
		}
		// SKIP VARIABLE
		else if (state == 1)
		{
			++ptr;
			if (*ptr == '+' || *ptr == '-' || *ptr == '\0')
			{
				pow = 1;
				state = 3; // save data
			}
			else if (*ptr == '^')
			{
				state = 2; // read pow
			}
			else
			{
				err_code = INVALID_INPUT;
			}
		}
		// READ POW
		else if (state == 2)
		{
			++ptr;
			char* check_ptr;
			err_code = sread_until(ptr, "+-", &ptr, &str);
			errno = 0;
			pow = strtoull(str, &check_ptr, 10);
			err_code = err_code ? err_code : (errno == ERANGE ? OVERFLOW : OK);
			err_code = err_code ? err_code : (*check_ptr == '\0' ? OK : INVALID_INPUT);
			state = 3; // save data
		}
		// SAVE DATA
		else if (state == 3)
		{
			if (coef != 0)
			{
				List_iter iter = list_begin(*poly);
				List_iter iter_end = list_end(*poly);
				while (iter.cur->pow > pow)
				{
					iter_next(&iter);
				}
				if (iter.cur->pow == pow && !iter_equal(iter, iter_end))
				{
					iter.cur->coef += coef;
				}
				else
				{
					err_code = list_insert(poly, iter, pow, coef);
				}
			}
			state = 0;
		}
		free(str);
	}
	
	if (err_code)
	{
		list_destruct(poly);
		return err_code;
	}
	return OK;
}

status_codes polynomial_add(List poly1, List poly2, List* summator)
{
	if (summator == NULL)
	{
		return INVALID_INPUT;
	}
	
	List res;
	list_set_null(&res);
	status_codes err_code = list_construct(&res);
	
	List_iter iter1 = list_begin(poly1);
	List_iter iter2 = list_begin(poly2);
	List_iter iter1_end = list_end(poly1);
	List_iter iter2_end = list_end(poly2);
	
	int end_flag1 = iter_equal(iter1, iter1_end);
	int end_flag2 = iter_equal(iter2, iter2_end);

	while (!err_code && (!end_flag1 || !end_flag2))
	{
		if (!end_flag1 && !end_flag2 && iter1.cur->pow == iter2.cur->pow)
		{
			ull pow = iter1.cur->pow;
			ll coef = iter1.cur->coef + iter2.cur->coef;
			if (coef != 0)
			{
				err_code = list_insert(&res, list_end(res), pow, coef);
			}
			iter_next(&iter1);
			iter_next(&iter2);
		}
		else if (end_flag2 || (!end_flag1 && iter1.cur->pow > iter2.cur->pow))
		{
			ull pow = iter1.cur->pow;
			ll coef = iter1.cur->coef;
			err_code = list_insert(&res, list_end(res), pow, coef);
			iter_next(&iter1);
		}
		else
		{
			ull pow = iter2.cur->pow;
			ll coef = iter2.cur->coef;
			err_code = list_insert(&res, list_end(res), pow, coef);
			iter_next(&iter2);
		}
		end_flag1 = iter_equal(iter1, iter1_end);
		end_flag2 = iter_equal(iter2, iter2_end);
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

status_codes polynomial_sub(List poly1, List poly2, List* summator)
{
	if (summator == NULL)
	{
		return INVALID_INPUT;
	}
	
	List res;
	list_set_null(&res);
	status_codes err_code = list_construct(&res);
	
	List_iter iter1 = list_begin(poly1);
	List_iter iter2 = list_begin(poly2);
	List_iter iter1_end = list_end(poly1);
	List_iter iter2_end = list_end(poly2);
	
	int end_flag1 = iter_equal(iter1, iter1_end);
	int end_flag2 = iter_equal(iter2, iter2_end);
	
	while (!err_code && (!end_flag1 || !end_flag2))
	{
		if (!end_flag1 && !end_flag2 && iter1.cur->pow == iter2.cur->pow)
		{
			ull pow = iter1.cur->pow;
			ll coef = iter1.cur->coef - iter2.cur->coef;
			if (coef != 0)
			{
				err_code = list_insert(&res, list_end(res), pow, coef);
			}
			iter_next(&iter1);
			iter_next(&iter2);
		}
		else if (end_flag2 || (!end_flag1 && iter1.cur->pow > iter2.cur->pow))
		{
			ull pow = iter1.cur->pow;
			ll coef = iter1.cur->coef;
			err_code = list_insert(&res, list_end(res), pow, coef);
			iter_next(&iter1);
		}
		else
		{
			ull pow = iter2.cur->pow;
			ll coef = -iter2.cur->coef;
			err_code = list_insert(&res, list_end(res), pow, coef);
			iter_next(&iter2);
		}
		end_flag1 = iter_equal(iter1, iter1_end);
		end_flag2 = iter_equal(iter2, iter2_end);
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

status_codes polynomial_mult(List poly1, List poly2, List* summator)
{
	if (summator == NULL)
	{
		return INVALID_INPUT;
	}
	
	List res;
	list_set_null(&res);
	status_codes err_code = list_construct(&res);
	
	List_iter iter1 = list_begin(poly1);
	List_iter iter1_end = list_end(poly1);
	while(!err_code && !iter_equal(iter1, iter1_end))
	{
		List tmp;
		list_set_null(&tmp);
		err_code = list_copy(poly2, &tmp);
		List_iter iter2 = list_begin(tmp);
		List_iter iter2_end = list_end(tmp);
		while (!err_code && !iter_equal(iter2, iter2_end))
		{
			iter2.cur->pow += iter1.cur->pow;
			iter2.cur->coef *= iter1.cur->coef;
			iter_next(&iter2);
		}
		err_code = err_code ? err_code : polynomial_add(res, tmp, &res);
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

status_codes polynomial_div(List poly1, List poly2, List* summator)
{
	if (summator == NULL)
	{
		return INVALID_INPUT;
	}
	if (poly2.size == 0)
	{
		return DIVISION_BY_ZERO;
	}
	
	List res, tmp;
	list_set_null(&res);
	list_set_null(&tmp);
	
	status_codes err_code = list_construct(&res);
	err_code = err_code ? err_code : list_copy(poly1, &tmp);
	
	ull cur_pow = list_begin(tmp).cur->pow;
	ull div_pow = list_begin(poly2).cur->pow;
	ll cur_coef = list_begin(tmp).cur->coef;
	ll div_coef = list_begin(poly2).cur->coef;
	
	int run_flag = 1;
	while(!err_code && run_flag)
	{
		List subtr;
		list_set_null(&subtr);
		err_code = list_copy(poly2, &subtr);
		
		ull res_pow = cur_pow - div_pow;
		ll res_coef = cur_coef / div_coef;
		err_code = err_code ? err_code : list_insert(&res, list_end(res), res_pow, res_coef);
		
		List_iter iter = list_begin(subtr);
		List_iter iter_end = list_end(subtr);
		while (!err_code && !iter_equal(iter, iter_end))
		{
			iter.cur->pow += res_pow;
			iter.cur->coef *= res_coef;
			iter_next(&iter);
		}
		
		List new_tmp;
		list_set_null(&new_tmp);
		err_code = err_code ? err_code : list_construct(&new_tmp);
		err_code = err_code ? err_code : polynomial_sub(tmp, subtr, &new_tmp);
		list_destruct(&subtr);
		list_destruct(&tmp);
		tmp = new_tmp;
		
		if (!err_code)
		{
			iter = list_begin(tmp);
			iter_end = list_end(tmp);
			while (iter.cur->pow >= div_pow && iter.cur->coef < div_coef && !iter_equal(iter, iter_end))
			{
				iter_next(&iter);
			}
			if (iter.cur->pow < div_pow || iter_equal(iter, iter_end))
			{
				run_flag = 0;
			}
			else
			{
				cur_pow = iter.cur->pow;
				cur_coef = iter.cur->coef;
			}
		}
	}
	
	list_destruct(&tmp);
	if (err_code)
	{
		list_destruct(&res);
		return err_code;
	}

	list_destruct(summator);
	*summator = res;
	return OK;
}

status_codes polynomial_mod(List poly1, List poly2, List* summator)
{
	if (summator == NULL)
	{
		return INVALID_INPUT;
	}
	if (poly2.size == 0)
	{
		return DIVISION_BY_ZERO;
	}
	
	List res;
	list_set_null(&res);
	status_codes err_code = list_copy(poly1, &res);
	
	ull cur_pow = list_begin(res).cur->pow;
	ull div_pow = list_begin(poly2).cur->pow;
	ll cur_coef = list_begin(res).cur->coef;
	ll div_coef = list_begin(poly2).cur->coef;
	
	int run_flag = 1;
	while(!err_code && run_flag)
	{
		List subtr;
		list_set_null(&subtr);
		err_code = err_code ? err_code : list_copy(poly2, &subtr);
		
		ull pow = cur_pow - div_pow;
		ll coef = cur_coef / div_coef;
		List_iter iter = list_begin(subtr);
		List_iter iter_end = list_end(subtr);
		while (!err_code && !iter_equal(iter, iter_end))
		{
			iter.cur->pow += pow;
			iter.cur->coef *= coef;
			iter_next(&iter);
		}
		
		List new_res;
		list_set_null(&new_res);
		err_code = err_code ? err_code : list_construct(&new_res);
		err_code = err_code ? err_code : polynomial_sub(res, subtr, &new_res);
		list_destruct(&subtr);
		list_destruct(&res);
		res = new_res;

		if (!err_code)
		{
			iter = list_begin(res);
			iter_end = list_end(res);
			while (iter.cur->pow >= div_pow && iter.cur->coef < div_coef && !iter_equal(iter, iter_end))
			{
				iter_next(&iter);
			}
			if (iter.cur->pow < div_pow || iter_equal(iter, iter_end))
			{
				run_flag = 0;
			}
			else
			{
				cur_pow = iter.cur->pow;
				cur_coef = iter.cur->coef;
			}
		}
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

status_codes polynomial_eval(List poly, ll x, ll* value)
{
	ll res = 0;
	ll prev_pow = 0;
	List_iter iter = list_begin(poly);
	List_iter iter_end = list_end(poly);
	while(!iter_equal(iter, iter_end))
	{
		for (ull i = iter.cur->pow; i < prev_pow; ++i)
		{
			res *= x;
		}
		res += iter.cur->coef;
		prev_pow = iter.cur->pow;
		iter_next(&iter);
	}
	for (ull i = 0; i < prev_pow; ++i)
	{
		res *= x;
	}
	*value = res;
	return OK;
}

status_codes polynomial_diff(List poly, List* summator)
{
	if (summator == NULL)
	{
		return INVALID_INPUT;
	}
	
	List res;
	list_set_null(&res);
	status_codes err_code = list_construct(&res);
	
	List_iter iter = list_begin(poly);
	List_iter iter_end = list_end(poly);
	
	while(!err_code && !iter_equal(iter, iter_end))
	{
		if (iter.cur->pow > 0)
		{
			ull pow = iter.cur->pow - 1;
			ll coef = iter.cur->coef * iter.cur->pow;
			err_code = list_insert(&res, list_end(res), pow, coef);
		}
		iter_next(&iter);
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

status_codes polynomial_cmps(List poly1, List poly2, List* summator)
{
	if (summator == NULL)
	{
		return INVALID_INPUT;
	}
	
	List res;
	list_set_null(&res);
	status_codes err_code = list_construct(&res);
	
	ull prev_pow = 0;
	List_iter iter = list_begin(poly1);
	List_iter iter_end = list_end(poly1);
	while (!err_code && !iter_equal(iter, iter_end))
	{
		for (ull i = iter.cur->pow; i < prev_pow; ++i)
		{
			List res_tmp;
			list_set_null(&res_tmp);
			err_code = err_code ? err_code : polynomial_mult(res, poly2, &res_tmp);
			list_destruct(&res);
			res = res_tmp;
		}

		List res_tmp, tmp;
		list_set_null(&res_tmp);
		list_set_null(&tmp);
		err_code = err_code ? err_code : list_construct(&res_tmp);
		err_code = err_code ? err_code : list_construct(&tmp);
		err_code = err_code ? err_code : list_insert(&tmp, list_end(tmp), 0, iter.cur->coef);
		err_code = err_code ? err_code : polynomial_add(res, tmp, &res_tmp);
		list_destruct(&tmp);
		list_destruct(&res);
		res = res_tmp;
		prev_pow = iter.cur->pow;
		iter_next(&iter);
	}
	for (ull i = 0; i < prev_pow; ++i)
	{
		List res_tmp;
		list_set_null(&res_tmp);
		err_code = err_code ? err_code : polynomial_mult(res, poly2, &res_tmp);
		list_destruct(&res);
		res = res_tmp;
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

status_codes sread_until(const char* src, const char* delims, const char** end_ptr, char** str)
{
	if (src == NULL || str == NULL)
	{
		return INVALID_INPUT;
	}
	ull cnt = 0;
	ull size = 2;
	*str = (char*) malloc(sizeof(char) * size);
	if (*str == NULL)
	{
		return BAD_ALLOC;
	}
	
	char flags[256];
	for (ull i = 0; i < 256; ++i)
	{
		flags[i] = 1;
	}
	for (ull i = 0; delims[i]; ++i)
	{
		flags[(int) delims[i]] = 0;
	}
	const char* ptr = src;
	
	while (flags[(int) *ptr] && *ptr != '\0')
	{
		if (cnt + 1 == size)
		{
			size *= 2;
			char* temp_word = realloc(*str, sizeof(char) * size);
			if (temp_word == NULL)
			{
				free(*str);
				return BAD_ALLOC;
			}
			*str = temp_word;
		}
		(*str)[cnt++] = *ptr;
		++ptr;
	}
	(*str)[cnt] = '\0';
	*end_ptr = ptr;
	return OK;
}

status_codes fread_cmd(FILE* file, char** str)
{
	if (file == NULL || str == NULL)
	{
		return INVALID_INPUT;
	}
	ull cnt = 0;
	ull size = 2;
	*str = (char*) malloc(sizeof(char) * size);
	if (*str == NULL)
	{
		return BAD_ALLOC;
	}
	
	char ch = getc(file);
	int skip_flag = 1;
	while (skip_flag)
	{
		while (ch == ' ' || ch == '\t' || ch == '\n')
		{
			ch = getc(file);
		}
		skip_flag = 0;
		
		if (ch == '%')
		{
			while (ch != '\n' && !feof(file))
			{
				ch = getc(file);
			}
			ch = getc(file);
			skip_flag = 1;
		}
		else if (ch == '[')
		{
			while (ch != ']' && !feof(file))
			{
				ch = getc(file);
			}
			if (ch != ']')
			{
				free(*str);
				return FILE_CONTENT_ERROR;
			}
			ch = getc(file);
			skip_flag = 1;
		}
	}
	
	while (ch != ';' && !feof(file))
	{
		if (cnt + 2 == size)
		{
			size *= 2;
			char* temp_word = realloc(*str, sizeof(char) * size);
			if (temp_word == NULL)
			{
				free(*str);
				return BAD_ALLOC;
			}
			*str = temp_word;
		}
		(*str)[cnt++] = ch;
		ch = getc(file);
	}
	if (ch == ';')
	{
		(*str)[cnt++] = ch;
	}
	(*str)[cnt] = '\0';
	return OK;
}

status_codes parse_cmd(const char* src, polynomial_command* cmd, ull* arg_cnt, char** arg1, char** arg2)
{
	if (src == NULL || cmd == NULL || arg_cnt == NULL || arg1 == NULL || arg2 == NULL)
	{
		return INVALID_INPUT;
	}
	
	if (src[0] == '\0')
	{
		*cmd = NONE;
		*arg_cnt = 0;
		*arg1 = NULL;
		*arg2 = NULL;
		return OK;
	}
	
	status_codes err_code = OK;
	const char* ptr = src;
	ull arg_cnt_tmp = 0;
	char* str_cmd = NULL;
	polynomial_command cmd_tmp;
	char* arg1_tmp = NULL;
	char* arg2_tmp = NULL;
	
	err_code = sread_until(ptr, "(", &ptr, &str_cmd);
	++ptr;
	if (!err_code && *ptr != ')')
	{
		err_code = sread_until(ptr, ",)", &ptr, &arg1_tmp);
		++arg_cnt_tmp;
	}
	if (!err_code && *ptr == ',')
	{
		++ptr;
		err_code =  sread_until(ptr, ")", &ptr, &arg2_tmp);
		++arg_cnt_tmp;
	}
	
	err_code = err_code ? err_code : (*ptr == ')' ? OK : INVALID_INPUT);
	++ptr;
	err_code = err_code ? err_code : (*ptr == ';' ? OK : INVALID_INPUT);
	++ptr;
	err_code = err_code ? err_code : (*ptr == '\0' ? OK : INVALID_INPUT);
	
	if (!err_code && arg1_tmp != NULL)
	{
		err_code = arg1_tmp[0] != '\0' ? OK : INVALID_INPUT;
	}
	if (!err_code && arg2_tmp != NULL)
	{
		err_code = arg2_tmp[0] != '\0' ? OK : INVALID_INPUT;
	}
	if (!err_code)
	{
		if (!strcmp(str_cmd, "Add"))
		{
			cmd_tmp = ADD;
			err_code = (arg_cnt_tmp == 2 || arg_cnt_tmp == 1) ? OK : INVALID_INPUT;
		}
		else if (!strcmp(str_cmd, "Sub"))
		{
			cmd_tmp = SUB;
			err_code = (arg_cnt_tmp == 2 || arg_cnt_tmp == 1) ? OK : INVALID_INPUT;
		}
		else if (!strcmp(str_cmd, "Mult"))
		{
			cmd_tmp = MULT;
			err_code = (arg_cnt_tmp == 2 || arg_cnt_tmp == 1) ? OK : INVALID_INPUT;
		}
		else if (!strcmp(str_cmd, "Div"))
		{
			cmd_tmp = DIV;
			err_code = (arg_cnt_tmp == 2 || arg_cnt_tmp == 1) ? OK : INVALID_INPUT;
		}
		else if (!strcmp(str_cmd, "Mod"))
		{
			cmd_tmp = MOD;
			err_code = (arg_cnt_tmp == 2 || arg_cnt_tmp == 1) ? OK : INVALID_INPUT;
		}
		else if (!strcmp(str_cmd, "Eval"))
		{
			cmd_tmp = EVAL;
			err_code = (arg_cnt_tmp == 1) ? OK : INVALID_INPUT;
		}
		else if (!strcmp(str_cmd, "Diff"))
		{
			cmd_tmp = DIFF;
			err_code = (arg_cnt_tmp == 1 || arg_cnt_tmp == 0) ? OK : INVALID_INPUT;
		}
		else if (!strcmp(str_cmd, "Cmps"))
		{
			cmd_tmp = CMPS;
			err_code = (arg_cnt_tmp == 2 || arg_cnt_tmp == 1) ? OK : INVALID_INPUT;
		}
		else
		{
			err_code = INVALID_INPUT;
		}
	}
	free(str_cmd);
	if (err_code)
	{
		free(arg1_tmp);
		free(arg2_tmp);
		return err_code;
	}
	*cmd = cmd_tmp;
	*arg_cnt = arg_cnt_tmp;
	*arg1 = arg1_tmp;
	*arg2 = arg2_tmp;
	return OK;
}