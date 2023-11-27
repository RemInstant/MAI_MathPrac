#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>

typedef long long ll;
typedef unsigned long long ull;

typedef enum
{
	OK,
	INVALID_ARG,
	INVALID_INPUT,
	INVALID_FLAG,
	INVALID_NUMBER,
	INVALID_BRACKET_ORDER,
	FILE_OPENING_ERROR,
	FILE_CONTENT_ERROR,
	OVERFLOW,
	UNINITIALIZED_USAGE,
	DIVISION_BY_ZERO,
	NEGATIVE_POWER,
	BAD_ALLOC,
	CORRUPTED_MEMORY
} status_codes;

void print_error(status_codes code)
{
	switch (code)
	{
		case OK:
			return;
		case INVALID_ARG:
			printf("Invalid function argument\n");
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
		case UNINITIALIZED_USAGE:
			printf("Uninitialized variable was used\n");
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

typedef enum
{
	NONE,
	CONST,
	OPERATION,
	L_BRACKET,
	R_BRACKET
} node_content;

typedef struct deque_node
{
	node_content content;
	void* data;
	struct deque_node* prev;
	struct deque_node* next;
} deque_node;

typedef struct
{
	deque_node* node;
} Deque_iter;

void iter_prev(Deque_iter* iter);
void iter_next(Deque_iter* iter);
int iter_equal(Deque_iter iter_l, Deque_iter iter_r);

typedef struct
{
	deque_node* begin;
	deque_node* end;
	ull size;
} Deque;

status_codes deque_set_null(Deque* deque);
status_codes deque_construct(Deque* deque);
status_codes deque_destruct(Deque* deque);
status_codes deque_empty(Deque* deque, int* empty);
status_codes deque_first(Deque deque, node_content* content, void** data);
status_codes deque_last(Deque deque, node_content* content, void** data);
status_codes deque_push_front(Deque* deque, node_content content, void* data);
status_codes deque_push_back(Deque* deque, node_content content, void* data);
status_codes deque_pop_front(Deque* deque, node_content* content, void** data);
status_codes deque_pop_back(Deque* deque, node_content* content, void** data);
status_codes deque_print(Deque deque);
Deque_iter deque_begin(Deque deque);
Deque_iter deque_end(Deque deque);

status_codes sread_until(const char* src, const char* delims, const char** end_ptr, char** str);
status_codes get_expr_token(const char* src, const char** end_ptr, node_content* content, void** data);
status_codes to_postfix_notation(const char* infix, Deque* postfix);
status_codes postfix_calc(Deque postfix, ll* res);

status_codes handle_file(FILE* file, ll* expr_cnt, char*** infixes, char*** postfixes, ll** res);
status_codes fread_line(FILE* file, char** str);
status_codes generate_error_file(const char* input, char** output);

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		//printf("Usage: cmd_path <input>\n");
		//return OK;
	}
	if (argc != 2)
	{
		//print_error(INVALID_INPUT);
		//return INVALID_INPUT;
	}
	
	FILE* input = fopen("lab4/t05_input", "r");
	if (input == NULL)
	{
		print_error(FILE_OPENING_ERROR);
		return FILE_OPENING_ERROR;
	}
	
	char* infix = NULL;
	status_codes err_code = fread_line(input, &infix);
	
	printf("%s\n", infix);
	Deque postfix;
	deque_set_null(&postfix);
	err_code = err_code ? err_code : to_postfix_notation(infix, &postfix);
	err_code = err_code ? err_code : deque_print(postfix);
	
	ll res = 0;
	err_code = err_code ? err_code : postfix_calc(postfix, &res);
	printf("\n%lld\n", res);
	
	fclose(input);
	deque_destruct(&postfix);
	if (err_code)
	{
		print_error(err_code);
	}
	return err_code;
}

void iter_prev(Deque_iter* iter)
{
	iter->node = iter->node->prev;
}

void iter_next(Deque_iter* iter)
{
	iter->node = iter->node->next;
}

int iter_equal(Deque_iter iter_l, Deque_iter iter_r)
{
	return iter_l.node == iter_r.node;
}

status_codes deque_set_null(Deque* deque)
{
	if (deque == NULL)
	{
		return INVALID_ARG;
	}
	deque->begin = deque->end = NULL;
	deque->size = 0;
	return OK;
}

