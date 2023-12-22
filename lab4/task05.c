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
	FILE_END,
	OVERFLOW,
	UNINITIALIZED_USAGE,
	DIVISION_BY_ZERO,
	NEGATIVE_POWER,
	ZERO_POWERED_ZERO,
	BAD_ALLOC,
	CORRUPTED_MEMORY
} status_code;

void print_error(status_code code);
void fprint_error(FILE* file, status_code code);

typedef enum
{
	NONE,
	CONST,
	OPERATION,
	L_BRACKET,
	R_BRACKET
} node_content;

typedef struct node
{
	node_content content;
	void* data;
	struct node* next;
} node;

typedef struct
{
	node* top;
	ull size;
} Stack;

status_code stack_set_null(Stack* stack);
status_code stack_destruct(Stack* stack);
status_code stack_top(Stack stack, node_content* content, void** data);
status_code stack_push(Stack* stack, node_content content, void* data);
status_code stack_pop(Stack* stack, node_content* content, void** data);
status_code stack_print(Stack stack);

status_code sread_until(const char* src, const char* delims, const char** end_ptr, char** str);
status_code get_expr_const(const char* src, const char** end_ptr, void** data);
status_code get_expr_oper(const char* src, const char** end_ptr, node_content* content, void** data);
status_code to_postfix_notation(const char* infix, Stack* postfix);
status_code calc_operation(char operation, ll arg1, ll arg2, ll* res);
status_code postfix_calc(Stack postfix, ll* res);

typedef struct
{
	status_code valid_code;
	char* infix;
	Stack postfix;
	ll res;
} expr_data;

status_code handle_file(FILE* file, ull* expr_cnt, expr_data** data_arr);
status_code fread_line(FILE* file, char** str);
status_code generate_error_path(const char* input, char** output);

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
	
	char* path = NULL;
	status_code err_code = fread_line(input, &path);
	if (err_code == FILE_END)
	{
		printf("There is no pathes in the input file\n");
	}
	while (!err_code)
	{
		status_code file_code = OK;
		FILE* file = fopen(path, "r");
		if (file == NULL)
		{
			file_code = FILE_OPENING_ERROR;
		}
		ull cnt = 0;
		expr_data* data = NULL;
		file_code = file_code ? file_code : handle_file(file, &cnt, &data);
		printf("FILE %s:\n", path);
		if (!file_code)
		{
			status_code err_file_code = OK;
			char* error_path = NULL;
			FILE* err_file = NULL;
			if (cnt == 0)
			{
				printf("There are no expressions in the file\n");
			}
			for (ull i = 0; i < cnt && !file_code; ++i)
			{
				if (data[i].valid_code)
				{
					if (err_file == NULL && err_file_code == OK)
					{
						err_file_code = generate_error_path(path, &error_path);
						if (!err_file_code)
						{
							err_file = fopen(error_path, "w");
						}
						if (!err_file_code && err_file == NULL)
						{
							err_file_code = FILE_OPENING_ERROR;
						}
					}
					if (!err_file_code)
					{
						fprintf(err_file, "%llu: %s - ", i, data[i].infix);
						fprint_error(err_file, data[i].valid_code);
					}
				}
				else
				{
					printf("%llu: %s -> ", i, data[i].infix);
					file_code = stack_print(data[i].postfix);
					printf("-> %lld\n", data[i].res);
				}
			}
			if (!err_file_code && err_file != NULL)
			{
				printf("There are errors in this file which are printed into the error file: %s\n", error_path);
			}
			if (err_file_code)
			{
				printf("An error occurred while creating/opening error file: ");
				print_error(err_file_code);
			}
			for (ull i = 0; i < cnt; ++i)
			{
				free(data[i].infix);
				stack_destruct(&data[i].postfix);
			}
			if (file != NULL)
			{
				fclose(file);
			}
			if (err_file != NULL)
			{
				fclose(err_file);
			}
			free(data);
			free(error_path);
		}
		if (file_code)
		{
			print_error(file_code);
		}
		free(path);
		err_code = err_code ? err_code : fread_line(input, &path);
		printf("\n");
	}
	if (err_code == FILE_END)
	{
		err_code = OK;
	}
	if (input != NULL)
	{
		fclose(input);
	}
	if (err_code)
	{
		print_error(err_code);
	}
	return err_code;
}

