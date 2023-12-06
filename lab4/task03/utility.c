#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <math.h>

typedef long long ll;
typedef unsigned long long ull;

void print_error(status_code code)
{
	fprint_error(stdout, code);
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
		case INVALID_EPSILON:
			fprintf(file, "Invalid epsilon\n");
			return;
		case INVALID_BRACKET_ORDER:
			fprintf(file, "Invalid bracket order\n");
			return;
		case FILE_OPENING_ERROR:
			fprintf(file, "File cannot be opened\n");
			return;
		case FILE_INVALID_CONTENT:
			fprintf(file, "File content is invalid\n");
			return;
		case FILE_END:
			fprintf(file, "Unexpected end of file was found\n");
			return;
		case OVERFLOW:
			fprintf(file, "Attempting to overflow\n");
			return;
		case UNINITIALIZED_USAGE:
			fprintf(file, "Attempting to access uninitialized variable\n");
			return;
		case DIVISION_BY_ZERO:
			fprintf(file, "Attempting to divide by zero\n");
			return;
		case ZERO_POWERED_ZERO:
			fprintf(file, "Attempting to raize zero in the power zero\n");
			return;
		case BAD_ALLOC:
			fprintf(file, "Memory lack occurred\n");
			return;
		case BAD_ACCESS:
			fprintf(file, "Attempting to access incorrect memory\n");
			return;
		default:
			fprintf(file, "Unexpected error occurred\n");
			return;
	}
}


int pair_str_double_comparator(const void* ptr_1, const void* ptr_2)
{
	const pair_str_double* pair_1 = (const pair_str_double*) ptr_1;
	const pair_str_double* pair_2 = (const pair_str_double*) ptr_2;
	return strcmp(pair_1->str, pair_2->str);
}


status_code fread_line(FILE* file, char** line)
{
	if (file == NULL || line == NULL)
	{
		return INVALID_ARG;
	}
	ull iter = 0;
	ull size = 4;
	char* line_tmp = (char*) malloc(sizeof(char) * size);
	if (line_tmp == NULL)
	{
		return BAD_ALLOC;
	}
	char ch = getc(file);
	if (feof(file))
	{
		free(line_tmp);
		return FILE_END;
	}
	while (!feof(file) && ch != '\n')
	{
		if (iter + 1 == size)
		{
			size *= 2;
			char* tmp = realloc(line_tmp, size);
			if (tmp == NULL)
			{
				free(line_tmp);
				return BAD_ALLOC;
			}
			line_tmp = tmp;
		}
		line_tmp[iter++] = ch;
		ch = getc(file);
	}
	line_tmp[iter] = '\0';
	*line = line_tmp;
	return OK;
}

status_code read_line(char** str)
{
	return fread_line(stdin, str);
}

status_code sread_until(const char* src, const char* delims, int inclusive_flag, const char** end_ptr, char** str)
{
	if (src == NULL || delims == NULL || str == NULL)
	{
		return INVALID_ARG;
	}
	ull cnt = 0;
	ull size = 4;
	char* str_tmp = (char*) malloc(sizeof(char) * size);
	if (str_tmp == NULL)
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
		if (cnt + 2 == size)
		{
			size *= 2;
			char* tmp = realloc(str_tmp, sizeof(char) * size);
			if (tmp == NULL)
			{
				free(str_tmp);
				return BAD_ALLOC;
			}
			str_tmp = tmp;
		}
		str_tmp[cnt++] = *ptr;
		++ptr;
	}
	if (end_ptr != NULL)
	{
		*end_ptr = ptr;
	}
	if (inclusive_flag && *ptr != '\0')
	{
		str_tmp[cnt++] = *ptr;
	}
	str_tmp[cnt] = '\0';
	*str = str_tmp;
	return OK;
}