status_codes deque_construct(Deque* deque)
{
	if (deque == NULL)
	{
		return INVALID_ARG;
	}
	deque_node* terminator = (deque_node*) malloc(sizeof(deque_node));
	if (terminator == NULL)
	{
		return BAD_ALLOC;
	}
	terminator->content = NONE;
	terminator->data = NULL;
	terminator->prev = terminator->next = terminator;
	deque->begin = deque->end = terminator;
	deque->size = 0;
	return OK;
}

status_codes deque_destruct(Deque* deque)
{
	if (deque == NULL)
	{
		return INVALID_ARG;
	}
	deque_node* cur = deque->begin;
	while (cur != deque->end)
	{
		deque_node* tmp = cur;
		cur = cur->next;
		free(tmp->data);
		free(tmp);
	}
	free(cur);
	deque->begin = deque->end = NULL;
	deque->size = 0;
	return OK;
}

status_codes deque_empty(Deque* deque, int* empty)
{
	if (deque == NULL || empty == NULL)
	{
		return INVALID_ARG;
	}
	*empty = deque->begin == deque->end;
	return OK;
}

status_codes deque_first(Deque deque, node_content* content, void** data)
{
	if (data== NULL)
	{
		return INVALID_ARG;
	}
	if (content != NULL)
	{
		*content = deque.begin->content;
	}
	if (data != NULL)
	{
		*data = deque.begin->data;
	}
	return OK;
}

status_codes deque_last(Deque deque, node_content* content, void** data)
{
	if (data== NULL)
	{
		return INVALID_ARG;
	}
	if (content != NULL)
	{
		*content = deque.end->prev->content;
	}
	if (data != NULL)
	{
		*data = deque.end->prev->data;
	}
	return OK;
}

status_codes deque_push_front(Deque* deque, node_content content, void* data)
{
	if (deque == NULL)
	{
		return INVALID_ARG;
	}
	deque_node* node = (deque_node*) malloc(sizeof(deque_node));
	if (node == NULL)
	{
		return BAD_ALLOC;
	}
	node->content = content;
	node->data = data;
	node->prev = deque->end;
	node->next = deque->begin;
	deque->begin = node;
	deque->size++;
	return OK;
}

status_codes deque_push_back(Deque* deque, node_content content, void* data)
{
	if (deque == NULL)
	{
		return INVALID_ARG;
	}
	deque_node* node = (deque_node*) malloc(sizeof(deque_node));
	if (node == NULL)
	{
		return BAD_ALLOC;
	}
	node->content = content;
	node->data = data;
	node->prev = deque->end->prev;
	node->next = deque->end;
	if (deque->size == 0)
	{
		deque->begin = node;
	}
	else
	{
		deque->end->prev->next = node;
	}
	deque->end->prev = node;
	deque->size++;
	return OK;
}

status_codes deque_pop_front(Deque* deque, node_content* content, void** data)
{
	if (deque == NULL)
	{
		return INVALID_ARG;
	}
	if (deque->begin == deque->end)
	{
		return INVALID_INPUT;
	}
	if (content != NULL)
	{
		*content = deque->begin->content;
	}
	if (data != NULL)
	{
		*data = deque->begin->data;
	}
	deque_node* tmp = deque->begin;
	deque->begin = deque->begin->next;
	deque->size--;
	free(tmp);
	return OK;
}

status_codes deque_pop_back(Deque* deque, node_content* content, void** data)
{
	if (deque == NULL)
	{
		return INVALID_ARG;
	}
	if (deque->begin == deque->end)
	{
		return INVALID_INPUT;
	}
	if (content != NULL)
	{
		*content = deque->end->prev->content;
	}
	if (data != NULL)
	{
		*data = deque->end->prev->data;
	}
	deque_node* tmp = deque->end->prev;
	if (tmp == deque->begin)
	{
		deque->begin = deque->end;
		deque->end->prev = deque->end;
	}
	else
	{
		deque->end->prev->prev->next = deque->end;
		deque->end->prev = deque->end->prev->prev;
	}
	deque->size--;
	free(tmp);
	return OK;
}

status_codes deque_print(Deque deque)
{
	if (deque.begin == NULL)
	{
		return INVALID_ARG;
	}
	Deque_iter iter = deque_begin(deque);
	Deque_iter iter_end = deque_end(deque);
	while (!iter_equal(iter, iter_end))
	{
		if (iter.node->content == CONST)
		{
			printf("%lld ", *((ll*) iter.node->data));
		}
		else if (iter.node->content == OPERATION)
		{
			printf("%c ", *((char*) iter.node->data));
		}
		else
		{
			return INVALID_INPUT;
		}
		iter_next(&iter);
	}
	return OK;
}