status_code stack_set_null(Stack* stack)
{
	if (stack == NULL)
	{
		return INVALID_ARG;
	}
	stack->top = NULL;
	stack->size = 0;
	return OK;
}

status_code stack_destruct(Stack* stack)
{
	if (stack == NULL)
	{
		return INVALID_ARG;
	}
	node* cur = stack->top;
	while (cur != NULL)
	{
		node* tmp = cur;
		cur = cur->next;
		free(tmp->data);
		free(tmp);
	}
	stack->top = NULL;
	stack->size = 0;
	return OK;
}

status_code stack_top(Stack stack, node_content* content, void** data)
{
	node_content tmp_content = NONE;
	void* tmp_data = NULL;
	if (stack.top != NULL)
	{
		tmp_content = stack.top->content;
		tmp_data = stack.top->data;
	}
	if (content != NULL)
	{
		*content = tmp_content;
	}
	if (data != NULL)
	{
		*data = tmp_data;
	}
	return OK;
}

status_code stack_push(Stack* stack, node_content content, void* data)
{
	if (stack == NULL)
	{
		return INVALID_ARG;
	}
	node* new_node = (node*) malloc(sizeof(node));
	if (new_node == NULL)
	{
		return BAD_ALLOC;
	}
	new_node->content = content;
	new_node->data = data;
	new_node->next = stack->top;
	stack->top = new_node;
	stack->size++;
	return OK;
}

status_code stack_pop(Stack* stack, node_content* content, void** data)
{
	if (stack == NULL)
	{
		return INVALID_ARG;
	}
	if (stack->top == NULL)
	{
		return INVALID_INPUT;
	}
	if (content != NULL)
	{
		*content = stack->top->content;
	}
	if (data != NULL)
	{
		*data = stack->top->data;
	}
	node* tmp = stack->top;
	stack->top = stack->top->next;
	stack->size--;
	free(tmp);
	return OK;
}

