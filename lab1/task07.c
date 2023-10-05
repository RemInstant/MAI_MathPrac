#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int is_separator(char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n';
}

void do_r(FILE* file1, FILE* file2, FILE* output)
{
	char ch1 = ' ', ch2 = ' ';
	
	while (is_separator(ch1))
	{
		ch1 = getc(file1);
	}
	while (is_separator(ch2))
	{
		ch2 = getc(file2);
	}
	
	int turn = ch1 != EOF; // 1 - file1, 0 - file2
	
	while (ch1 != EOF || ch2 != EOF)
	{
		if (turn)
		{
			while (!is_separator(ch1) && ch1 != EOF)
			{
				fputc(ch1, output);
				ch1 = getc(file1);
			}
			while (is_separator(ch1))
			{
				ch1 = getc(file1);
			}
		}
		else
		{
			while (!is_separator(ch2) && ch2 != EOF)
			{
				fputc(ch2, output);
				ch2 = getc(file2);
			}
			while (is_separator(ch2))
			{
				ch2 = getc(file2);
			}
		}
		
		turn = turn ? (ch2 == EOF) : (ch1 != EOF);
		
		if (ch1 != EOF || ch2 != EOF)
		{
			fputc(' ', output);
		}
	}
}

void do_a(FILE* input, FILE* output)
{
	char ch = ' ';
	int counter = 1;
	
	while (is_separator(ch))
	{
		ch = getc(input);
	}
	while (ch != EOF)
	{
		while (!is_separator(ch) && ch != EOF)
		{
			if (counter % 10 == 0)
			{
				if (isalpha(ch))
				{
					ch = tolower(ch);
				}
				for (int i = 3; i >= 0; --i)
				{
					// mask 00000011
					fprintf(output, "%d", (ch & (3 << 2*i)) >> 2*i);
				}
			}
			else if (counter % 5 == 0)
			{
				if (isalpha(ch))
				{
					ch = tolower(ch);
				}
				fputc(ch, output);
			}
			else if (!(counter & 1))
			{
				for (int i = 2; i >= 0; --i)
				{
					// mask 00000111
					fprintf(output, "%d", (ch & (7 << 3*i)) >> 3*i);
				}
			}
			else
			{
				fputc(ch, output);
			}
			
			ch = getc(input);
		}
		
		while (is_separator(ch))
		{
			ch = getc(input);
		}
		
		counter %= 10;
		++counter;
		
		if (ch != EOF)
		{
			fputc(' ', output);
		}
	}
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: command_name <flag>\n");
		printf("flags:\n");
		printf("-r <file1> <file2> <output file>  -  Write words in turns from files 1 and 2 into the output file.\n");
		printf("                                     If one of it ends, the remaining words of the other file are written in a row.\n");
		printf("-a <input file> <output file>     -  Write words from the input file to the output one according to the following rules:\n");
		printf("                                     1. The letters of every 10th word are turned into small letters.\n");
		printf("                                        Then all the characters of this word are converted into their ASCII code\n");
		printf("                                        in the quaternary number system.\n");
		printf("                                     2. The letters of every 2nd (but not 10th) word are turned into small letters.\n");
		printf("                                     3. The characters of every 5th (but not 10th) word are converted into\n");
		printf("                                        their ASCII code in the octal number system.\n");
		return 0;
	}
	
	char* flag = argv[1];
	
	if (((flag[0] != '-') && (flag[0] != '/')) || flag[2] != '\0' 
			|| (flag[1] != 'r' && flag[1] != 'a'))
	{
		printf("Invalid flag\n");
		return 2;
	}
	
	FILE* input_file1 = NULL;
	FILE* input_file2 = NULL;
	FILE* output_file = NULL;
	
	switch (flag[1])
	{
		case 'r':
		{
			if (argc != 5)
			{
				printf("Invalid input\n");
				return 1;
			}
			if ((input_file1 = fopen(argv[2], "r")) == NULL)
			{
				printf("Cannot open the file1");
				return 3;
			}
			if ((input_file2 = fopen(argv[3], "r")) == NULL)
			{
				fclose(input_file1);
				printf("Cannot open the file2");
				return 3;
			}
			if ((output_file = fopen(argv[4], "w")) == NULL)
			{
				fclose(input_file1);
				fclose(input_file2);
				printf("Cannot open the output file");
				return 3;
			}
			break;
		}
		case 'a':
		{
			if (argc != 4)
			{
				printf("Invalid input\n");
				return 1;
			}
			if ((input_file1 = fopen(argv[2], "r")) == NULL)
			{
				printf("Cannot open the file1");
				return 3;
			}
			if ((output_file = fopen(argv[3], "w")) == NULL)
			{
				fclose(input_file1);
				printf("Cannot open the output file");
				return 3;
			}
			break;
		}
	}

	switch (flag[1])
	{
		case 'r':
			do_r(input_file1, input_file2, output_file);
			fclose(input_file1);
			fclose(input_file2);
			fclose(output_file);
			break;
		case 'a':
			do_a(input_file1, output_file);
			fclose(input_file1);
			fclose(output_file);
			break;
	}
}