Deque_iter deque_begin(Deque deque)
{
	return (Deque_iter) { deque.begin };
}

Deque_iter deque_end(Deque deque)
{
	return (Deque_iter) { deque.end };
}

int is_operation(char operation)
{
	switch (operation)
	{
		case '^':
		case '*':
		case '/':
		case '%':
		case '+':
		case '-':
			return 1;
		default:
			return 0;
	}
}

int get_operation_priority(char operation)
{
	switch (operation)
	{
		case '^':
			return 3;
		case '*':
		case '/':
		case '%':
			return 2;
		case '+':
		case '-':
			return 1;
		default:
			return 0;
	}
}

status_codes str_to_ll(const char* str, ll* res)
{
	if (str == NULL || res == NULL)
	{
		return INVALID_ARG;
	}
	if (str[0] == '\0' || ((str[0] == '+' || str[0] == '-') && str[1] == '\0'))
	{
		return INVALID_INPUT;
	}
	errno = 0;
	char* ptr;
	ll res_tmp = strtoll(str, &ptr, 10);
	if (*ptr != '\0')
	{
		return INVALID_INPUT;
	}
	if (errno == ERANGE)
	{
		return OVERFLOW;
	}
	*res = res_tmp;
	return OK;
}

status_codes sread_until(const char* src, const char* delims, const char** end_ptr, char** str)
{
	if (src == NULL || str == NULL)
	{
		return INVALID_ARG;
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

status_codes get_expr_token(const char* src, const char** end_ptr, node_content* content, void** data)
{
	if (src == NULL || end_ptr == NULL || content == NULL || data == NULL)
	{
		return INVALID_ARG;
	}
	status_codes err_code = OK;
	if (*src == '(')
	{
		*content = L_BRACKET;
		*data = NULL;
	}
	else if (*src == ')')
	{
		*content = R_BRACKET;
		*data = NULL;
	}
	else if (is_operation(*src))
	{
		char* data_tmp = (char*) malloc(sizeof(char));
		if (data_tmp == NULL)
		{
			return BAD_ALLOC;
		}
		data_tmp[0] = *src;
		*content = OPERATION;
		*data = data_tmp;
	}
	else if (isdigit(*src))
	{
		char* str_data = NULL;
		ll* data_tmp = (ll*) malloc(sizeof(ll));
		err_code = data_tmp != NULL ? OK : BAD_ALLOC;
		err_code = sread_until(src, "()+-*/%^", end_ptr, &str_data);
		err_code = err_code ? err_code : str_to_ll(str_data, data_tmp);
		free(str_data);
		if (err_code)
		{
			free(data_tmp);
			return err_code;
		}
		*content = CONST;
		*data = data_tmp;
		return OK;
	}
	else
	{
		err_code = INVALID_INPUT;
	}
	if (err_code)
	{
		return err_code;
	}
	++src;
	*end_ptr = src;
	return OK;
}

status_codes validate_token_combination(node_content prev, node_content cur)
{
	if (prev == OPERATION && cur == OPERATION)
	{
		return INVALID_INPUT;
	}
	if (prev == CONST && cur == CONST)
	{
		return INVALID_INPUT;
	}
	if ((prev == L_BRACKET && cur == R_BRACKET) || (prev == R_BRACKET && cur == L_BRACKET))
	{
		return INVALID_INPUT;
	}
	if ((prev == OPERATION && cur == R_BRACKET) || (prev == L_BRACKET && cur == OPERATION))
	{
		return INVALID_INPUT;
	}
	if ((prev == CONST && cur == L_BRACKET) || (prev == R_BRACKET && cur == CONST))
	{
		return INVALID_INPUT;
	}
	return OK;
}

status_codes to_postfix_notation(const char* infix, Deque* postfix)
{
	if (infix == NULL || postfix == NULL)
	{
		return INVALID_ARG;
	}
	
	status_codes err_code = OK;
	Deque postfix_tmp, opers;
	deque_set_null(&postfix_tmp);
	deque_set_null(&opers);
	err_code = err_code ? err_code : deque_construct(&postfix_tmp);
	err_code = err_code ? err_code : deque_construct(&opers);
	
	node_content prev_content = NONE;
	const char* ptr = infix;
	while (!err_code && *ptr)
	{
		node_content content;
		void* data;
		err_code = get_expr_token(ptr, &ptr, &content, &data);
		err_code = err_code ? err_code : validate_token_combination(prev_content, content);
		// --- HANDLE NUMBER ---
		if (!err_code && content == CONST)
		{
			err_code = deque_push_back(&postfix_tmp, CONST, data);
		}
		// --- HANDLE OPERATION ---
		else if (!err_code && content == OPERATION)
		{
			void* other_data = NULL;
			deque_last(opers, NULL, &other_data);
			int cur_prior = get_operation_priority(*((char*) data));
			int other_prior = other_data != NULL ? get_operation_priority(*((char*) other_data)) : -1;
			while (!err_code && other_prior >= cur_prior)
			{
				err_code = deque_push_back(&postfix_tmp, OPERATION, other_data);
				err_code = err_code ? err_code : deque_pop_back(&opers, NULL, NULL);
				deque_last(opers, NULL, &other_data);
				other_prior = other_data != NULL ? get_operation_priority(*((char*) other_data)) : -1;
			}
			err_code = err_code ? err_code : deque_push_back(&opers, OPERATION, data);
		}
		// --- HANDLE LEFT BRACKET ---
		else if (!err_code && content == L_BRACKET)
		{
			err_code = deque_push_back(&opers, L_BRACKET, NULL);
		}
		// --- HANDLE RIGHT BRACKET ---
		else if (!err_code && content == R_BRACKET)
		{
			node_content other_content = NONE;
			void* other_data = NULL;
			deque_last(opers, &other_content, &other_data);
			err_code = other_content != NONE ? OK : INVALID_BRACKET_ORDER;
			while (!err_code && other_content != L_BRACKET)
			{
				err_code = deque_push_back(&postfix_tmp, OPERATION, other_data);
				err_code = err_code ? err_code : deque_pop_back(&opers, NULL, NULL);
				deque_last(opers, &other_content, &other_data);
				err_code = err_code ? err_code : (other_content != NONE ? OK : INVALID_BRACKET_ORDER);
			}
			other_data = NULL;
			err_code = err_code ? err_code : deque_pop_back(&opers, NULL, &other_data);
			free(other_data);
		}
		prev_content = content;
	}
	while (!err_code && opers.size > 0)
	{
		node_content content = NONE;
		void* data = NULL;
		err_code = deque_pop_back(&opers, &content, &data);
		err_code = err_code ? err_code : (content != L_BRACKET ? OK : INVALID_BRACKET_ORDER);
		err_code = err_code ? err_code : deque_push_back(&postfix_tmp, OPERATION, data);
	}
	deque_destruct(&opers);
	if (err_code)
	{
		deque_destruct(&postfix_tmp);
		return err_code;
	}
	*postfix = postfix_tmp;
	return OK;
}

status_codes bpow(ll base, ll pow, ll* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	if (pow < 0)
	{
		return NEGATIVE_POWER;
	}
	ll res_tmp = 1;
	ll mult = base;
	while (pow > 0)
	{
		if (pow & 1)
		{
			if ((res_tmp > LLONG_MAX / mult) || (res_tmp < LLONG_MIN / mult))
			{
				return OVERFLOW;
			}
			res_tmp *= mult;
		}
		mult *= base;
		pow >>= 1;
	}
	*res = res_tmp;
	return OK;
}

status_codes calc_operation(char operation, ll arg1, ll arg2, ll* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	switch (operation)
	{
		case '+':
		{
			if ((arg1 > LLONG_MAX - arg2) || (arg1 + arg2 < LLONG_MIN))
			{
				return OVERFLOW;
			}
			*res = arg1 + arg2;
			return OK;
		}
		case '-':
		{
			if ((arg1 - arg2 > LLONG_MAX) || (arg1 < LLONG_MIN + arg2))
			{
				return OVERFLOW;
			}
			*res = arg1 - arg2;
			return OK;
		}
		case '*':
		{
			if ((arg1 > LLONG_MAX / arg2) || (arg1 < LLONG_MIN / arg2))
			{
				return OVERFLOW;
			}
			*res = arg1 * arg2;
			return OK;
		}
		case '/':
		{
			if (arg2 == 0)
			{
				return DIVISION_BY_ZERO;
			}
			*res = arg1 / arg2;
			return OK;
		}
		case '%':
		{
			if (arg2 == 0)
			{
				return DIVISION_BY_ZERO;
			}
			*res = arg1 % arg2;
			return OK;
		}
		case '^':
			return bpow(arg1, arg2, res);
		default:
			return INVALID_INPUT;
	}
}

status_codes postfix_calc(Deque postfix, ll* res)
{
	if (postfix.begin == NULL || res == NULL)
	{
		return INVALID_ARG;
	}
	
	Deque args;
	deque_set_null(&args);
	status_codes err_code = deque_construct(&args);
	
	Deque_iter iter = deque_begin(postfix);
	Deque_iter iter_end = deque_end(postfix);
	
	while (!err_code && !iter_equal(iter, iter_end))
	{
		if (iter.node->content == CONST)
		{
			ll* data = (ll*) malloc(sizeof(ll));
			err_code = data != NULL ? OK : BAD_ALLOC;
			if (!err_code)
			{
				*data = *((ll*) iter.node->data);
				err_code = deque_push_back(&args, CONST, data);
			}
		}
		else if (iter.node->content == OPERATION)
		{
			char* operation = (char*) iter.node->data;
			void* arg1 = NULL;
			void* arg2 = NULL;
			ll* local_res = (ll*) malloc(sizeof(ll));
			err_code = res != NULL ? OK : BAD_ALLOC;
			err_code = err_code ? err_code : deque_pop_back(&args, NULL, &arg1);
			err_code = err_code ? err_code : deque_pop_back(&args, NULL, &arg2);
			err_code = err_code ? err_code : calc_operation(*operation, *((ll*) arg1), *((ll*) arg2), local_res);
			err_code = err_code ? err_code : deque_push_back(&args, CONST, local_res);
			free(arg1);
			free(arg2);
		}
		else
		{
			err_code = INVALID_INPUT;
		}
		iter_next(&iter);
	}
	
	void *res_tmp = NULL;
	if (!err_code)
	{
		err_code = args.size == 1 ? OK : INVALID_INPUT;
		err_code = err_code ? err_code : deque_pop_back(&args, NULL, &res_tmp);
	}
	deque_destruct(&args);
	if (err_code)
	{
		return err_code;
	}
	*res = *((ll*) res_tmp);
	free(res_tmp);
	return OK;
}

status_codes fread_line(FILE* file, char** line)
{
	if (file == NULL || line == NULL)
	{
		return INVALID_ARG;
	}
	ull iter = 0;
	ull size = 2;
	*line = (char*) malloc(sizeof(char) * size);
	if (*line == NULL)
	{
		return BAD_ALLOC;
	}
	char ch = getc(file);
	while (!feof(file))
	{
		if (iter > size - 2)
		{
			size *= 2;
			char* temp_line = realloc(*line, size);
			if (temp_line == NULL)
			{
				free(*line);
				return BAD_ALLOC;
			}
			*line = temp_line;
		}
		(*line)[iter++] = ch;
		ch = getc(file);
	}
	(*line)[iter] = '\0';
	return OK;
}

status_codes generate_random_str(char** str)
{
	if (str == NULL)
	{
		return INVALID_ARG;
	}
	ull iter = 0;
	ull size = 2;
	*str = (char*) malloc(sizeof(char) * size);
	if (*str == NULL)
	{
		return BAD_ALLOC;
	}
	char symbols[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	
	char ch = symbols[rand() % 63];
	while ((ch != '\0' || iter == 0) && iter < 63)
	{
		while (iter == 0 && ch == '\0')
		{
			ch = symbols[rand() % 63];
		}
		if (iter > size - 2)
		{
			size *= 2;
			char* temp_line = realloc(*str, size);
			if (temp_line == NULL)
			{
				free(*str);
				return BAD_ALLOC;
			}
			*str = temp_line;
		}
		(*str)[iter++] = ch;
		ch = symbols[rand() % 63];
	}
	(*str)[iter] = '\0';
	return OK;
}

status_codes construct_output_path(const char* input, const char* output_name, char** output)
{
	if (input == NULL || output_name == NULL || output == NULL)
	{
		return INVALID_ARG;
	}
	ull last_delim_pos = 0;
	for (ull i = 0; input[i]; ++i)
	{
		if (input[i] == '/' || input[i] == '\\')
		{
			last_delim_pos = i;
		}
	}
	*output = (char*) malloc(sizeof(char) * (last_delim_pos + strlen(output_name) + 2));
	if (*output == NULL)
	{
		return BAD_ALLOC;
	}
	memcpy(*output, input, sizeof(char) * (last_delim_pos + 1));
	strcpy(*output + last_delim_pos + 1, output_name);
	return OK;
}
