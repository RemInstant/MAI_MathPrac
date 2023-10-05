#include <stdio.h>
#include <stdlib.h>
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
	if(isdigit(ch))
	{
		return ch - '0';
	}
	if(isalpha(ch))
	{
		return ch - 'A' + 10;
	}
	return -1;
}

status_codes to_decimal(char* number_str, int base, long long* res)
{
	*res = 0;
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
	return OK;
}

status_codes parse_numbers(FILE* input, FILE* output)
{
	char ch = ' ';
	
	while (is_separator(ch))
	{
		ch = getc(input);
	}
	while (ch != EOF)
	{
		char max_ch = -1;
		int iter = 0;
		int size = 2;
		char* number_str = (char*) malloc(sizeof(char) * size);
		if (number_str == NULL)
		{
			return BAD_ALLOC;
		}
		
		while (!is_separator(ch) && ch != EOF)
		{
			if (iter > size - 2)
			{
				size *= 2;
				char* temp_str = realloc(number_str, size);
				if (temp_str == NULL)
				{
					free(number_str);
					return BAD_ALLOC;
				}
				number_str = temp_str;
			}
			
			if (!isdigit(ch) && !isalpha(ch))
			{
				free(number_str);
				return INVALID_INPUT;
			}
			
			if (isalpha(ch))
			{
				ch = toupper(ch);
			}
			if (ch > max_ch)
			{
				max_ch = ch;
			}
			if (size != 2 || ch != '0')
			{
				number_str[iter] = ch;
				++iter;
			}
			ch = getc(input);
		}
		
		if (iter == 0)
		{
			number_str[0] = '0';
			++iter;
		}
		number_str[iter] = '\0';
		
		int base = char_to_int(max_ch) + 1;
		if (base < 2)
		{
			base = 2;
		}
		
		long long res;
		status_codes dec_code = to_decimal(number_str, base, &res);
		if (dec_code != OK)
		{
			free(number_str);
			return dec_code;
		}
		
		fprintf(output, "%s %d %lld", number_str, base, res);
		free(number_str);
		
		while (is_separator(ch))
		{
			ch = getc(input);
		}
		if (ch != EOF)
		{
			fprintf(output, "\n");
		}
	}
	return OK;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: command_name <input file> <output file>\n");
		printf("(Files containing only digits and letters can be processed)\n");
		printf("For each number of input file program figures out the minimum base (X) of numeral system");
		printf(" this number can be represented in.\n");
		printf("Then, the number is converted from this numeral system into decimal one\n");
		printf("Output format:\n");
		printf("<base-X number> <X> <base-10 number>\n");
		return 0;
	}
	
	if(argc != 3) {
		printf("Invalid input\n");
		return 1;
	}
	
	FILE* input_file = NULL;
	FILE* output_file = NULL;
	
	if ((input_file = fopen(argv[1], "r")) == NULL)
	{
		printf("Cannot open the input file");
		return 2;
	}
	if ((output_file = fopen(argv[2], "w")) == NULL)
	{
		fclose(input_file);
		printf("Cannot open the output file");
		return 2;
	}
	
	status_codes parse_code = parse_numbers(input_file, output_file);
	
	fclose(input_file);
	fclose(output_file);
	
	switch (parse_code)
	{
		case OK:
			break;
		case INVALID_INPUT:
			printf("The input file contains invalid data\n");
			return 3;
		case OVERFLOW:
			printf("An overflow occurred while processing the input file\n");
			return 4;
		case BAD_ALLOC:
			printf("A memory lack error occured while processing the input file\n");
			return 5;
		default:
			printf("Unexpected error occurred\n");
			return 6;
	}
}