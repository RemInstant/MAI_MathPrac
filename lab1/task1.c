#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int min(int a, int b) {
	return a < b ? a : b;
}

int validate_string_integer(char* str_int) {
	for (int i = 0; str_int[i] != '\0'; ++i) {
		if (i == 0 && str_int[i] == '-') continue;
		if ('0' <= str_int[i] && str_int[i] <= '9') continue;
		return 0;
	}
	return 1;
}

int check_overflow(char* str_int) {
	int integer = 0;
	int negative = 0;
	
	for (int i = 0; str_int[i] != '\0'; ++i) {
		if (i == 0 && str_int[i] == '-') {
			negative = 1;
			continue;
		}	
		if ('0' > str_int[i] || str_int[i] > '9')
			return 0;
			
		if ((!negative && integer > INT_MAX / 10) || (negative && integer < INT_MIN / 10)) 
			return 1;
		integer *= 10;
		
		int add = str_int[i] - '0';
		if ((!negative && integer > INT_MAX - add) || (negative && integer < INT_MIN + add))
			return 1;
		integer += negative ? -add : add;
	}
	return 0;
}

void get_multiples(int integer, int* cnt, int** multiples) {
	if (cnt == NULL || multiples == NULL) return;
	if (*multiples != NULL) free(*multiples);
	
	integer = abs(integer);
	*cnt = (integer != 0) ? (100 / integer) : 0;
	*multiples = malloc(sizeof(int)*(*cnt));
	
	if (*cnt == 0) return;
	for(int i = integer, j = 0; i <= 100; i += integer, ++j) {
		(*multiples)[j] = i;
	}
}

typedef enum {
	psc_prime,
	psc_composite,
	psc_none
} prime_status_codes;

prime_status_codes is_prime(long long integer) {
	if (integer <= 1) return psc_none;
	if (integer == 2) return psc_prime;
	if (integer % 2 == 0) return psc_composite;
	for (int i = 3; i*i <= integer; i += 2)
		if (integer % i == 0) return psc_composite;
	return psc_prime;
}

typedef enum {
	fsc_ok,
	fsc_overflow,
	fsc_invalid_input
} factorial_status_codes;

factorial_status_codes factorial(long long integer, unsigned long long* result) {
	if(integer < 0) return fsc_invalid_input;
	
	*result = 1ull;
	
	for (int i = 2; i <= integer; ++i) {
		if (*result > ULONG_LONG_MAX / i)
			return fsc_overflow;
		
		*result *= i;
	}
	
	return fsc_ok;
}

typedef enum {
	nnsc_ok,
	nnsc_overflow,
	nnsc_invalid_input
} natural_number_status_codes;

natural_number_status_codes sum_natural_numbers(int integer, unsigned long long* result) {
	if(integer < 0) return nnsc_invalid_input;
	
	*result = integer+1;
	
	if (*result > ULONG_LONG_MAX / integer)
		return nnsc_overflow;
	
	*result *= integer;
	*result /= 2;
	
	return nnsc_ok;
}

int get_int_len(long long integer) {
	if (integer == 0) return 1;
	int len = 0;
	while (integer > 0) {
		integer /= 10;
		++len;
	}
	return len;
}