status_code skip_multi_line_comment(FILE* file)
{
	if (file == NULL)
	{
		return INVALID_ARG;
	}
	char ch = '[';
	int comment_nesting = 1;
	while (comment_nesting > 0 && !feof(file))
	{
		ch = getc(file);
		if (ch == '[')
		{
			++comment_nesting;
		}
		else if (ch == ']')
		{
			--comment_nesting;
		}
	}
	if (comment_nesting > 0)
	{
		return FILE_INVALID_CONTENT;
	}
	return OK;
}

status_code fread_cmd(FILE* file, char** str)
{
	if (file == NULL || str == NULL)
	{
		return INVALID_INPUT;
	}
	ull iter = 0;
	ull size = 4;
	char* str_tmp = (char*) malloc(sizeof(char) * size);
	if (str_tmp == NULL)
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
			status_code err_code = skip_multi_line_comment(file);
			if (err_code)
			{
				free(str_tmp);
				return err_code;
			}
			ch = getc(file);
			skip_flag = 1;
		}
	}
	if (feof(file))
	{
		free(str_tmp);
		return FILE_END;
	}
	while (ch != ';' && !feof(file))
	{
		if (iter + 2 == size)
		{
			size *= 2;
			char* tmp = realloc(str_tmp, sizeof(char) * size);
			if (tmp == NULL)
			{
				free(str_tmp);
				return BAD_ALLOC;
			}
			str_tmp = tmp;
		}
		str_tmp[iter++] = ch;
		ch = getc(file);
		if (ch == '[')
		{
			status_code err_code = skip_multi_line_comment(file);
			if (err_code)
			{
				free(str_tmp);
				return err_code;
			}
			ch = getc(file);
		}
	}
	if (ch != ';')
	{
		free(str_tmp);
		return FILE_INVALID_CONTENT;
	}
	str_tmp[iter++] = ';';
	str_tmp[iter] = '\0';
	*str = str_tmp;
	return OK;
}

status_code parse_cmd(const char* src, char** cmd_name, ull* arg_cnt, char*** args)
{
	if (src == NULL || cmd_name == NULL || arg_cnt == NULL || args == NULL)
	{
		return INVALID_INPUT;
	}
	if (src[0] == '\0')
	{
		*cmd_name = NULL;
		*arg_cnt = 0;
		*args = NULL;
		return OK;
	}
	status_code err_code = OK;
	const char* ptr = src;
	
	ull arg_size = 2;
	*arg_cnt = 0;
	*args = (char**) malloc(sizeof(char*) * 2);
	if (*args == NULL)
	{
		return BAD_ALLOC;
	}
	err_code = sread_until(ptr, "(", 0, &ptr, cmd_name);
	err_code = err_code ? err_code : (*(ptr++) == '(' ? OK : INVALID_INPUT);
	while (!err_code && *ptr != ')' && *ptr != '\0')
	{
		if (*arg_cnt == arg_size)
		{
			arg_size *= 2;
			char** tmp = (char**) realloc(*args, sizeof(char*) * arg_size);
			err_code = tmp != NULL ? OK : BAD_ALLOC;
			*args = tmp != NULL ? tmp : *args;
		}
		if (*ptr == '{')
		{
			err_code = err_code ? err_code : sread_until(ptr, "}", 1, &ptr, &((*args)[*arg_cnt]));
			err_code = err_code ? err_code : (*(ptr++) == '}' ? OK : INVALID_INPUT);
			err_code = err_code ? err_code : ((*(ptr) == ',' || *(ptr) == ')') ? OK : INVALID_INPUT);
		}
		else
		{
			err_code = err_code ? err_code : sread_until(ptr, ",)", 0, &ptr, &((*args)[*arg_cnt]));
		}
		err_code = err_code ? err_code : ((*args)[(*arg_cnt)++][0] != '\0' ? OK : INVALID_INPUT);
		if (!err_code && *ptr == ',')
		{
			++ptr;
		}
		while (!err_code && *ptr == ' ')
		{
			++ptr;
		}
	}
	err_code = err_code ? err_code : (*(ptr++) == ')' ? OK : INVALID_INPUT);
	err_code = err_code ? err_code : (*(ptr++) == ';' ? OK : INVALID_INPUT);
	err_code = err_code ? err_code : (*ptr == '\0' ? OK : INVALID_INPUT);
	if (err_code)
	{
		for (ull i = 0; i < *arg_cnt; ++i)
		{
			free((*args)[i]);
		}
		free(*args);
		free(*cmd_name);
		*arg_cnt = 0;
		*args = NULL;
		*cmd_name = NULL;
	}
	return err_code;
}

