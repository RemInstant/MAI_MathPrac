#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stdarg.h>

typedef long long ll;
typedef unsigned long long ull;

typedef enum
{
	OK,
	INVALID_INPUT,
	INVALID_FLAG,
	INVALID_NUMBER,
	OVERFLOW,
	BAD_ALLOC,
	NULL_POINTER_ERROR
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
		case OVERFLOW:
			printf("An overflow occurred\n");
			return;
		case BAD_ALLOC:
			printf("Memory lack error occurred\n");
			return;
		case NULL_POINTER_ERROR:
			printf("Null pointer error occurred\n");
			return;
		default:
			printf("Unexpected error occurred\n");
			return;
	}
}

int ctoi(char ch)
{
	if (isdigit(ch))
	{
		return ch - '0';
	}
	if (isalpha(ch))
	{
		return toupper(ch) - 'A' + 10;
	}
	return -1;
}

char itoc(int integer)
{
	if (integer < 0)
	{
		return -1;
	}
	if (integer < 10)
	{
		return '0' + integer;
	}
	if (integer < 36)
	{
		return 'A' + integer - 10;
	}
	return -1;
}

status_codes long_sum(int base, char* left, char* right, char** res)
{
	if (left == NULL || right == NULL || res == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	if (base < 2 || base > 36)
	{
		return INVALID_INPUT;
	}
	for (int i = 0; left[i]; ++i)
	{
		int l_digit = ctoi(left[i]);
		if (l_digit == -1 || l_digit >= base)
		{
			return INVALID_INPUT;
		}
	}
	for (int i = 0; right[i]; ++i)
	{
		int r_digit = ctoi(right[i]);
		if (r_digit == -1 || r_digit >= base)
		{
			return INVALID_INPUT;
		}
	}
	
	ull left_len = strlen(left);	// longer
	ull right_len = strlen(right);	// shorter
	if (left_len < right_len)
	{
		char* tmp = left;
		left = right;
		right = tmp;
		ull tmp_len = left_len;
		left_len = right_len;
		right_len = tmp_len;
	}
	if (left_len > ULLONG_MAX - 2)
	{
		return OVERFLOW;
	}
	
	ull res_len = left_len + 1;
	*res = (char*) malloc(sizeof(char) * (res_len+1));
	if (*res == NULL)
	{
		return BAD_ALLOC;
	}
	(*res)[res_len - 1] = '\0';
	
	int carry_digit = 0;
	for (ull i = left_len - 1; i >= left_len - right_len && i != ULLONG_MAX; --i)
	{
		int l_digit = ctoi(left[i]);
		int r_digit = ctoi(right[i - (left_len - right_len)]);
		int res_digit = (l_digit + r_digit + carry_digit) % base;
		carry_digit = (l_digit + r_digit + carry_digit) / base;
		(*res)[i] = itoc(res_digit);
	}
	for (ull i = left_len - right_len - 1; i != ULLONG_MAX; --i)
	{
		int l_digit = ctoi(left[i]);
		int res_digit = (l_digit + carry_digit) % base;
		carry_digit = (l_digit + carry_digit) / base;
		(*res)[i] = itoc(res_digit);
	}
	if (carry_digit != 0)
	{
		for (ull i = res_len; i > 0; --i)
		{
			(*res)[i] = (*res)[i-1];
		}
		(*res)[0] = itoc(carry_digit);
	}
	return OK;
}

status_codes poly_sum(int base, char** res, ull cnt, ...)
{
	if (res == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	
	va_list arg;
	va_start(arg, cnt);
	
	*res = (char*) malloc(sizeof(char) * 2);
	if(*res == NULL)
	{
		return BAD_ALLOC;
	}
	sprintf(*res, "0");
	
	for (ull i = 0; i < cnt; ++i)
	{
		char* summand = va_arg(arg, char*);
		char* tmp;
		status_codes sum_code = long_sum(base, *res, summand, &tmp);
		if (sum_code != OK)
		{
			free(*res);
			return sum_code;
		}
		free(*res);
		*res = tmp;
	}
	return OK;
}

int main(int argc, char** argv)
{
	char* res = NULL;
	poly_sum(10, &res, 4, "111222", "11333", "1444", "1");
	printf("%s\n", res);
	free(res);
	poly_sum(16, &res, 3, "10A", "553", "462");
	printf("%s\n", res);
	free(res);
	poly_sum(2, &res, 3, "101", "111", "10000");
	printf("%s\n", res);
	free(res);
}