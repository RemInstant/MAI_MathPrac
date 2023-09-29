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

typedef enum
{
	SSC_CONVERGENCE,
	SSC_DIVERGENCE,
	SSC_INVALID_INPUT
} series_status_codes;

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

series_status_codes sum_a(double eps, double x, double* ans)
{
	// this series converges for any x
	if (ans == NULL) 
	{
		return SSC_INVALID_INPUT;
	}
	
	int n = 0;
	double sum = 0, elem = 1;
	
	do
	{
		sum += elem;
		++n;
		elem *= x / n;

	} while (fabs(elem) >= eps);
	
	*ans = sum;
	
	return SSC_CONVERGENCE;
}

int sum_b(double eps, double x, double* ans)
{
	// this series converges for any x
	if (ans == NULL)
	{
		return SSC_INVALID_INPUT;
	}
	
	int n = 0;
	double sum = 0, elem = 1;
	
	do
	{
		sum += elem;
		++n;
		elem *= -x*x / (2.0*n * (2.0*n-1));
		
	} while (fabs(elem) >= eps);
	
	*ans = sum;
	
	return SSC_CONVERGENCE;
}

int sum_c(double eps, double x, double* ans)
{
	// this series converges for |x| < 1
	if (ans == NULL)
	{
		return SSC_INVALID_INPUT;
	}
	
	if (fabs(x) >= 1)
	{
		return SSC_DIVERGENCE;
	}
	
	int n = 0;
	double sum = 0, elem = 1;
	
	do
	{
		sum += elem;
		++n;
		elem *= 9.0 * n*n * x*x / ((3.0*n-1) * (3.0*n-2));
		
	} while (fabs(elem) >= eps);
	
	*ans = sum;
	
	return SSC_CONVERGENCE;
}

int sum_d(double eps, double x, double* ans)
{
	// this series converges for |x| < 1
	if (ans == NULL)
	{
		return SSC_INVALID_INPUT;
	}
	
	if (fabs(x) >= 1)
	{
		return SSC_DIVERGENCE;
	}
	
	int n = 1;
	double sum = 0, elem = -x*x / 2;
	
	do
	{
		sum += elem;
		++n;
		elem *= -(2.0*n - 1) * x*x / (2.0*n);
		
	} while (fabs(elem) >= eps);
	
	*ans = sum;
	
	return SSC_CONVERGENCE;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: command_name <eps> <x>\n");
		printf("application computes some series that are depend on real number x and named a, b, c and d...\n");
		printf("you know them, yeah?\n");
		printf("(series c and d converges for |x| < 1)\n");
		return 0;
	}
	
	if (argc != 3)
	{
		printf("Invalid input\n");
		return 1;
	}
	
	char* string_eps = argv[1];
	char* string_x = argv[2];
	
	if (validate_string_double(string_eps) != OK || validate_string_double(string_x) != OK)
	{
		printf("Invalid float\n");
		return 2;
	}
	
	double eps = atof(string_eps);
	double x = atof(string_x);
	
	if (eps <= 0)
	{
		printf("Epsilon must be positive\n");
		return 3;
	}
	
	double ans;
	
	printf("The sum of the series a is %.15lf\n", ans);
	printf("The sum of the series b is %.15lf\n", ans);
	
	if (sum_c(eps, x, &ans) == SSC_CONVERGENCE)
	{
		printf("The sum of the series c is %.15lf\n", ans);
	}
	else
	{
		printf("The series c disconverges\n");
	}
	
	if (sum_d(eps, x, &ans) == SSC_CONVERGENCE)
	{
		printf("The sum of the series d is %.15lf\n", ans);
	}
	else
	{
		printf("The series d disconverges\n");
	}
}