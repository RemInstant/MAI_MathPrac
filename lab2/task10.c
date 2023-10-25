#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef long long ll;
typedef unsigned long long ull;

typedef enum
{
	OK,
	INVALID_INPUT,
	INVALID_FLAG,
	INVALID_NUMBER,
	FILE_OPENING_ERROR,
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
		case FILE_OPENING_ERROR:
			printf("File cannot be opened\n");
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

// a0 + a1*x + a2 * x^2 + ...
status_codes calc_polynomial(double x, ull n, double* coefs, double* res)
{
	if (coefs == NULL || res == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	
	*res = 0;
	for (int i = 0; i <= n; ++i)
	{
		*res *= x;
		*res += coefs[n-i];
	}
	return OK;
}

status_codes transform_polynomial(double eps, double a, double** res_coefs, ull n, ...)
{
	if (res_coefs == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	
	double* coefs = (double*) malloc(sizeof(double) * (n + 1));
	if (coefs == NULL)
	{
		return BAD_ALLOC;
	}
	*res_coefs = (double*) malloc(sizeof(double) * (n + 1));
	if (*res_coefs == NULL)
	{
		free(coefs);
		return BAD_ALLOC;
	}
	
	va_list arg;
	va_start(arg, n);
	for (int i = 0; i <= n; ++i)
	{
		coefs[i] = va_arg(arg, double);
	}
	va_end(arg);
	
	double divisor = 1;
	for (int i = 0; i <= n; ++i)
	{
		status_codes calc_code = calc_polynomial(a, n-i, coefs, *res_coefs + i);
		if (calc_code != OK)
		{
			free(coefs);
			free(*res_coefs);
			return calc_code;
		}
		
		(*res_coefs)[i] /= divisor;
		divisor *= i + 1;
		
		// differentiation
		for (int j = 0; j < n-i; ++j)
		{
			coefs[j] = (j + 1) * coefs[j+1];
		}
		coefs[n-i] = 0;
	}
	
	free(coefs);
	return OK;
}

int main(int argc, char** argv)
{
	double* coefs;
	ull n = 3;
	status_codes code = transform_polynomial(1, 1, &coefs, n, 5.0, 11.0, 3.0, 8.0);
	if (code != OK)
	{
		printf("exit code: %d", code);
		return code;
	}
	
	for (ull i = 0; i <= n; ++i)
	{
		printf("g%llu = %lf\n", i, coefs[i]);
	}
}