#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>

typedef long long ll;
typedef unsigned long long ull;

typedef enum
{
	OK,
	INVALID_INPUT,
	INVALID_FLAG,
	INVALID_NUMBER,
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

status_codes str_to_ll(char* str_ll, int base, ll* res)
{
	char* pointer;
	errno = 0;
	*res = strtoll(str_ll, &pointer, base);
	if (errno == ERANGE)
	{
		return OVERFLOW;
	}
	if (*pointer != '\0')
	{
		return INVALID_NUMBER;
	}
	return OK;
}

status_codes validate_input(int argc, char** argv)
{
	if (argc < 3)
	{
		return INVALID_INPUT;
	}
	
	char flag = argv[1][1];
	if(argv[1][0] != '-' || argv[1][2] != '\0')
	{
		return INVALID_FLAG;
	}
	if(flag != 'l' && flag != 'r' && flag != 'u' && flag != 'n' && flag != 'c')
	{
		return INVALID_FLAG;
	}
	if (flag == 'c')
	{
		if (argc < 4)
		{
			return INVALID_INPUT;
		}
		ll number;
		status_codes transform_code = str_to_ll(argv[3], 10, &number);
		if(transform_code != OK)
		{
			return transform_code;
		}
		if (number < 0 || number > UINT_MAX)
		{
			return INVALID_NUMBER;
		}
	}
	return OK;
}

int rand_comp(const void* a, const void* b)
{
	return rand() & 1;
}

status_codes select_strings(int argc, char** argv, char*** strings)
{
	if (argv == NULL || strings == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	int cnt = argc - 3;
	cnt = (cnt < 1) ? 1 : cnt;
	*strings = (char**) malloc(sizeof(char*) * cnt);
	if (*strings == NULL)
	{
		return BAD_ALLOC;
	}
	(*strings)[0] = argv[2];
	for (int i = 1; i < cnt; ++i)
	{
		(*strings)[i] = argv[3 + i];
	}
	return OK;
}

status_codes str_len(char* str, ull* len)
{
	if (str == NULL || len == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	*len = 0;
	while(str[*len] && *len < ULLONG_MAX)
	{
		++(*len);
	}
	if (str[*len] != '\0')
	{
		return OVERFLOW;
	}
	return OK;
}

status_codes str_reverse(char* str, char** res)
{
	if (str == NULL || res == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	ull len;
	status_codes len_code = str_len(str, &len);
	if (len_code != OK)
	{
		return len_code;
	}
	*res = (char*) malloc(sizeof(char) * (len + 1));
	if (*res == NULL)
	{
		return BAD_ALLOC;
	}
	for (ull i = 0; i < len; ++i)
	{
		(*res)[i] = str[len-i-1];
	}
	(*res)[len] = '\0';
	return OK;
}

status_codes odd_to_upper(char* str, char** res)
{
	if (str == NULL || res == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	ull len;
	status_codes len_code = str_len(str, &len);
	if (len_code != OK)
	{
		return len_code;
	}
	*res = (char*) malloc(sizeof(char) * (len + 1));
	if (*res == NULL)
	{
		return BAD_ALLOC;
	}
	for (ull i = 0; i < len; ++i)
	{
		if ((i & 1) && isalpha(str[i]))
		{
			(*res)[i] = toupper(str[i]);
		}
		else
		{
			(*res)[i] = str[i];
		}
	}
	(*res)[len] = '\0';
	return OK;
}

status_codes group_chars(char* str, char** res)
{
	if (str == NULL || res == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	ull len;
	status_codes len_code = str_len(str, &len);
	if (len_code != OK)
	{
		return len_code;
	}
	*res = (char*) malloc(sizeof(char) * (len + 1));
	if (*res == NULL)
	{
		return BAD_ALLOC;
	}
	ull j = 0;
	for (ull i = 0; i < len; ++i)
	{
		if (isdigit(str[i]))
		{
			(*res)[j++] = str[i];
		}
	}
	for (ull i = 0; i < len; ++i)
	{
		if (isalpha(str[i]))
		{
			(*res)[j++] = str[i];
		}
	}
	for (ull i = 0; i < len; ++i)
	{
		if (!isalnum(str[i]))
		{
			(*res)[j++] = str[i];
		}
	}
	(*res)[len] = '\0';
	return OK;
}

status_codes random_cat(ull cnt, char** strings, unsigned seed, char** res) 
{
	if (strings == NULL || res == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	ull res_len = 0;
	for (ull i = 0; i < cnt; ++i)
	{
		ull len;
		status_codes len_code = str_len(strings[i], &len);
		if (len_code != OK)
		{
			return len_code;
		}
		if (res_len > ULLONG_MAX - len)
		{
			return OVERFLOW;
		}
		res_len += len;
	}
	*res = (char*) malloc(sizeof(char) * (res_len + 1));
	if (*res == NULL)
	{
		return BAD_ALLOC;
	}
	
	char** shuffled_strings = (char**) malloc(sizeof(char*) * cnt);
	if (shuffled_strings == NULL)
	{
		free(*res);
		return BAD_ALLOC;
	}
	for (ull i = 0; i < cnt; ++i)
	{
		shuffled_strings[i] = strings[i];
	}
	
	srand(seed);
	qsort(shuffled_strings, cnt, sizeof(char*), rand_comp);
	for (ull i = 0, k = 0; i < cnt; ++i)
	{
		for (ull j = 0; shuffled_strings[i][j]; ++j, ++k)
		{
			(*res)[k] = shuffled_strings[i][j];
		}
	}
	free(shuffled_strings);
	(*res)[res_len] = '\0';
	return OK;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: ...no one has ever read the usage, so why should I try to write it?\n");
		return 0;
	}
	
	status_codes valid_code = validate_input(argc, argv);
	if (valid_code != OK)
	{
		print_error(valid_code);
		return valid_code;
	}
	
	char flag = argv[1][1];
	char* main_str = argv[2];
	
	switch (flag)
	{
		case 'l':
		{
			ull len;
			status_codes len_code = str_len(main_str, &len);
			if (len_code != OK)
			{
				print_error(len_code);
				return len_code;
			}
			printf("str length: %llu\n", len);
			break;
		}
		case 'r':
		{
			char* rev_str;
			status_codes rev_code = str_reverse(main_str, &rev_str);
			if (rev_code != OK)
			{
				print_error(rev_code);
				return rev_code;
			}
			printf("reversed str: %s\n", rev_str);
			free(rev_str);
			break;
		}
		case 'u':
		{
			char* odd_str;
			status_codes odd_code = odd_to_upper(main_str, &odd_str);
			if (odd_code != OK)
			{
				print_error(odd_code);
				return odd_code;
			}
			printf("odd str: %s\n", odd_str);
			free(odd_str);
			break;
		}
		case 'n':
		{
			char* grouped_str;
			status_codes group_code = group_chars(main_str, &grouped_str);
			if (group_code != OK)
			{
				print_error(group_code);
				return group_code;
			}
			printf("grouped str: %s\n", grouped_str);
			free(grouped_str);
			break;
		}
		case 'c':
		{
			unsigned seed = (unsigned) strtoll(argv[3], NULL, 10);
			char** strings;
			status_codes select_code = select_strings(argc, argv, &strings);
			if (select_code != OK)
			{
				print_error(select_code);
				return select_code;
			}
			ull cnt = argc - 3;
			cnt = (cnt < 1) ? 1 : cnt;
			char* res;
			status_codes cat_code = random_cat(cnt, strings, seed, &res);
			if (cat_code != OK)
			{
				free(strings);
				print_error(cat_code);
				return cat_code;
			}
			printf("rand str: %s\n", res);
			free(strings);
			free(res);
			break;
		}
		default:
			break;
	}
}