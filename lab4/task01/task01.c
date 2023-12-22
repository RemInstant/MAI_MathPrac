#include "utility.h"
#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: cmd_path <input> <output>\n");
		return OK;
	}
	if (argc != 3 || !strcmp(argv[1], argv[2]))
	{
		print_error(INVALID_INPUT, 1);
		return INVALID_INPUT;
	}
	
	FILE* input = fopen(argv[1], "r");
	if (input == NULL)
	{
		print_error(FILE_OPENING_ERROR, 1);
		return FILE_OPENING_ERROR;
	}
	FILE* output = fopen(argv[2], "w");
	if (input == NULL)
	{
		fclose(input);
		print_error(FILE_OPENING_ERROR, 1);
		return FILE_OPENING_ERROR;
	}
	
	hash_table macro;
	htable_set_null(&macro);
	status_code err_code = htable_construct(&macro, calc_default_hash);
	int macro_flag = 1;
	char* word = NULL;
	while (!err_code && macro_flag)
	{
		char* key = NULL;
		char* value = NULL;
		err_code = err_code ? err_code : fread_word(input, &word);
		if (!err_code && !strcmp(word, "#define"))
		{
			free(word);
			err_code = err_code ? err_code : fskip_separators(input);
			err_code = err_code ? err_code : fread_word(input, &key);
			err_code = err_code ? err_code : fskip_separators(input);
			err_code = err_code ? err_code : fread_line(input, &value);
			err_code = err_code ? err_code : htable_set_value(&macro, key, value);
		}
		else
		{
			macro_flag = 0;
		}
		free(key);
		free(value);
	}
	fseek(input, -1, SEEK_CUR);
	char test = getc(input);
	if (is_separator(test))
	{
		ungetc(test, input);
		fseek(input, -strlen(word), SEEK_CUR);
	}
	else
	{
		fseek(input, -strlen(word), SEEK_CUR);
	}
	free(word);
	word = NULL;
	
	while (!err_code)
	{
		char ch = getc(input);
		while(is_separator(ch))
		{
			putc(ch, output);
			ch = getc(input);
		}
		if (ch != EOF)
		{
			fseek(input, -1, SEEK_CUR);
		}
		else
		{
			err_code = FILE_END;
		}
		err_code = err_code ? err_code : fread_word(input, &word);
		int is_contained = 0;
		err_code = err_code ? err_code : htable_contains(&macro, word, &is_contained);
		if (!err_code && is_contained)
		{
			char* value = NULL;
			err_code = htable_get_value(&macro, word, &value);
			fprintf(output, "%s", value);
			free(value);
		}
		else if (!err_code)
		{
			fprintf(output, "%s", word);
		}
		if (!err_code)
		{
			fseek(input, -1, SEEK_CUR);
			ch = getc(input);
			if (is_separator(ch))
			{
				fprintf(output, "%c", ch);
			}
		}
		free(word);
		word = NULL;
	}
	if (err_code == FILE_END)
	{
		err_code = OK;
	}
	
	fclose(input);
	fclose(output);
	if (err_code)
	{
		print_error(err_code, 1);
	}
	return err_code;
}
