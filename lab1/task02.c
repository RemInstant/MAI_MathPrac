#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>

#include <float.h>

typedef enum
{
	OK,
	INVALID_INPUT,
	OVERFLOW,
	BAD_ALLOC
} status_codes;

typedef enum
{
	PSC_PRIME,
	PSC_COMPOSITE,
	PSC_NONE
} prime_status_codes;

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

prime_status_codes is_prime(long long integer)
{
	if (integer <= 1) 
	{
		return PSC_NONE;
	}
	if (integer == 2)
	{
		return PSC_PRIME;
	}
	if (!(integer & 1))
	{
		return PSC_COMPOSITE;
	}

	for (int i = 3; i*i <= integer; i += 2)
	{
		if (integer % i == 0)
		{
			return PSC_COMPOSITE;
		}
	}
	return PSC_PRIME;
}

double calc_e_with_lim(double eps)
{
	int n = 1;
	double prev = 0; 
	double cur = 2; // function with n equal to 1
	do
	{
		++n;
		prev = cur;
		cur = 1.0;
		for (int i = 0; i < n; ++i)
		{
			cur *= 1 + 1.0/n;
		}
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_e_with_sum(double eps)
{
	int n = 0;
	double factorial = 1;
	double next_elem = 1;
	double ans = 0;
	do
	{
		ans += next_elem;
		
		++n;
		factorial *= n;
		next_elem = 1 / factorial;
		
	} while (next_elem >= eps);
	return ans;
}

double calc_e_with_equation(double eps)
{
	// ln(x) = 1 using the Newton's method
	double prev = 0; 
	double cur = 3;
	do
	{
		prev = cur;	
		// f(x) = log(x) - 1
		// f(x) / f'(x) = x * (log(x) - 1)
		cur -= cur*(log(cur)-1);
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_pi_with_lim(double eps)
{
	int n = 1;
	double prev = 0;
	double cur = 4; // function with n equal to 1;
	
	do
	{
		++n;
		prev = cur;
		// f(n) = f(n-1) * f(n) / f(n-1) = f(n-1) * (4n(n-1)/(2n-1)^2)
		cur *= 4.0 * n * (n-1) / ((2.0*n-1) * (2.0*n-1));
	} while (fabsl(cur-prev) >= eps);
	return cur;
}

double calc_pi_with_sum(double eps)
{
	int n = 1;
	double next_elem = 4;
	double ans = 0;
	do
	{
		ans += next_elem;
		
		++n;
		next_elem = 4 / (2.0*n-1);
		if (!(n & 1)) 
		{
			next_elem *= -1;
		}
	} while (fabsl(next_elem) >= eps);
	return ans;
}

double calc_pi_with_equation(double eps)
{
	// cos(x) = -1 using the Newton's method
	double prev = 0; 
	double cur = 3;
	do
	{
		prev = cur;	
		// f(x) = cos(x) + 1
		// f(x) / f'(x) = -(cos(x) + 1) / sin(x)
		cur += (cos(cur)+1) / sin(cur);
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_ln2_with_lim(double eps)
{
	int n = 1;
	double prev = 0; 
	double cur = 1; // function with n equal to 1
	do
	{
		++n;
		prev = cur;
		cur = n * (pow(2, 1.0/n) - 1);
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_ln2_with_sum(double eps)
{
	int n = 1;
	double next_elem = 1;
	double ans = 0;
	do
	{
		ans += next_elem;
		
		++n;
		next_elem = 1.0 / n;
		if (!(n & 1)) 
		{
			next_elem *= -1;
		}
	} while (fabsl(next_elem) >= eps);
	return ans;
}

double calc_ln2_with_equation(double eps)
{
	// exp(x) = 2 using the Newton's method
	double prev = 0; 
	double cur = 1;
	do
	{
		prev = cur;	
		// f(x) = exp(x) - 2
		// f(x) / f'(x) = (exp(x) - 2) / exp(x) = 1 - 2/exp(x)
		cur -= 1 - 2/exp(cur);
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_sqrt2_with_lim(double eps)
{
	double prev = 0; 
	double cur = -0.5;
	do
	{
		prev = cur;
		cur += -cur * cur / 2 + 1;
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_sqrt2_with_prod(double eps)
{
	double next_elem = pow(2, 0.25);
	double prev = 1;
	double cur = 1;
	do
	{
		prev = cur;
		cur *= next_elem;
		next_elem = pow(next_elem, 0.5);
			
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_sqrt2_with_equation(double eps)
{
	// x^2 = 2 using the Newton's method
	double prev = 0; 
	double cur = 1;
	do 
	{
		prev = cur;	
		// f(x) = x^2 - 2
		// f(x) / f'(x) = (x^2 - 2) / (2x)
		cur -= (cur*cur-2) / (2*cur);
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_gamma_with_lim(double eps)
{
	int m = 1;
	double prev = 0; 
	double cur = 0; // function with n equal to 1
	
	do
	{
		++m;
		prev = cur;
		cur = 0;
		
		double elem = 0;
		double comb = m;
		double logarithm = 0;
		
		for (int k = 2; k <= m; ++k)
		{
			comb *= (m-k+1.0) / k;
			logarithm += log(k);
			
			elem = comb * logarithm / k;
			if (k & 1) 
			{
				elem *= -1;
			}
			cur += elem;
		}
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_gamma_with_sum(double eps)
{
	int n = 2;
	long long temp;
	double next_elem = 0.5;
	double pi = calc_pi_with_equation(1e-12);
	double ans = -pi*pi / 6;
	do
	{
		ans += next_elem;
		
		++n;
		next_elem = pow(floor(sqrt(n)), -2) - 1.0/n;
		
		temp = (int) sqrt(n);
		
	} while (fabsl(next_elem) >= eps || fabsl(temp * temp - n) < 1e-18);
	return ans;
}

double calc_gamma_with_equation(double eps)
{
	int t = 2;
	double prod = 0.5;
	
	double prev = 0; 
	double cur = -log(0.5 * log(2)); // function with t equal to 2
	
	do
	{
		++t;
		prev = cur;	
		
		if (is_prime(t) == PSC_PRIME) prod *= (t-1.0)/t;
		
		cur = -log(prod * log(t));

	} while (fabsl(cur - prev) >= eps);
	return cur;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: command_name <eps>\n");
		printf("application computes such constants as e, pi, ln2, sqrt2, gamma using limit, series and equation with accuracy of <eps>\n");
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
	
	printf("e = %.10lf (limit)\n", calc_e_with_lim(eps));
	printf("e = %.10lf (series)\n", calc_e_with_sum(eps));
	printf("e = %.10lf (equation)\n\n", calc_e_with_equation(eps));
	printf("pi = %.10lf (limit)\n", calc_pi_with_sum(eps));
	printf("pi = %.10lf (series)\n", calc_pi_with_lim(eps));
	printf("pi = %.10lf (equation)\n\n", calc_pi_with_equation(eps));
	printf("ln2 = %.10lf (limit)\n", calc_ln2_with_lim(eps));
	printf("ln2 = %.10lf (series)\n", calc_ln2_with_sum(eps));
	printf("ln2 = %.10lf (equation)\n\n", calc_ln2_with_equation(eps));
	printf("sqrt2 = %.10lf (limit)\n", calc_sqrt2_with_lim(eps));
	printf("sqrt2 = %.10lf (series)\n", calc_sqrt2_with_prod(eps));
	printf("sqrt2 = %.10lf (equation)\n\n", calc_sqrt2_with_equation(eps));
	printf("gamma = %.10lf (limit)\n", calc_gamma_with_lim(eps));
	printf("gamma = %.10lf (series)\n", calc_gamma_with_sum(eps));
	printf("gamma = %.10lf (equation)\n", calc_gamma_with_equation(eps));
}