status_code validate_var_name(const char* var_name)
{
	if (var_name == NULL)
	{
		return INVALID_ARG;
	}
	if (var_name[0] == '\0' || isdigit(var_name[0]))
	{
		return INVALID_INPUT;
	}
	for (ull i = 0; var_name[i]; ++i)
	{
		if (!isalnum(var_name[i]))
		{
			return INVALID_INPUT;
		}
	}
	return OK;
}

status_code parse_dict_str_double(const char* src, ull* cnt, pair_str_double** dict)
{
	if (src == NULL || cnt == NULL || dict == NULL)
	{
		return INVALID_ARG;
	}
	*cnt = 0;
	for (ull i = 0; src[i]; ++i)
	{
		if (src[i] == ':')
		{
			++(*cnt);
		}
	}
	*dict = (pair_str_double*) malloc(sizeof(pair_str_double) * *cnt);
	if (*dict == NULL)
	{
		return BAD_ALLOC;
	}
	for (ull i = 0; i < *cnt; ++i)
	{
		(*dict)[i].str = NULL;
	}
	// format reminder: {x:-1.11,y:2.1,z:0.81}
	const char* ptr = src;
	status_code err_code = *(ptr++) == '{' ? OK : INVALID_INPUT;
	for (ull i = 0; i < *cnt && !err_code; ++i)
	{
		char* raw_double = NULL;
		err_code = err_code ? err_code : sread_until(ptr, ":", 0, &ptr, &(*dict)[i].str);
		err_code = err_code ? err_code : validate_var_name((*dict)[i].str);
		err_code = err_code ? err_code : (*(ptr++) != '\0' ? OK : INVALID_INPUT);
		err_code = err_code ? err_code : sread_until(ptr, ",}", 0, &ptr, &raw_double);
		err_code = err_code ? err_code : (*(ptr) != '\0' ? OK : INVALID_INPUT);
		ptr = (i + 1 < *cnt) ? (ptr + 1) : ptr; // i + 1 < *cnt => ',' is expected to be skipped
		err_code = err_code ? err_code : parse_double(raw_double, &(*dict)[i].val);
		free(raw_double);
	}
	err_code = err_code ? err_code : (*(ptr++) == '}' ? OK : INVALID_INPUT);
	err_code = err_code ? err_code : (*ptr == '\0' ? OK : INVALID_INPUT);
	if (err_code)
	{
		for (ull i = 0; i < *cnt; ++i)
		{
			free((*dict)[i].str);
		}
		free(*dict);
		*cnt = 0;
		*dict = NULL;
	}
	return err_code;
}


int is_word(const char* str)
{
	if (str == NULL || str[0] == '\0')
	{
		return 0;
	}
	for (ull i = 0; str[i]; ++i)
	{
		if (!isalpha(str[i]))
		{
			return 0;
		}
	}
	return 1;
}

int is_number(const char* str)
{
	if (str == NULL || str[0] == '\0')
	{
		return 0;
	}
	if (!isdigit(str[0]) && str[0] != '-' && str[0] != '+')
	{
		return 0;
	}
	if ((str[0] == '-' || str[0] == '+') && str[1] == '\0')
	{
		return 0;
	} 
	for (ull i = 1; str[i]; ++i)
	{
		if (!isdigit(str[i]))
		{
			return 0;
		}
	}
	return 1;
}

