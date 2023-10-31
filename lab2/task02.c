#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <complex.h>
#include <ctype.h>
#include <errno.h>
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

status_codes calc_geometric_mean(double* ans, int cnt, ...)
{
	if (ans == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	
	va_list arg;
	va_start(arg, cnt);
	*ans = 1;
	for (int i = 0; i < cnt; ++i)
	{
		double number = va_arg(arg, double);
		*ans *= number;
	}
	if (!(cnt & 1) && (*ans < 0))
	{
		return INVALID_INPUT;
	}
	
	int sign = 1;
	if (*ans < 0)
	{
		if ((!cnt & 1))
		{
			return INVALID_INPUT;
		}
		*ans *= -1;
		sign = -1;
	}
	*ans = sign * pow(*ans, 1.0 / cnt);
	va_end(arg);
	return OK;
}

status_codes bpow(double base, int exp, double* res)
{
	if (res == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	if (base == 0)
	{
		if (exp == 0)
		{
			return INVALID_INPUT;
		}
		*res = 0;
		return OK;
	}
	
	int neg_flag = 0;
	if (exp < 0)
	{
		neg_flag = 1;
		exp = -exp;
	}
	if (exp == 0)
	{
		*res = 1;
		return OK;
	}
	
	double temp = 1;
	status_codes code;
	if (exp & 1)
	{
		code = bpow(base * base, exp >> 1, &temp);
		temp *= base;
	}
	else
	{
		code = bpow(base * base, exp >> 1, &temp);
	}
	
	if (neg_flag)
	{
		*res = 1 / temp;
	}
	else
	{
		*res = temp;
	}
	return code;
}

int main(int argc, char** argv)
{
	double res;
	printf("Powers of 2 from -5 to 5:\n");
	for (int i = -5; i <= 5; ++i)
	{
		bpow(2, i, &res);
		printf("%lf ", res);
	}
	printf("\n\n");
	calc_geometric_mean(&res, 3, 1.0, 2.0, 3.0);
	printf("geom mean of 1, 2, 3:\n%lf\n", res);
	calc_geometric_mean(&res, 5, 25.0, 36.0, 10.0, 45.0, 60.0);
	printf("geom mean of 25, 36, 10, 45, 60:\n%lf\n", res);
}