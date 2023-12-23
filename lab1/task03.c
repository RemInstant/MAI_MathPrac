#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>

typedef enum
{
	OK,
	INVALID_INPUT,
	INVALID_EPS,
	OVERFLOW,
	BAD_ALLOC
} status_codes;

typedef enum
{
	TWO_ROOTS,
	ONE_ROOT,
	NO_ROOTS,
	INF_ROOTS,
} root_count;

status_codes validate_string_integer(char* str_int)
{
	int integer = 0;
	
	if (str_int[0] != '-')
	{
		for (int i = 0; str_int[i]; ++i)
		{
			if (!isdigit(str_int[i]))
			{
				return INVALID_INPUT;
			}
			
			int add = str_int[i] - '0';
			if (integer > (INT_MAX - add) / 10)
			{
				return OVERFLOW;
			}
			
			integer = integer * 10 + add;
		}
	}
	else
	{
		for (int i = 1; str_int[i]; ++i)
		{
			if (!isdigit(str_int[i]))
			{
				return INVALID_INPUT;
			}
			
			int subtr = str_int[i] - '0';
			if (integer < (INT_MIN + subtr) / 10)
			{
				return OVERFLOW;
			}
			
			integer = integer * 10 - subtr;
		}
	}
	return OK;
}

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

status_codes validate_arguments(char flag, int argc, char** argv)
{
	switch (flag)
	{
		case 'q':
		case 't':
			
			if (argc != 6)
			{
				return INVALID_INPUT;
			}
			for (int i = 2; i < 6; ++i)
			{
				if (validate_string_double(argv[i]) != OK)
				{
					return INVALID_INPUT;
				}
			}
			if (atof(argv[2]) <= 0)
			{
				return INVALID_EPS;
			}
			break;
			
		case 'm':
			
			if (argc != 4)
			{
				return INVALID_INPUT;
			}
			
			for (int i = 2; i < 4; ++i)
			{
				if (validate_string_integer(argv[i]) != OK || atoi(argv[i]) == 0)
				{
					return INVALID_INPUT;
				}
			}
			break;
	}
	return OK;
}

status_codes factorial(long long integer, unsigned long long* result)
{
	if (integer < 0 || result == NULL)
	{
		return INVALID_INPUT;
	}
	
	*result = 1;
	for (int i = 2; i <= integer; ++i)
	{
		if (*result > ULLONG_MAX / i)
		{
			return OVERFLOW;
		}
		*result *= i;
	}
	return OK;
}

status_codes farr_swap(double* farr, int i, int j)
{
	if (farr == NULL)
	{
		return INVALID_INPUT;
	}
	double temp = farr[i];
	farr[i] = farr[j];
	farr[j] = temp;
	return OK;
}