status_code parse_long_long(const char* src, int base, ll* number)
{
	if (src == NULL || number == NULL)
	{
		return INVALID_ARG;
	}
	if (base < 0 || base == 1 || base > 36)
	{
		return INVALID_INPUT;
	}
	if (src[0] == '\0')
	{
		return INVALID_INPUT;
	}
	errno = 0;
	char* ptr;
	*number = strtoll(src, &ptr, base);
	if (*ptr != '\0')
	{
		return INVALID_INPUT;
	}
	if (errno == ERANGE)
	{
		return OVERFLOW;
	}
	return OK;
}

status_code parse_double(const char* src, double* number)
{
	if (src == NULL || number == NULL)
	{
		return INVALID_ARG;
	}
	if (src[0] == '\0')
	{
		return INVALID_INPUT;
	}
	char* ptr;
	*number = strtod(src, &ptr);
	if (*ptr != '\0')
	{
		return INVALID_INPUT;
	}
	return OK;
}


int str_comparator(const void* ptr_1, const void* ptr_2)
{
	const char** str_ptr_1 = (const char**) ptr_1;
	const char** str_ptr_2 = (const char**) ptr_2;
	return strcmp(*str_ptr_1, *str_ptr_2);
}


int sign(ll number)
{
	return number == 0 ? 0 : (number > 0 ? 1 : -1);
}

status_code add_safely(ll arg_1, ll arg_2, ll* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	if (arg_2 > 0 && (arg_1 > LLONG_MAX - arg_2))
	{
		return OVERFLOW;
	}
	if (arg_2 < 0 && (arg_2 < LLONG_MIN - arg_2))
	{
		return OVERFLOW;
	}
	*res = arg_1 + arg_2;
	return OK;
}

status_code sub_safely(ll arg_1, ll arg_2, ll* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	if (arg_2 > 0 && (arg_2 < LLONG_MIN + arg_2))
	{
		return OVERFLOW;
	}
	if (arg_2 < 0 && (arg_1 > LLONG_MAX + arg_2))
	{
		return OVERFLOW;
	}
	*res = arg_1 - arg_2;
	return OK;
}

status_code mult_safely(ll arg_1, ll arg_2, ll* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	if ((arg_1 == LLONG_MIN && arg_2 != 1) || (arg_2 == LLONG_MIN && arg_1 != 1))
	{
		return OVERFLOW;
	}
	if (llabs(arg_1) > LLONG_MAX / llabs(arg_2))
	{
		return OVERFLOW;
	}
	*res = arg_1 * arg_2;
	return OK;
}

status_code div_safely(ll arg_1, ll arg_2, ll* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	if (arg_2 == 0)
	{
		return DIVISION_BY_ZERO;
	}
	*res = arg_1 / arg_2;
	return OK;
}

status_code bpow_safely(ll base, ll pow, ll* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	if (pow < 0)
	{
		return INVALID_INPUT;
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
			status_code err_code = mult_safely(res_tmp, mult, &res_tmp);
			if (err_code)
			{
				return err_code;
			}
		}
		mult *= mult;
		pow >>= 1;
	}
	*res = res_tmp;
	return OK;
}

status_code fbpow_safely(double base, ll pow, double* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	if (base == 0 && pow == 0)
	{
		return ZERO_POWERED_ZERO;
	}
	if (pow == 0)
	{
		*res = 1;
		return OK;
	}
	int pow_sign = sign(pow);
	pow = llabs(pow);
	double res_tmp = 1;
	double mult = base;
	while (pow > 0)
	{
		if (pow & 1)
		{
			res_tmp *= mult;
		}
		mult *= mult;
		pow >>= 1;
	}
	*res = pow_sign == 1 ? res_tmp : 1.0 / res_tmp;
	return OK;
}
