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

status_codes modify_name_to_output(char* input_name, char** output_name)
{
	if (input_name == NULL || output_name == NULL)
	{
		return INVALID_INPUT;
	}
	
	int len = strlen(input_name);
	int last_sep_pos = len-1; // last position of a separator, i.e. '/', '\'
	int dot_pos = len-1; // last position of a dot
	while (input_name[last_sep_pos] != '\\' && input_name[last_sep_pos] != '/' && last_sep_pos > -1)
	{
		--last_sep_pos;
	}
	while (input_name[dot_pos] != '.' && dot_pos > 0)
	{
		--dot_pos;
	}
	if (dot_pos == 0)
	{
		dot_pos = len;
	}
	
	int out_len = 3 + dot_pos - last_sep_pos;
	(*output_name) = (char*) malloc(sizeof(char) * (out_len + 1));
	if (*output_name == NULL)
	{
		return BAD_ALLOC;
	}
	sprintf(*output_name, "out_");
	(*output_name)[out_len] = '\0';
	for (int i = 4; i < out_len; ++i)
	{
		(*output_name)[i] = input_name[last_sep_pos - 3 + i];
	}
	return OK;
}

status_codes do_d(FILE* input_file, FILE* output_file)
{
	if (input_file == NULL || output_file == NULL)
	{
		return INVALID_INPUT;
	}
	
	char ch;
	while ((ch = fgetc(input_file)) != EOF) 
	{
		if (!isdigit(ch))
		{
			fputc(ch, output_file);
		}
	}
	return OK;
}

status_codes do_i(FILE* input_file, FILE* output_file)
{
	if (input_file == NULL || output_file == NULL)
	{
		return INVALID_INPUT;
	}
	
	char ch = 1;
	unsigned int counter = 0;
	int overflow = 0;
	
	while (ch != EOF)
	{
		ch = 0;
		counter = 0;
		overflow = 0;
		
		while (ch != '\n' && ch != EOF)
		{
			ch = fgetc(input_file);
			
			if (ch != '\n' && ch != EOF && !overflow)
			{
				if (isalpha(ch))
				{
					if (counter > UINT_MAX - 1)
					{
						overflow = 1;
					}
					else
					{
						++counter;
					}
				}
			}
		}
		
		if (overflow)
		{
			fprintf(output_file, "overflow\n");
		}
		else
		{
			fprintf(output_file, "%d\n", counter);
		}
	}
	return OK;
}

status_codes do_s(FILE* input_file, FILE* output_file)
{
	if (input_file == NULL || output_file == NULL)
	{
		return INVALID_INPUT;
	}
	
	char ch = 1;
	unsigned int counter = 0;
	int overflow = 0;
	
	while (ch != EOF)
	{
		ch = 0;
		counter = 0;
		overflow = 0;
		
		while (ch != '\n' && ch != EOF)
		{
			ch = fgetc(input_file);
			
			if (ch != '\n' && ch != EOF && !overflow)
			{
				if (!isalpha(ch) && !isdigit(ch) && ch != ' ')
				{
					if (counter > UINT_MAX - 1)
					{
						overflow = 1;
					}
					else
					{
						++counter;
					}
				}
			}
		}
		
		if (overflow)
		{
			fprintf(output_file, "overflow\n");
		}
		else
		{
			fprintf(output_file, "%d\n", counter);
		}
	}
	return OK;
}

status_codes do_a(FILE* input_file, FILE* output_file)
{
	if (input_file == NULL || output_file == NULL)
	{
		return INVALID_INPUT;
	}
	
	char ch;
	while ((ch = fgetc(input_file)) != EOF)
	{
		if (isdigit(ch))
		{
			fputc(ch, output_file);
		}
		else
		{
			fprintf(output_file, "%X", ch);
		}
	}
	return OK;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: command_name <flag> <input file>\n");
		printf("flags:\n");
		printf("-[n]d  -  exclude Arabic numerals from file\n");
		printf("-[n]i  -  count occurrence of Latin letters for each line\n");
		printf("-[n]s  -  count occurrence of characters other than Latin letters,");
		printf(" Arabic numerals and spaces for each line\n");
		printf("-[n]a  -  change characters other than Arabic numerals with their hexadecimal ASCII code\n");
		printf("The result is written to a file.\n");
		printf("If flag -n defined the output file name must be the third argument of command.");
		printf(" Otherwise, its name is concatenation of prefix \"out_\" and input file name\n");
		return 0;
	}
	
	char* flag = argv[1];
	char main_flag = ' ';
	int output_flag = 0;
	
	if (flag[0] == '-' || flag[0] == '/')
	{
		if (flag[2] == '\0')
		{
			main_flag = flag[1];
		}
		else if (flag[3] == '\0' && flag[1] == 'n')
		{
			main_flag = flag[2];
			output_flag = 1;
		}
	}
	
	if (main_flag != 'd' && main_flag != 'i' && main_flag != 's' && main_flag != 'a')
	{
		printf("Invalid flag\n");
		return 2;
	}
	
	if (argc != (3 + output_flag))
	{
		printf("Invalid input\n");
		return 1;
	}
	
	char* input_name = argv[2];
	char* output_name = NULL;
	
	if (output_flag)
	{
		output_name = argv[3];
	}
	else
	{	
		switch (modify_name_to_output(input_name, &output_name))
		{
			case OK:
				break;
			case BAD_ALLOC:
				printf("Memory lack error\n");
				return 3;
			default:
				printf("Unexpected error occurred\n");
				return 5;
		}
	}
	
	FILE* input_file;
	FILE* output_file;
	
	if ((input_file = fopen(input_name, "r")) == NULL)
	{
		printf("Input file cannot be opened\n");
	}
	else if ((output_file = fopen(output_name, "w")) == NULL)
	{
		fclose(input_file);
		printf("Output file cannot be opened\n");
	}
	
	if (input_file == NULL || output_file == NULL)
	{
		if (output_flag)
		{
			free(output_name);
		}
		return 4;
	}
	
	switch (main_flag)
	{
		case 'd':
			do_d(input_file, output_file);
			break;
		case 'i':
			do_i(input_file, output_file);
			break;
		case 's':
			do_s(input_file, output_file);
			break;
		case 'a':
			do_a(input_file, output_file);
			break;
	}
	
	fclose(input_file);
	fclose(output_file);
	
	if (output_flag)
	{
		free(output_name);
	}
}