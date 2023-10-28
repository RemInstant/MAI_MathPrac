#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
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

double func1(double x)
{
	return log(x) + 6.5 - x*x;
}

double func2(double x)
{
	return exp(x) + x;
}

double func3(double x)
{
	return pow(9, tan(x)) - pow(12, tan(x)) + 3;
}

int sign(double x)
{
	return x == 0 ? 0 : (x > 0 ? 1 : -1);
}

status_codes solve_eq_dichotomy(double left, double right, double eps, double (*func)(double), double* ans)
{
	int right_sign = sign(func(right));
	int left_sign = sign(func(left));
	
	if (fabs(left_sign) < eps)
	{
		*ans = left;
		return OK;
	}
	if (fabs(right_sign) < eps)
	{
		*ans = right;
		return OK;
	}
	if (right_sign == left_sign)
	{
		return INVALID_INPUT;
	}
	
	int increasing = right_sign > left_sign;

	double mid, val;
	do
	{
		mid = (left + right) / 2;
		val = func(mid);
		
		if ((val > 0) ^ increasing)
		{
			left = mid;
		}
		else
		{
			right = mid;
		}
	} while(fabs(val) >= eps);
	
	*ans = mid;
	return OK;
}

int main(int argc, char** argv)
{
	double res;
	printf("Equation: ln(x) + 6.5 - x*x = 0\n");
	solve_eq_dichotomy(1, 4, 1e-5, func1, &res);
	printf("[1; 4], eps = 1e-5 -> %.10lf\n", res);
	solve_eq_dichotomy(1e-6, 1, 1e-12, func1, &res);
	printf("[1e-6; 1], eps = 1e-12 -> %.10lf\n", res);
	solve_eq_dichotomy(1, 5555, 1e-3, func1, &res);
	printf("[1; 5555], eps = 1e-3 -> %.10lf\n", res);
	
	printf("\nEquation: exp(x) + x = 0\n");
	solve_eq_dichotomy(-1, 1, 1e-2, func2, &res);
	printf("[-1; 1], eps = 1e-2 -> %.10lf\n", res);
	solve_eq_dichotomy(-10, 10, 1e-12, func2, &res);
	printf("[-10; 10], eps = 1e-12 -> %.10lf\n", res);
	solve_eq_dichotomy(-3939, 5777, 1e-5, func2, &res);
	printf("[-3939; 5777], eps = 1e-5 -> %.10lf\n", res);
	
	printf("\nEquation: pow(9, tan(x)) - pow(12, tan(x)) + 3 = 0\n");
	solve_eq_dichotomy(-1, 1, 1e-9, func3, &res);
	printf("[-1; 1], eps = 1e-9 -> %.10lf\n", res);
	solve_eq_dichotomy(3, 4.5, 1e-7, func3, &res);
	printf("[3; 4.5], eps = 1e-7 -> %.10lf\n", res);
	solve_eq_dichotomy(-7, 12, 1e-4, func3, &res);
	printf("[-7; 12], eps = 1e-4 -> %.10lf\n", res);
}