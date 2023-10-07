#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

typedef enum
{
	OK,
	INVALID_INPUT,
	OVERFLOW,
	BAD_ALLOC
} status_codes;

int is_separator(char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n';
}

int char_to_int(char ch)
{
	if (isdigit(ch))
	{
		return ch - '0';
	}
	if (isalpha(ch))
	{
		return toupper(ch) - 'A' + 10;
	}
	return -1;
}

char int_to_char(unsigned integer)
{
	if (integer < 10)
	{
		return '0' + integer;
	}
	if (integer < 36)
	{
		return 'A' + integer - 10;
	}
	return -1;
}

int is_stop(char* str)
{
	if (str == NULL)
	{
		return 0;
	}
	return str[0] == 's' && str[1] == 't' && str[2] == 'o' && str[3] == 'p' && !str[4];
}

status_codes read_word(char** word, int* size)
{
	int iter = 0;
	*size = 2;
	*word = (char*) malloc(sizeof(char) * *size);
	
	if (*word == NULL)
	{
		return BAD_ALLOC;
	}
	
	char ch;
	scanf("%c", &ch);
	
	while (!is_separator(ch))
	{
		if (iter > *size - 2)
		{
			*size *= 2;
			char* temp_word = realloc(*word, *size);
			if (temp_word == NULL)
			{
				free(*word);
				return BAD_ALLOC;
			}
			*word = temp_word;
		}
		
		if (*size != 2 || ch != '0')
		{
			(*word)[iter] = ch;
			++iter;
		}
		
		scanf("%c", &ch);
	}
	
	(*word)[iter] = '\0';
	
	return OK;
}

status_codes validate_based_str_integer(char* str_int, int base)
{
	int integer = 0;
	
	if (str_int[0] != '-')
	{
		for (int i = 0; str_int[i]; ++i)
		{
			if (!isdigit(str_int[i]) && !isalpha(str_int[i]))
			{
				return INVALID_INPUT;
			}
			
			int add = char_to_int(str_int[i]);
			
			if (add >= base)
			{
				return INVALID_INPUT;
			}
			if (integer > (LLONG_MAX - add) / base)
			{
				return OVERFLOW;
			}
			
			integer = integer * base + add;
		}
	}
	else
	{
		if (str_int[1] == '\0')
		{
			return INVALID_INPUT;
		}
		for (int i = 1; str_int[i]; ++i)
		{
			if (!isdigit(str_int[i]) && !isalpha(str_int[i]))
			{
				return INVALID_INPUT;
			}
			
			int subtr = char_to_int(str_int[i]);
			
			if (subtr >= base)
			{
				return INVALID_INPUT;
			}
			if (integer < (LLONG_MIN + subtr) / base)
			{
				return OVERFLOW;
			}
			
			integer = integer * base - subtr;
		}
	}
	return OK;
}

status_codes to_decimal(char* number_str, int base, long long* res)
{
	*res = 0;
	if (number_str[0] != '-')
	{
		for (int i = 0; number_str[i]; ++i)
		{
			int add = char_to_int(number_str[i]);
			if (*res > (LLONG_MAX - add) / base)
			{
				return OVERFLOW;
			}
			*res *= base;
			*res += add;
		}
	}
	else
	{
		for (int i = 1; number_str[i]; ++i)
		{
			int subtr = char_to_int(number_str[i]);
			if (*res < (LLONG_MIN + subtr) / base)
			{
				return OVERFLOW;
			}
			*res *= base;
			*res -= subtr;
		}
	}
	return OK;
}

status_codes to_base_n(long long integer, int base, char res[])
{
	if (res == NULL)
	{
		return INVALID_INPUT;
	}
	
	int iter = 0;
	long long divider = 1;
	
	if (integer < 0)
	{
		res[iter++] = '-';
		while (divider <= LLONG_MAX / base && -divider * base > integer)
		{
			divider *= base;
		}
	}
	else
	{
		while (divider <= LLONG_MAX / base && divider * base < integer)
		{
			divider *= base;
		}
	}
	
	while (divider > 0)
	{
		res[iter++] = int_to_char(llabs(integer / divider % base));
		divider /= base;
	}
	res[iter] = '\0';
	
	return OK;
}

status_codes handle_input(int base, char** str_res, long long* int_res)
{
	int size;
	char* str = NULL;
	char* max_str = NULL;
	
	long long integer;
	long long max_integer;
	
	while (!is_stop(str))
	{
		if (str != NULL && str[0])
		{
			status_codes validate_code = validate_based_str_integer(str, base);
			if (validate_code != OK)
			{
				free(str);
				if (max_str != NULL)
				{
					free(max_str);
				}
				return validate_code;
			}
			
			status_codes dec_code = to_decimal(str, base, &integer);
			if (dec_code != OK)
			{
				free(str);
				if (max_str != NULL)
				{
					free(max_str);
				}
				return dec_code;
			}
			
			if (max_str == NULL)
			{
				max_str = str;
				max_integer = integer;
			}
			else
			{
				if (llabs(integer) > llabs(max_integer))
				{
					free(max_str);
					max_str = str;
					max_integer = integer;
				}
				else
				{
					free(str);
				}
			}
		}
		
		status_codes read_code = read_word(&str, &size);
		if (read_code != OK)
		{
			if (max_str != NULL)
			{
				free(max_str);
			}
			return read_code;
		}
	}
	
	if (str != max_str)
	{
		free(str);
	}
	
	*str_res = max_str;
	*int_res = max_integer;
	return OK;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: command_name <base>\n");
		printf("Program takes as input numbers of defined numeral system (2 <= base <= 36),\n");
		printf("find maximum absolute value among them and print it in bases 9, 18, 27, 36.\n");
		printf("(Enter \"stop\" to end input phase)\n");
		return 0;
	}
	
	if (argc != 2)
	{
		printf("Invalid input\n");
		return 1;
	}
	
	if (validate_based_str_integer(argv[1], 10) != OK)
	{
		printf("Invalid integer\n");
		return 1;
	}
	
	int base = atoi(argv[1]);
	
	if (base < 2 || base > 36)
	{
		printf("Invalid input: base must be not less than 2 and not greater than 36\n");
		return 1;
	}
	
	char* max_str = NULL;
	long long max_integer;
	
	switch (handle_input(base, &max_str, &max_integer))
	{
		case OK:
			break;
		case INVALID_INPUT:
			printf("Invalid integer\n");
			return 1;
		case OVERFLOW:
			printf("Entered integer caused an overflow\n");
			return 2;
		case BAD_ALLOC:
			printf("Memory lack error\n");
			return 3;
		default:
			printf("Unexpected error occurred\n");
			return 4;
	}
	
	if (max_str == NULL)
	{
		printf("No integers entered\n");
		return 0;
	}
	
	char str_int9[66];
	char str_int18[66];
	char str_int27[66];
	char str_int36[66];
	
	to_base_n(max_integer, 9, str_int9);
	to_base_n(max_integer, 18, str_int18);
	to_base_n(max_integer, 27, str_int27);
	to_base_n(max_integer, 36, str_int36);
	
	printf("Maximum absolute value (base-%d): %s\n", base, max_str);
	printf("in base-9: %s\n", str_int9);
	printf("in base-18: %s\n", str_int18);
	printf("in base-27: %s\n", str_int27);
	printf("in base-36: %s\n", str_int36);
	free(max_str);
}