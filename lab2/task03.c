#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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

typedef struct
{
	ull line, letter;
} occurrence;

status_codes handle_file(char* path, char* pattern, ull* occ_cnt, occurrence** occ)
{
	if (path == NULL || pattern == NULL || occ == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	
	FILE* input = fopen(path, "r");
	if (input == NULL)
	{
		return FILE_OPENING_ERROR;
	}
	
	ull occ_size = 2;
	*occ_cnt = 0;
	*occ = (occurrence*) malloc(sizeof(occurrence) * occ_size);
	if (*occ == NULL)
	{
		fclose(input);
		return BAD_ALLOC;
	}
	
	ull buf_size = strlen(pattern);
	char* buf = (char*) malloc(sizeof(char) * (buf_size + 1));
	if (buf == NULL)
	{
		fclose(input);
		free(*occ);
		return BAD_ALLOC;
	}
	
	char ch = ' ';
	buf[buf_size] = '\0';
	for (int i = 0; i < buf_size; ++i)
	{
		ch = getc(input);
		if (ch != EOF)
		{
			buf[i] = ch;
		}
		else
		{
			fclose(input);
			free(buf);
			return OK;
		}
	}
	
	ull line = 1, letter = 1;
	while (ch != EOF)
	{
		if (!(strcmp(buf, pattern)))
		{
			if (*occ_cnt == occ_size)
			{
				occ_size *= 2;
				occurrence* tmp = (occurrence*) realloc(*occ, sizeof(occurrence)* occ_size);
				if (tmp == NULL)
				{
					fclose(input);
					free(buf);
					free(*occ);
					return BAD_ALLOC;
				}
				*occ = tmp;
			}
			occurrence new_occ;
			new_occ.line = line;
			new_occ.letter = letter;
			(*occ)[(*occ_cnt)++] = new_occ;
		}
		
		++letter;
		if (buf[0] == '\n')
		{
			++line;
			letter = 1;
		}
		
		ch = getc(input);
		for (int i = 1; i < buf_size; ++i)
		{
			buf[i-1] = buf[i];
		}
		buf[buf_size-1] = ch;
	}
	
	fclose(input);
	free(buf);
	return OK;
}

status_codes find_occurences(char* pattern, ull** occ_cnts, occurrence*** occs, ull file_cnt, ...)
{
	if (pattern == NULL || occs == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	
	*occ_cnts = (ull*) malloc(sizeof(ull*) * file_cnt);
	if (*occ_cnts == NULL)
	{
		return BAD_ALLOC;
	}
	
	*occs = (occurrence**) malloc(sizeof(occurrence*) * file_cnt);
	if (*occs == NULL)
	{
		free(*occ_cnts);
		return BAD_ALLOC;
	}
	
	va_list arg;
	va_start(arg, file_cnt);
	for (int i = 0; i < file_cnt; ++i)
	{
		char* path = va_arg(arg, char*);
		
		status_codes code = handle_file(path, pattern, *occ_cnts + i, *occs + i);
		if (code != OK)
		{
			for (int j = 0; j < i; ++i)
			{
				free((*occs)[j]);
			}
			free(*occ_cnts);
			free(*occs);
			return code;
		}
	}
	va_end(arg);
	return OK;
}

int main(int argc, char** argv)
{
	ull* occ_cnts = 0;
	occurrence** occs = NULL;
	
	status_codes code = find_occurences(argv[1], &occ_cnts, &occs, 3, "input", "input2", "output");
	if (code != OK)
	{
		printf("exit code: %d\n", code);
		return code;
	}
	
	printf("Occurrences\n");
	for (ull i = 0; i < 3; ++i)
	{
		printf("file %llu: ", i+1);
		for (ull j = 0; j < occ_cnts[i]; ++j)
		{
			printf("(%llu; %llu)", occs[i][j].line, occs[i][j].letter);
			if (j + 1 < occ_cnts[i])
			{
				printf(", ");
			}
		}
		if (occ_cnts[i] == 0)
		{
			printf("no occurrences :(");
		}
		printf("\n");
	}
}