status_codes permutations(double eps, unsigned int cnt, double* numbers, unsigned long long* res_cnt, double*** res)
{
	if (res_cnt == NULL || res == NULL)
	{
		return INVALID_INPUT;
	}
	
	if (numbers == NULL)
	{
		*res_cnt = 0;
		return OK;
	}
	
	status_codes fact_code = factorial(cnt, res_cnt);
	if (fact_code != OK)
	{
		return fact_code;
	}
	
	*res = (double**) malloc(sizeof(double*) * (*res_cnt));
	if (*res == NULL)
	{
		return BAD_ALLOC;
	}
	
	for (int i = 0; i < *res_cnt; ++i)
	{
		(*res)[i] = (double*) malloc(sizeof(double) * cnt);
		
		if ((*res)[i] == NULL)
		{
			for (int j = i-1; j >= 0; --j)
			{
				free((*res)[j]);
			}
			free(*res);
			*res = NULL;
			return BAD_ALLOC;
		}
	}
	
	for (int j = 0; j < cnt; ++j)
	{
		(*res)[0][j] = numbers[j];
	}
	
	// compute permutations in lexicographic order
	for (int i = 1; i < *res_cnt; ++i)
	{
		for (int j = 0; j < cnt; ++j)
		{
			(*res)[i][j] = (*res)[i-1][j];
		}

		double* cur_perm = (*res)[i];
		int fi_pos = cnt-2, se_pos = cnt-1;
		
		while (fi_pos != -1 && cur_perm[fi_pos] >= cur_perm[fi_pos+1])
		{
			--fi_pos;
		}
		
		if (fi_pos == -1)
		{
			// from the last sequence in lexicograpic order to the first one (bcs started from non-first)
			for (int j = 0; 2*j < cnt; ++j)
			{
				farr_swap(cur_perm, j, cnt-j-1);
			}
		}
		else
		{
			while (cur_perm[se_pos] <= cur_perm[fi_pos]) 
			{
				--se_pos;
			}
			
			farr_swap(cur_perm, fi_pos, se_pos);
			
			for (int j = 1; 2*j < cnt-fi_pos; ++j)
			{
				farr_swap(cur_perm, fi_pos+j, cnt-j);
			}	
		}
	}
	
	int bound = *res_cnt; // first element to be deleted
	
	// turn array into { unique permutations | repeating permutations }
	for (int i = 0; i < bound; ++i)
	{
		for (int j = i+1; j < bound; ++j)
		{
			int flag = fabs((*res)[i][0] - (*res)[j][0]) < eps;
			flag = flag && fabs((*res)[i][1] - (*res)[j][1]) < eps;
			flag = flag && fabs((*res)[i][2] - (*res)[j][2]) < eps;
			
			if (flag)
			{
				double* temp = (*res)[bound-1];
				(*res)[bound-1] = (*res)[j];
				(*res)[j] = temp;
				--bound; --j;
			}
		}
	}
	*res_cnt = bound;
	double** tmp = (double**) realloc(*res, sizeof(double*) * (bound));
	if (tmp == NULL)
	{
		for (int i = 0; i < *res_cnt; ++i)
		{
			free((*res)[i]);
		}
		free(*res);
		*res = NULL;
		return BAD_ALLOC;
	}
	// free repeating permutations
	for (int i = bound; i < *res_cnt; ++i)
	{
		free((*res)[i]);
	}
	*res = tmp;
	return OK;
}