status_code stack_print(Stack stack)
{
	node* cur = stack.top;
	while (cur != NULL)
	{
		if (cur->content == CONST)
		{
			printf("%lld ", *((ll*) cur->data));
		}
		else if (cur->content == OPERATION)
		{
			printf("%c ", *((char*) cur->data));
		}
		else
		{
			return INVALID_INPUT;
		}
		cur = cur->next;
	}
	return OK;
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

int get_op_prior(char operation)
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

status_code str_to_ll(const char* str, ll* res)
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

status_code sread_until(const char* src, const char* delims, const char** end_ptr, char** str)
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

status_code get_expr_const(const char* src, const char** end_ptr, void** data)
{
	if (src == NULL || end_ptr == NULL || data == NULL)
	{
		return INVALID_ARG;
	}
	status_code err_code = OK;
	char* str_data = NULL;
	ll* data_tmp = (ll*) malloc(sizeof(ll));
	int sign = 1;
	err_code = data_tmp != NULL ? OK : BAD_ALLOC;
	if (*src == '+' || *src == '-')
	{
		sign = *src == '-' ? -1 : 1;
		++src;
	}
	err_code = err_code ? err_code : sread_until(src, "()+-*/%^", end_ptr, &str_data);
	err_code = err_code ? err_code : str_to_ll(str_data, data_tmp);
	free(str_data);
	if (err_code)
	{
		free(data_tmp);
		return err_code;
	}
	*data_tmp *= sign;
	*data = data_tmp;
	return OK;
}

status_code get_expr_oper(const char* src, const char** end_ptr, node_content* content, void** data)
{
	if (src == NULL || end_ptr == NULL || content == NULL || data == NULL)
	{
		return INVALID_ARG;
	}
	status_code err_code = OK;
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

status_code validate_token_combination(node_content prev, node_content cur)
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

status_code to_postfix_notation(const char* infix, Stack* postfix)
{
	if (infix == NULL || postfix == NULL)
	{
		return INVALID_ARG;
	}
	
	status_code err_code = OK;
	Stack postfix_tmp, opers;
	stack_set_null(&postfix_tmp);
	stack_set_null(&opers);
	stack_set_null(postfix);
	
	node_content prev_content = NONE;
	const char* ptr = infix;
	while (!err_code && *ptr)
	{
		node_content content;
		void* data;
		if (prev_content != CONST && prev_content != R_BRACKET && *ptr != '(')
		{
			err_code = get_expr_const(ptr, &ptr, &data);
			content = CONST;
		}
		else
		{
			err_code = get_expr_oper(ptr, &ptr, &content, &data);
		}
		err_code = err_code ? err_code : validate_token_combination(prev_content, content);
		// --- HANDLE NUMBER ---
		if (!err_code && content == CONST)
		{
			err_code = stack_push(&postfix_tmp, CONST, data);
		}
		// --- HANDLE OPERATION ---
		else if (!err_code && content == OPERATION)
		{
			void* other_data = NULL;
			stack_top(opers, NULL, &other_data);
			int cur_prior = get_op_prior(*((char*) data));
			int other_prior = other_data != NULL ? get_op_prior(*((char*) other_data)) : -1;
			while (!err_code && other_prior >= cur_prior)
			{
				err_code = stack_push(&postfix_tmp, OPERATION, other_data);
				err_code = err_code ? err_code : stack_pop(&opers, NULL, NULL);
				stack_top(opers, NULL, &other_data);
				other_prior = other_data != NULL ? get_op_prior(*((char*) other_data)) : -1;
			}
			err_code = err_code ? err_code : stack_push(&opers, OPERATION, data);
		}
		// --- HANDLE LEFT BRACKET ---
		else if (!err_code && content == L_BRACKET)
		{
			err_code = stack_push(&opers, L_BRACKET, NULL);
		}
		// --- HANDLE RIGHT BRACKET ---
		else if (!err_code && content == R_BRACKET)
		{
			node_content other_content = NONE;
			void* other_data = NULL;
			stack_top(opers, &other_content, &other_data);
			err_code = other_content != NONE ? OK : INVALID_BRACKET_ORDER;
			while (!err_code && other_content != L_BRACKET)
			{
				err_code = stack_push(&postfix_tmp, OPERATION, other_data);
				err_code = err_code ? err_code : stack_pop(&opers, NULL, NULL);
				stack_top(opers, &other_content, &other_data);
				err_code = err_code ? err_code : (other_content != NONE ? OK : INVALID_BRACKET_ORDER);
			}
			other_data = NULL;
			err_code = err_code ? err_code : stack_pop(&opers, NULL, &other_data);
			free(other_data);
		}
		prev_content = content;
	}
	while (!err_code && opers.size > 0)
	{
		node_content content = NONE;
		void* data = NULL;
		err_code = stack_pop(&opers, &content, &data);
		err_code = err_code ? err_code : (content != L_BRACKET ? OK : INVALID_BRACKET_ORDER);
		err_code = err_code ? err_code : stack_push(&postfix_tmp, OPERATION, data);
	}
	stack_destruct(&opers);
	while (!err_code && postfix_tmp.size > 0)
	{
		node_content tmp_content;
		void* tmp_data;
		err_code = stack_pop(&postfix_tmp, &tmp_content, &tmp_data);
		err_code = err_code ? err_code : stack_push(postfix, tmp_content, tmp_data);
	}
	stack_destruct(&postfix_tmp);
	if (err_code)
	{
		stack_destruct(postfix);
		return err_code;
	}
	return OK;
}

status_code bpow(ll base, ll pow, ll* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	if (pow < 0)
	{
		return NEGATIVE_POWER;
	}
	if (base == 0 && pow == 0)
	{
		return ZERO_POWERED_ZERO;
	}
	ll res_tmp = 1;
	ll mult = base;
	while (pow > 0)
	{
		if (pow & 1)
		{
			ll check = mult < 0 ? -mult : mult;
			if ((res_tmp > LLONG_MAX / check) || (res_tmp < LLONG_MIN / check))
			{
				return OVERFLOW;
			}
			res_tmp *= mult;
		}
		mult *= mult;
		pow >>= 1;
	}
	*res = res_tmp;
	return OK;
}

status_code calc_operation(char operation, ll arg1, ll arg2, ll* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	if (arg2 < 0 && (operation == '+' || operation == '-'))
	{
		operation = operation == '+' ? '-' : '+';
		arg2 *= -1;
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
			ll check = arg2 < 0 ? -arg2 : arg2;
			if ((arg1 > LLONG_MAX / check) || (arg1 < LLONG_MIN / check))
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

status_code postfix_calc(Stack postfix, ll* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	
	Stack args;
	stack_set_null(&args);
	
	status_code err_code = OK;
	node* cur = postfix.top;
	while (!err_code && cur != NULL)
	{
		if (cur->content == CONST)
		{
			ll* data = (ll*) malloc(sizeof(ll));
			err_code = data != NULL ? OK : BAD_ALLOC;
			if (!err_code)
			{
				*data = *((ll*) cur->data);
				err_code = stack_push(&args, CONST, data);
			}
		}
		else if (cur->content == OPERATION)
		{
			char* operation = (char*) cur->data;
			void* arg1 = NULL;
			void* arg2 = NULL;
			ll* local_res = (ll*) malloc(sizeof(ll));
			err_code = res != NULL ? OK : BAD_ALLOC;
			err_code = err_code ? err_code : stack_pop(&args, NULL, &arg2);
			err_code = err_code ? err_code : stack_pop(&args, NULL, &arg1);
			err_code = err_code ? err_code : calc_operation(*operation, *((ll*) arg1), *((ll*) arg2), local_res);
			err_code = err_code ? err_code : stack_push(&args, CONST, local_res);
			if (err_code)
			{
				free(local_res);
			}
			free(arg1);
			free(arg2);
		}
		else
		{
			err_code = INVALID_INPUT;
		}
		cur = cur->next;
	}
	
	void *res_tmp = NULL;
	if (!err_code)
	{
		err_code = args.size == 1 ? OK : INVALID_INPUT;
		err_code = err_code ? err_code : stack_pop(&args, NULL, &res_tmp);
	}
	stack_destruct(&args);
	if (err_code)
	{
		return err_code;
	}
	*res = *((ll*) res_tmp);
	free(res_tmp);
	return OK;
}

status_code handle_file(FILE* file, ull* expr_cnt, expr_data** data_arr)
{
	if (file == NULL || expr_cnt == NULL || data_arr == NULL)
	{
		return INVALID_ARG;
	}
	
	ull cnt = 0;
	ull size = 2;
	expr_data* data_arr_tmp = (expr_data*) malloc(sizeof(expr_data) * 2);
	
	char* infix = NULL;
	status_code err_code = fread_line(file, &infix);
	if (err_code == FILE_END)
	{
		free(data_arr_tmp);
		*expr_cnt = 0;
		*data_arr = NULL;
		return OK;
	}
	while (!err_code)
	{
		Stack postfix;
		ll res = 0;
		stack_set_null(&postfix);
		status_code expr_code = to_postfix_notation(infix, &postfix);
		expr_code = expr_code ? expr_code : postfix_calc(postfix, &res);
		if (expr_code)
		{
			stack_destruct(&postfix);
		}
		expr_data data;
		data.valid_code = expr_code;
		data.infix = infix;
		data.postfix = postfix;
		data.res = res;
		if (cnt == size)
		{
			size *= 2;
			expr_data* tmp = (expr_data*) realloc(data_arr_tmp, sizeof(expr_data) * size);
			if (tmp == NULL)
			{
				err_code = BAD_ALLOC;
				free(infix);
				stack_destruct(&postfix);
			}
			else
			{
				data_arr_tmp = tmp;
			}
		}
		if (!err_code)
		{
			data_arr_tmp[cnt++] = data;
		}
		err_code = err_code ? err_code : fread_line(file, &infix);
	}
	if (err_code == FILE_END)
	{
		err_code = OK;
	}
	if (!err_code)
	{
		expr_data* tmp = (expr_data*) realloc(data_arr_tmp, sizeof(expr_data) * cnt);
		if (tmp == NULL)
		{
			err_code = BAD_ALLOC;
		}
		else
		{
			data_arr_tmp = tmp;
		}
	}
	if (err_code)
	{
		for (ull i = 0; i < cnt; ++i)
		{
			free(data_arr_tmp[0].infix);
			stack_destruct(&data_arr_tmp[0].postfix);
		}
		free(data_arr_tmp);
		return err_code;
	}
	*expr_cnt = cnt;
	*data_arr = data_arr_tmp;
	return OK;
}

status_code fread_line(FILE* file, char** line)
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
	if (feof(file))
	{
		free(*line);
		*line = NULL;
		return FILE_END;
	}
	while (!feof(file) && ch != '\n')
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

status_code generate_error_path(const char* input, char** output)
{
	if (input == NULL || output == NULL)
	{
		return INVALID_ARG;
	}
	
	char* output_tmp = (char*) malloc(sizeof(char) * (strlen(input) + 5));
	if (output_tmp == NULL)
	{
		return BAD_ALLOC;
	}
	
	ll dot_pos = -1;
	for (ull i = 0; input[i]; ++i)
	{
		if (input[i] == '.')
		{
			dot_pos = i;
		}
		if (input[i] == '/' || input[i] == '\\')
		{
			dot_pos = -1;
		}
	}
	if (dot_pos != -1)
	{
		sprintf(output_tmp, "%s", input);
		sprintf(output_tmp + dot_pos, "_err");
	}
	else
	{
		sprintf(output_tmp, "%s_err", input);
	}
	*output = output_tmp;
	return OK;
}

void print_error(status_code code)
{
	fprint_error(stdin, code);
}

void fprint_error(FILE* file, status_code code)
{
	switch (code)
	{
		case OK:
			return;
		case INVALID_ARG:
			fprintf(file, "Invalid function argument\n");
			return;
		case INVALID_INPUT:
			fprintf(file, "Invalid input\n");
			return;
		case INVALID_FLAG:
			fprintf(file, "Invalid flag\n");
			return;
		case INVALID_NUMBER:
			fprintf(file, "Invalid number\n");
			return;
		case INVALID_BRACKET_ORDER:
			fprintf(file, "Invalid bracket order\n");
			return;
		case FILE_OPENING_ERROR:
			fprintf(file, "File cannot be opened\n");
			return;
		case FILE_CONTENT_ERROR:
			fprintf(file, "Invalid content of file\n");
			return;
		case FILE_END:
			fprintf(file, "Unexpected end of file\n");
			return;
		case OVERFLOW:
			fprintf(file, "Tried to overflow\n");
			return;
		case UNINITIALIZED_USAGE:
			fprintf(file, "Uninitialized variable was used\n");
			return;
		case DIVISION_BY_ZERO:
			fprintf(file, "Tried to divide by zero\n");
			return;
		case ZERO_POWERED_ZERO:
			fprintf(file, "Tried to raise zero into power zero\n");
			return;
		case BAD_ALLOC:
			fprintf(file, "Memory lack error occurred\n");
			return;
		case CORRUPTED_MEMORY:
			fprintf(file, "Memory was corrupted\n");
			return;
		default:
			fprintf(file, "Unexpected error occurred\n");
			return;
	}
}
