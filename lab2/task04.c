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

typedef struct
{	
	double x, y;
} vec2d;

vec2d to_vec2d(double x, double y)
{
	return (vec2d) { x, y };
}

vec2d vec_diff(vec2d a, vec2d b)
{
	vec2d c;
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	return c;
}

double cross_prod(vec2d a, vec2d b)
{
	return a.x * b.y - a.y * b.x;
}

int sign(double x)
{
	return x == 0 ? 0 : (x > 0 ? 1 : -1);
}

status_codes check_rect(int* is_rect, ull dot_cnt, ...)
{
	if (is_rect == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	if (dot_cnt < 3)
	{
		return INVALID_INPUT;
	}
	
	*is_rect = 1;
	va_list arg;
	va_start(arg, dot_cnt);
	
	vec2d first_dot = va_arg(arg, vec2d);
	vec2d second_dot = va_arg(arg, vec2d);
	
	int prev_sign = 0;
	vec2d dot1 = first_dot;
	vec2d dot2 = second_dot;
	for (int i = 0; i < dot_cnt; ++i)
	{
		vec2d dot3;
		if (i + 2 < dot_cnt)
		{
			dot3 = va_arg(arg, vec2d);
		}
		else
		{
			dot3 = (i + 2 == dot_cnt) ? first_dot : second_dot;
		}
		vec2d segment1 = vec_diff(dot2, dot1);
		vec2d segment2 = vec_diff(dot3, dot2);
		int cur_sign = sign(cross_prod(segment1, segment2));
		if (cur_sign != 0) 
		{
			if (prev_sign == 0 || prev_sign == cur_sign)
			{
				prev_sign = cur_sign;
			}
			else
			{
				*is_rect = 0;
				return OK;
			}
		}
		dot1 = dot2;
		dot2 = dot3;
	}
	return OK;
}

status_codes calc_polynomial(double x, double* res, ull n, ...)
{
	if (res == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	
	va_list arg;
	va_start(arg, n);
	
	*res = 0;
	for (int i = 0; i <= n; ++i)
	{
		double coef = va_arg(arg, double);
		*res *= x;
		*res += coef;
	}
	return OK;
}

int main(int argc, char** argv)
{
	int is_rect;
	check_rect(&is_rect, 4, to_vec2d(1, 0), to_vec2d(0, 1), to_vec2d(-1, 0), to_vec2d(0, -1)); // <--- is Rect
	printf("%d\n", is_rect);
	check_rect(&is_rect, 4, to_vec2d(1, 0), to_vec2d(0, -1), to_vec2d(-1, 0), to_vec2d(0, 1)); // <--- is Rect
	printf("%d\n", is_rect);
	check_rect(&is_rect, 4, to_vec2d(1, 0), to_vec2d(-1, 0), to_vec2d(0, 1), to_vec2d(0, -1)); // <--- is not Rect
	printf("%d\n", is_rect);
	check_rect(&is_rect, 4, to_vec2d(0, 0), to_vec2d(1, 1), to_vec2d(-2, 0), to_vec2d(1, -1)); // <--- is not Rect
	printf("%d\n", is_rect);
	
	double res;
	calc_polynomial(1, &res, 3, 5.0, -12.0, 19.0, -25.0);
	printf("%lf\n", res);
	calc_polynomial(2, &res, 3, 5.0, -12.0, 19.0, -25.0);
	printf("%lf\n", res);
	calc_polynomial(3, &res, 3, 5.0, -12.0, 19.0, -25.0);
	printf("%lf\n", res);
}