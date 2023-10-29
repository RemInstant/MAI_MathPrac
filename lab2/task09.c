#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>

#define EPS 1e-12

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

typedef enum
{
	PSC_PRIME,
	PSC_COMPOSITE,
	PSC_NONE
} prime_status_codes;

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

ull gcd(ull a, ull b)
{
	while (b)
	{
		a %= b;
		ull tmp = a;
		a = b;
		b = tmp;
	}
	return a;
}

status_codes get_common_frac(double frac, ll* numerator, ll* denominator)
{
	if (numerator == NULL || denominator == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	double tmp = frac;
	*denominator = 1;
	while (fabs(tmp - floor(tmp)) >= EPS)
	{
		if (*denominator > ULLONG_MAX / 10)
		{
			return OVERFLOW;
		}
		tmp *= 10;
		*denominator *= 10;
	}
	*numerator = (ll) tmp;
	ull divisor = gcd(llabs(*numerator), *denominator);
	*numerator /= divisor;
	*denominator /= divisor;
	return OK;
}

status_codes check_finite_repres(int base, double frac, int* has_finite_repr)
{
	if (has_finite_repr == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	if (base < 2 || base > 36)
	{
		return INVALID_INPUT;
	}
	
	ll numer, denom;
	status_codes common_frac_code = get_common_frac(frac, &numer, &denom);
	if (common_frac_code != OK)
	{
		return common_frac_code;
	}
	
	for (ll i = 1; i*i <= denom; ++i)
	{
		if (denom % i == 0)
		{
			if ((is_prime(denom / i) == PSC_PRIME && base % (denom / i) != 0)
					|| (is_prime(i) == PSC_PRIME && base % i != 0))
			{
				*has_finite_repr = 0;
				return OK;
			}
		}
	}
	*has_finite_repr = 1;
	return OK;
}

status_codes poly_check_finite_repr(int base, int** have_finite_repr, ull cnt, ...)
{
	if (have_finite_repr == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	
	va_list arg;
	va_start(arg, cnt);
	
	*have_finite_repr = (int*) malloc(sizeof(int) * cnt);
	if(*have_finite_repr == NULL)
	{
		return BAD_ALLOC;
	}
	
	for (ull i = 0; i < cnt; ++i)
	{
		double frac = va_arg(arg, double);
		status_codes check_code = check_finite_repres(base, frac, *have_finite_repr + i);
		if (check_code != OK)
		{
			free(*have_finite_repr);
			return check_code;
		}
	}
	return OK;
}

int main(int argc, char** argv)
{
	int* have_finite_repr;
	int base = 2;
	int cnt = 4;
	status_codes code = poly_check_finite_repr(base, &have_finite_repr, cnt, 1.0, 0.5, 0.125, 0.3);
	
	if (code != OK)
	{
		printf("exit code: %d\n", code);
		return code;
	}
	
	for (int i = 0; i < cnt; ++i)
	{
		if (have_finite_repr[i])
		{
			printf("fraction N%d has a finite representation in base %d\n", i+1, base);
		}
		else
		{
			printf("fraction N%d hasn't a finite representation in base %d\n", i+1, base);
		}
	}
	free(have_finite_repr);
}