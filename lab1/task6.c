#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

typedef enum
{
	OK,
	INVALID_INPUT,
	OVERFLOW,
	BAD_ALLOC
} status_codes;

status_codes validate_string_double(char* str_double)
{
	int dot_cnt = 0;
	
	if (!isdigit(str_double[0]) && str_double[0] != '-')
	{
		return INVALID_INPUT;
	}
	
	for (int i = 1; str_double[i]; ++i)
	{
		if (str_double[i] == '.')
		{
			++dot_cnt;
			if (dot_cnt > 1) 
			{
				return INVALID_INPUT;
			}
		}
		else if (!isdigit(str_double[i]))
		{
			return INVALID_INPUT;
		}
	}
	return OK;
}

status_codes integrate_a(double eps, double* ans)
{
	if (ans == NULL)
	{
		return INVALID_INPUT;
	}
	
	// right Riemann sum
	unsigned long long step_cnt = 1;
	double part_len = 1;
	double prev, cur = log(2);
	
	do
	{
		if (step_cnt > ULLONG_MAX / 2)
		{
			return OVERFLOW;
		}
		
		prev = cur;
		step_cnt *= 2;
		part_len /= 2;
		
		cur = 0;
		
		for (int i = step_cnt; i > 0; --i)
		{
			double x = i * 1.0 / step_cnt;
			cur += log(1+x) / x * part_len;
		}
	} while (fabs(cur - prev) >= eps);
	
	*ans = cur;
	
	return OK;
}

status_codes integrate_b(double eps, double* ans)
{
	if (ans == NULL)
	{
		return INVALID_INPUT;
	}
	
	// right Riemann sum
	unsigned long long step_cnt = 1;
	double part_len = 1;
	double prev, cur = exp(-0.5);
	
	do
	{
		if (step_cnt > ULLONG_MAX / 2)
		{
			return OVERFLOW;
		}
		
		prev = cur;
		step_cnt *= 2;
		part_len /= 2;
		
		cur = 0;
		
		for (int i = step_cnt; i > 0; --i)
		{
			double x = i * 1.0 / step_cnt;
			cur += exp(-x*x * 0.5) * part_len;
		}
	} while (fabs(cur - prev) >= eps);
	
	*ans = cur;
	return OK;
}

status_codes integrate_c(double eps, double* ans)
{
	if (ans == NULL)
	{
		return INVALID_INPUT;
	}
	
	// left Riemann sum
	unsigned long long step_cnt = 1;
	double part_len = 1;
	double prev, cur = 0;
	
	do
	{
		if (step_cnt > ULLONG_MAX / 2)
		{
			return OVERFLOW;
		}
		
		prev = cur;
		step_cnt *= 2;
		part_len /= 2;
		
		cur = 0;
		
		for (int i = 0; i < step_cnt; ++i)
		{
			double x = i * 1.0 / step_cnt;
			cur += log(1.0 / (1 - x)) * part_len;
		}
	} while (fabs(cur - prev) >= eps);
	
	*ans = cur;
	return OK;
}

status_codes integrate_d(double eps, double* ans)
{
	if (ans == NULL)
	{
		return INVALID_INPUT;
	}
	
	// right Riemann sum
	unsigned long long step_cnt = 1;
	double part_len = 1;
	double prev, cur = 1;
	
	do
	{
		if (step_cnt > ULLONG_MAX / 2)
		{
			return OVERFLOW;
		}
		
		prev = cur;
		step_cnt *= 2;
		part_len /= 2;
		
		cur = 0;
		
		for (int i = step_cnt; i > 0; --i)
		{
			double x = i * 1.0 / step_cnt;
			cur += pow(x, x) * part_len;
		}
	} while (fabs(cur - prev) >= eps);
	
	*ans = cur;
	return OK;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: command_name <eps>\n");
		printf("application computes some integrals named a, b, c and d...\n");
		printf("I hope, you know those four as well\n");
		return 0;
	}
	
	if (argc != 2)
	{
		printf("Invalid input\n");
		return 1;
	}
	
	char* string_eps = argv[1];
	double eps;
	
	if (validate_string_double(string_eps) != OK)
	{
		printf("Invalid float\n");
		return 2;
	}
	
	eps = atof(string_eps);
	
	if (eps <= 0)
	{
		printf("Epsilon must be positive\n");
		return 3;
	}
	
	double ans;
	
	if (integrate_a(eps, &ans) == OK)
	{
		printf("The value of the integral a is %.15lf\n", ans);
	}
	else
	{
		printf("Calculating the integral a caused overflow\n");
	}
	
	if (integrate_b(eps, &ans) == OK)
	{
		printf("The value of the integral b is %.15lf\n", ans);
	}
	else
	{
		printf("Calculating the integral b caused overflow\n");
	}
	
	if (integrate_c(eps, &ans) == OK)
	{
		printf("The value of the integral c is %.15lf\n", ans);
	}
	else
	{
		printf("Calculating the integral c caused overflow\n");
	}
	
	if (integrate_d(eps, &ans) == OK)
	{
		printf("The value of the integral d is %.15lf\n", ans);
	}
	else
	{
		printf("Calculating the integral d caused overflow\n");
	}
}