void separate_into_digits(int integer, int* cnt, int** digits) {
	if (cnt == NULL || digits == NULL) return;
	if (*digits != NULL) free(*digits);
	
	*cnt = get_int_len(integer);
	
	*digits = malloc(sizeof(int)*(*cnt));

	for (int i = *cnt-1; i >= 0; --i) {
		(*digits)[i] = integer % 10;
		integer /= 10;
	}
}

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Invalid input\n");
		printf("Usage: command_name <flag> <32-bit integer>\n");
		printf("flags:\n");
		printf("-h  -  print natural numbers up to 100 that is multiples of <integer>\n");
		printf("-p  -  checks if <integer> is a prime (<integer> must be natural)\n");
		printf("-s  -  separate <integer> into digits\n");
		printf(" by all natural numbers up to <integer> (<integer>\n");
		printf("-e  -  print the exponential of natural numbers up to <integer>");
		printf(" with base from 1 to 10 (<integer> must be not greater than 10)\n");
		printf("-a  -  print the sum of all natural numbers up to <integer>\n");
		printf("-f  -  print the factorial of <integer>\n");
		return 1;
	}
	char* flag = argv[1];
	char* string_int = argv[2];
	
	if (((flag[0] != '-') && (flag[0] != '/')) || flag[2] != '\0' 
			|| (flag[1] != 'h' && flag[1] != 'p' && flag[1] != 's'
			&& flag[1] != 'e' && flag[1] != 'a' && flag[1] != 'f')) {
		printf("Invalid flag\n");
		return 2;
	}
	
	if (!validate_string_integer(string_int)) {
		printf("Invalid integer\n");
		return 3;
	}
	
	if (check_overflow(string_int)) {
		printf("Entered integer caused an overflow\n");
		return 4;
	}
	
	int integer = atoi(string_int);
	
	switch (flag[1]) {
		case 'h': {
			// multiples
			int cnt = 0;
			int* multiples = NULL;
			
			get_multiples(integer, &cnt, &multiples);
			
			if (multiples == NULL) {
				printf("Memory lack error\n");
				return 5;
			}
			
			if (cnt == 0) {
				printf("There are no natural numbers within 100 that are multiples of %d\n", integer);
			} else {
				printf("There are %d natural numbers within 100 that are multiples of %d:\n", cnt, integer);
				for (int i = 0; i < cnt; ++i) {
					printf("%d ", multiples[i]);
				}
				printf("\n");
			}
			free(multiples);
			break;
		}
		case 'p': {
			// prime check
			
			switch (is_prime(integer)) {
				case psc_prime: {
					printf("%d is a prime\n", integer);
					break;
				}
				case psc_composite: {
					printf("%d is a composite\n", integer);
					break;
				}
				case psc_none: {
					printf("%d is neither a prime nor composite\n", integer);
					break;
				}
			}
			
			break;
		}
		case 's': {
			// number separation
			int cnt = 0;
			int* digits = NULL;
			
			separate_into_digits(integer, &cnt, &digits);
			
			if (digits == NULL) {
				printf("Memory lack error\n");
				return 5;
			}
			
			printf("See this cool digits of %d:\n", integer);
			for (int i = 0; i < cnt; ++i) {
				printf("%d ", digits[i]);
			}
			printf("\n");
			free(digits);
			break;
		}
		case 'e': {
			// power table
			printf("Power table:\n");
			for (int base = 1; base <= 10; ++base) {
				long long res = 1;
				
				for (int pow = 1; pow <= min(10, integer); ++pow) {
					res *= base;
					for (int i = get_int_len(res); i <= pow; ++i) printf(" ");
					printf("%lld ", res);
				}
				printf("\n");
			}
			break;
		}
		case 'a': {
			// natural numbers sum
			unsigned long long result = 1ull;
			
			switch (sum_natural_numbers(integer, &result)) {
				case nnsc_ok: {
					printf("Sum of integers from 1 to %d is %llu\n", integer, result);
					break;
				}
				case nnsc_overflow: {
					printf("Computing the sum of integers from 1 to %d caused overflow\n", integer);
					break;
				}
				case nnsc_invalid_input: {
					printf("Sum of integers from 1 to %d cannot be computed\n", integer);
					break;
				}
			}
			
			break;
		}
		case 'f': {
			// factorial
			unsigned long long result = 1ull;
			
			switch (factorial(integer, &result)) {
				case fsc_ok: {
					printf("The factorial of %d is %llu\n", integer, result);
					break;
				}
				case fsc_overflow: {
					printf("Computing the factorial of %d caused an overflow\n", integer);
					break;
				}
				case fsc_invalid_input: {
					printf("The factorial of %d cannot be computed\n", integer);
					break;
				}
			}

			break;
		}
		default:
			break;
	}
}