status_codes solve_quadratic_equation(double eps, double a, double b, double c, root_count* cnt, double* x1, double* x2)
{
	if (cnt == NULL || x1 == NULL || x2 == NULL)
	{
		return INVALID_INPUT;
	}
	
	if (fabs(a) < eps)
	{
		if (fabs(b) < eps)
		{
			if (fabs(c) < eps)
			{
				*cnt = INF_ROOTS;
			}
			else
			{
				*cnt = NO_ROOTS;
			}
		}
		else
		{
			*cnt = ONE_ROOT;
			*x1 = -c / b;
		}
		return OK;
	}
	
	double discr = b*b - 4*a*c;
	
	if (discr < -eps)
	{
		*cnt = NO_ROOTS;
	}
	else if (fabs(discr) < eps)
	{
		*cnt = ONE_ROOT;
		*x1 = -b / (2*a);
	}
	else
	{
		*cnt = TWO_ROOTS;
		*x1 = (-b - sqrt(discr)) / (2*a);
		*x2 = (-b + sqrt(discr)) / (2*a);
	}
	return OK;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: command_name <flag>\n");
		printf("flags:\n");
		printf("-q <epsilon> <a> <b> <c>  -  solve quadratic equation for all permutations of real coefficients a, b, c.\n");
		printf("                             Epsilon specifies the accuracy of comparison of float point numbers.\n");
		printf("-m  <a> <b>               -  check if integer a is a multiple of integer b (integers must be non-zero)\n");
		printf("-t <epsilon> <a> <b> <c>  -  check if real _non-zero_ numbers a, b, c can be sides of a right triangle.\n");
		printf("                             Epsilon specifies the accuracy of comparison of float point numbers.\n");
		return 0;
	}
	
	char* flag = argv[1];
	
	if (((flag[0] != '-') && (flag[0] != '/')) || flag[2] != '\0' 
			|| (flag[1] != 'q' && flag[1] != 'm' && flag[1] != 't'))
	{
		printf("Invalid flag\n");
		return 2;
	}
	
	switch (validate_arguments(flag[1], argc, argv))
	{
		case OK:
			break;
		case INVALID_INPUT:
			printf("Invalid input\n");
			return 3;
		case INVALID_EPS:
			printf("Epsilon must be positive\n");
			return 4;
		default:
			printf("Unexpected error occurred\n");
			return 6;
	}
	
	switch (flag[1])
	{
		case 'q':
		{
			double eps = atof(argv[2]);
			double* eq_coefs = (double*) malloc(sizeof(double)*3);
			
			if (eq_coefs == NULL)
			{
				printf("Memory lack error\n");
				return 5;
			}
			
			eq_coefs[0] = atof(argv[3]);
			eq_coefs[1] = atof(argv[4]);
			eq_coefs[2] = atof(argv[5]);
			
			double** eq_coefs_perm = NULL;
			unsigned long long perm_cnt;
			
			status_codes perm_code = permutations(eps, 3, eq_coefs, &perm_cnt, &eq_coefs_perm);
			
			if (perm_code != OK)
			{
				free(eq_coefs);
				
				switch (perm_code)
				{
					case BAD_ALLOC:
						printf("Memory lack error\n");
						return 5;
					default:
						printf("Unexpected error occured");
						return 6;
				}
			}
			
			for (int i = 0; i < perm_cnt; ++i)
			{
				root_count root_cnt;
				double x1, x2;
				
				double a = eq_coefs_perm[i][0];
				double b = eq_coefs_perm[i][1];
				double c = eq_coefs_perm[i][2];
				
				solve_quadratic_equation(eps, a, b, c, &root_cnt, &x1, &x2);
				printf("Equation %.4lf*x^2 + %.4lf*x + %.4lf = 0   ", a, b, c);
				switch (root_cnt)
				{
					case TWO_ROOTS:
						printf("has two roots: x1 = %lf   x2 = %lf\n", x1, x2);
						break;
					case ONE_ROOT:
						printf("has one root: x1 = %lf\n", x1);
						break;
					case NO_ROOTS:
						printf("has no roots\n");
						break;
					case INF_ROOTS:
						printf("has infinity number of roots\n");
						break;
				}
			}
			
			for (int i = 0; i < perm_cnt; ++i)
			{
				free(eq_coefs_perm[i]);	
			}
			if (eq_coefs_perm != NULL)
			{
				free(eq_coefs_perm);
			}
			free(eq_coefs);
			break;
		}
		case 'm':
		{
			int first_number = atoi(argv[2]);
			int second_number = atoi(argv[3]);
			
			if (second_number % first_number == 0)
			{
				printf("%d is a multiple of %d\n", first_number, second_number);
			}
			else
			{
				printf("%d is not a multiple of %d\n", first_number, second_number);
			}
			break;
		}
		case 't':
		{
			double eps = atof(argv[2]);
			
			double side_a = atof(argv[3]);
			double side_b = atof(argv[4]);
			double side_c = atof(argv[5]);
			
			if (fabs(side_a * side_a + side_b * side_b - side_c * side_c) < eps 
					|| fabs(side_a * side_a + side_c * side_c - side_b * side_b) < eps
					|| fabs(side_b * side_b + side_c * side_c - side_a * side_a) < eps)
			{
				printf("numbers %lf, %lf, %lf can be sides of a right triangle\n", side_a, side_b, side_c);
			}
			else
			{
				printf("numbers %lf, %lf, %lf cannot be sides of a right triangle\n", side_a, side_b, side_c);
			}
			break;
		}
	}
}