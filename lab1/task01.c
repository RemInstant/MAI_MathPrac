#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
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
	PSC_PRIME,
	PSC_COMPOSITE,
	PSC_NONE
} prime_status_codes;

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
		if (!isdigit(str_int[1]))
		{
			return INVALID_INPUT;
		}
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

status_codes get_multiples(int integer, int* cnt, int** multiples)
{
	if (cnt == NULL || multiples == NULL)
	{
		return INVALID_INPUT;
	}
	
	integer = abs(integer);
	*cnt = (integer != 0) ? (100 / integer) : 0;
	*multiples = (int*) malloc(sizeof(int) * (*cnt));
	
	if (*multiples == NULL)
	{
		return BAD_ALLOC;
	}
	if (*cnt == 0)
	{
		return OK;
	}
	
	for (int i = 0, val = integer; i < *cnt; ++i) 
	{
		(*multiples)[i] = val;
		val += integer;
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

status_codes sum_natural_numbers(int integer, unsigned long long* result)
{
	if (integer < 1 || result == NULL)
	{
		return INVALID_INPUT;
	}
	
	*result = integer + 1;

	if (*result > 2 * ULLONG_MAX / integer)
	{
		return OVERFLOW;
	}
	
	*result *= integer;
	*result /= 2;
	
	return OK;
}

int get_int_len(long long integer)
{
	int len = (integer < 0) ? 1 : 0;
	
	do
	{
		integer /= 10;
		++len;
	} while (integer > 0);
	
	return len;
}

status_codes separate_into_digits(int integer, int* cnt, int** digits)
{
	if (cnt == NULL || digits == NULL) 
	{
		return INVALID_INPUT;
	}
	
	if (integer == INT_MIN)
	{
		return OVERFLOW;
	}
	
	integer = abs(integer);
	*cnt = get_int_len(integer);
	*digits = (int*) malloc(sizeof(int) * (*cnt));
	
	if (*digits == NULL)
	{
		return BAD_ALLOC;
	}
	
	for (int i = *cnt-1; i >= 0; --i)
	{
		(*digits)[i] = integer % 10;
		integer /= 10;
	}
	return OK;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: command_name <flag> <32-bit integer>\n");
		printf("flags:\n");
		printf("-h  -  print natural numbers up to 100 that is multiples of <integer>\n");
		printf("-p  -  check if <integer> is a prime\n");
		printf("-s  -  separate <integer> into digits\n");
		printf("-e  -  print the exponential of natural numbers up to <integer>");
		printf(" with base from 1 to 10 (1 <= <integer> <= 10)\n");
		printf("-a  -  print the sum of all natural numbers up to <integer>\n");
		printf("-f  -  print the factorial of <integer>\n");
		return 0;
	}
	
	if (argc != 3)
	{
		printf("Invalid input\n");
		return 1;
	}
	
	char* flag = argv[1];
	char* string_int = argv[2];
	
	if (((flag[0] != '-') && (flag[0] != '/')) || flag[2]
			|| (flag[1] != 'h' && flag[1] != 'p' && flag[1] != 's'
			&& flag[1] != 'e' && flag[1] != 'a' && flag[1] != 'f'))
	{
		printf("Invalid flag\n");
		return 2;
	}
	
	switch (validate_string_integer(string_int))
	{
		case OK:
			break;
		case INVALID_INPUT:
			printf("Invalid integer\n");
			return 3;
		case OVERFLOW:
			printf("Entered integer caused an overflow\n");
			return 4;
		default:
			printf("Unexpected error occurred\n");
			return 6;
	}
	
	int integer = atoi(string_int);
	
	switch (flag[1])
	{
		case 'h': // multiples
		{
			int cnt = 0;
			int* multiples = NULL;
			
			switch (get_multiples(integer, &cnt, &multiples))
			{
				case OK:
					break;
				case BAD_ALLOC:
					printf("Memory lack error\n");
					return 5;
				default:
					printf("Unexpected error occurred\n");
					return 6;
			}
			
			if (cnt == 0)
			{
				printf("There are no natural numbers within 100 that are multiples of %d\n", integer);
			}
			else
			{
				printf("There are %d natural numbers within 100 that are multiples of %d:\n", cnt, integer);
				for (int i = 0; i < cnt; ++i)
				{
					printf("%d ", multiples[i]);
				}
				printf("\n");
			}
			free(multiples);
			break;
		}
		case 'p': // prime check
		{
			switch (is_prime(integer))
			{
				case PSC_PRIME:
					printf("%d is a prime\n", integer);
					break;
				case PSC_COMPOSITE:
					printf("%d is a composite\n", integer);
					break;
				case PSC_NONE:
					printf("%d is neither a prime nor composite\n", integer);
					break;
			}
			break;
		}
		case 's': // number separation
		{
			int cnt = 0;
			int* digits = NULL;
			
			switch (separate_into_digits(integer, &cnt, &digits))
			{
				case OK:
					break;
				case OVERFLOW:
					printf("Entered integer caused an overflow\n");
					return 4;
				case BAD_ALLOC:
					printf("Memory lack error\n");
					return 5;
				default:
					printf("Unexpected error occurred\n");
					return 6;
			}
			
			printf("See this cool digits of %d:\n", integer);
			for (int i = 0; i < cnt; ++i)
			{
				printf("%d ", digits[i]);
			}
			printf("\n");
			free(digits);
			break;
		}
		case 'e': // power table
		{
			if (integer < 1 || integer > 10)
			{
				printf("Invalid input\n");
				return 1;
			}
			printf("Power table:\n");
			printf(" x ");
			for (int pow = 2; pow <= integer; ++pow)
			{
				for (int i = 2 + get_int_len(pow); i <= pow; ++i) 
				{
					printf(" ");
				}
				printf("x^%d ", pow);
			}
			printf("\n");
			
			for (int base = 1; base <= 10; ++base)
			{
				long long res = 1;

				for (int pow = 1; pow <= integer; ++pow)
				{
					res *= base;
					for (int i = get_int_len(res); i <= pow; ++i) 
					{
						printf(" ");
					}
					printf("%lld ", res);
				}
				printf("\n");
			}
			break;
		}
		case 'a': // natural numbers sum
		{
			unsigned long long result = 1ull;
			
			switch (sum_natural_numbers(integer, &result))
			{
				case OK:
					printf("The sum of natural numbers from 1 to %d is %llu\n", integer, result);
					break;
				case INVALID_INPUT:
					printf("The sum cannot be computed: %d is not a natural number\n", integer);
					break;
				case OVERFLOW:
					printf("Computing the sum of natural numbers from 1 to %d caused overflow\n", integer);
					break;
				default:
					printf("Unexpected error occurred\n");
					return 6;
			}
			break;
		}
		case 'f': // factorial
		{
			unsigned long long result = 1ull;
			
			switch (factorial(integer, &result))
			{
				case OK:
					printf("The factorial of %d is %llu\n", integer, result);
					break;
				case INVALID_INPUT:
					printf("The factorial of %d cannot be computed\n", integer);
					break;
				case OVERFLOW:
					printf("Computing the factorial of %d caused an overflow\n", integer);
					break;
				default:
					printf("Unexpected error occurred\n");
					return 6;
			}
			break;
		}
		default:
			break;
	}
}