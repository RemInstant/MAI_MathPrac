#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>

#include "../include/utility.h"

typedef long long ll;
typedef unsigned long long ull;

void print_error(status_code code, int nl_cnt)
{
    fprint_error(stdout, code, nl_cnt);
}

void fprint_error(FILE* file, status_code code, int nl_cnt)
{
    switch (code)
    {
        case OK:
            return;
        case NULL_ARG:
            fprintf(file, "Null argument");
            break;
        case INVALID_INPUT:
            fprintf(file, "Invalid input");
            break;
        case INVALID_ARG:
            fprintf(file, "Invalid argument");
            break;
        case INVALID_FLAG:
            fprintf(file, "Invalid flag");
            break;
        case INVALID_NUMBER:
            fprintf(file, "Invalid number");
            break;
        case INVALID_BASE:
            fprintf(file, "Invalid base");
            break;
        case INVALID_EPSILON:
            fprintf(file, "Invalid epsilon");
            break;
        case INVALID_CMD:
            fprintf(file, "Invalid command");
            break;
        case INVALID_BRACKET_ORDER:
            fprintf(file, "Invalid bracket order");
            break;
        case FILE_OPENING_ERROR:
            fprintf(file, "File cannot be opened");
            break;
        case FILE_INVALID_CONTENT:
            fprintf(file, "File content is invalid");
            break;
        case FILE_INVALID_CONFIG:
            fprintf(file, "Configuration file is invalid");
            break;
        case FILE_KEY_DUPLICATE:
            fprintf(file, "File contains key duplicates");
            break;
        case FILE_END:
            fprintf(file, "Unexpected end of file was found");
            break;
        case MATH_OVERFLOW:
            fprintf(file, "Attempting to overflow");
            break;
        case UNINITIALIZED_USAGE:
            fprintf(file, "Attempting to access uninitialized variable");
            break;
        case DIVISION_BY_ZERO:
            fprintf(file, "Attempting to divide by zero");
            break;
        case ZERO_POWERED_ZERO:
            fprintf(file, "Attempting to raize zero in the power zero");
            break;
        case BAD_ALLOC:
            fprintf(file, "Memory lack occurred");
            break;
        case BAD_ACCESS:
            fprintf(file, "Attempting to access incorrect memory");
            break;
        default:
            fprintf(file, "Unexpected error occurred");
            break;
    }
    for (int i = 0; i < nl_cnt; ++i)
    {
        printf("\n");
    }
}


status_code parse_operation(const char* op_name, operation* op)
{
    if (op_name == NULL)
    {
        return NULL_ARG;
    }
    
    if (!strcmp(op_name, "not"))
    {
        *op = OP_NOT;
    }
    else if (!strcmp(op_name, "input"))
    {
        *op = OP_INPUT;
    }
    else if (!strcmp(op_name, "output"))
    {
        *op = OP_OUTPUT;
    }
    else if (!strcmp(op_name, "add"))
    {
        *op = OP_ADD;
    }
    else if (!strcmp(op_name, "mult"))
    {
        *op = OP_MULT;
    }
    else if (!strcmp(op_name, "sub"))
    {
        *op = OP_SUB;
    }
    else if (!strcmp(op_name, "pow"))
    {
        *op = OP_POW;
    }
    else if (!strcmp(op_name, "div"))
    {
        *op = OP_DIV;
    }
    else if (!strcmp(op_name, "rem"))
    {
        *op = OP_REM;
    }
    else if (!strcmp(op_name, "xor"))
    {
        *op = OP_XOR;
    }
    else if (!strcmp(op_name, "or"))
    {
        *op = OP_OR;
    }
    else if (!strcmp(op_name, "="))
    {
        *op = OP_ASSIGN;
    }
    else
    {
        return INVALID_INPUT;
    }
    return OK;
}


status_code parse_operation_alias(const char* op_alias, config_data config, operation* op)
{
    if (op_alias == NULL)
    {
        return NULL_ARG;
    }
    
    if (!strcmp(op_alias, config.aliases[OP_NOT]))
    {
        *op = OP_NOT;
    }
    else if (!strcmp(op_alias, config.aliases[OP_INPUT]))
    {
        *op = OP_INPUT;
    }
    else if (!strcmp(op_alias, config.aliases[OP_OUTPUT]))
    {
        *op = OP_OUTPUT;
    }
    else if (!strcmp(op_alias, config.aliases[OP_ADD]))
    {
        *op = OP_ADD;
    }
    else if (!strcmp(op_alias, config.aliases[OP_MULT]))
    {
        *op = OP_MULT;
    }
    else if (!strcmp(op_alias, config.aliases[OP_SUB]))
    {
        *op = OP_SUB;
    }
    else if (!strcmp(op_alias, config.aliases[OP_POW]))
    {
        *op = OP_POW;
    }
    else if (!strcmp(op_alias, config.aliases[OP_DIV]))
    {
        *op = OP_DIV;
    }
    else if (!strcmp(op_alias, config.aliases[OP_REM]))
    {
        *op = OP_REM;
    }
    else if (!strcmp(op_alias, config.aliases[OP_XOR]))
    {
        *op = OP_XOR;
    }
    else if (!strcmp(op_alias, config.aliases[OP_OR]))
    {
        *op = OP_OR;
    }
    else if (!strcmp(op_alias, config.aliases[OP_ASSIGN]))
    {
        *op = OP_ASSIGN;
    }
    else
    {
        return INVALID_INPUT;
    }
    return OK;
}


void free_all(ull cnt, ...)
{
	va_list va;
	va_start(va, cnt);
	for (ull i = 0; i < cnt; ++i)
	{
		void* ptr = va_arg(va, void*);
		free(ptr);
	}
	va_end(va);
}

status_code skip_multi_line_comment(FILE* file, char comment_begin, char comment_end)
{
	if (file == NULL)
	{
		return NULL_ARG;
	}
	char ch = comment_begin;
	int comment_nesting = 1;
	while (comment_nesting > 0 && !feof(file))
	{
		ch = getc(file);
		if (ch == comment_begin)
		{
			++comment_nesting;
		}
		else if (ch == comment_end)
		{
			--comment_nesting;
		}
	}
	if (comment_nesting > 0)
	{
		return FILE_INVALID_CONTENT;
	}
	return OK;
}

status_code skip_line(FILE* file)
{
    if (file == NULL)
    {
        return NULL_ARG;
    }
    char ch = getc(file);
    while (ch != '\n' && ch != EOF)
    {
        ch = getc(file);
    }
    return OK;
}

status_code skip_spaces(FILE* file)
{
    if (file == NULL)
    {
        return NULL_ARG;
    }
    char ch = getc(file);
    while (isspace(ch) && ch != EOF)
    {
        ch = getc(file);
    }
    ungetc(ch, file);
    return OK;
}

status_code fread_char(FILE* file, char* ch_res, int skip_front_spaces)
{
    if (file == NULL || ch_res == NULL)
    {
        return NULL_ARG;
    }
    char ch = getc(file);
    while (skip_front_spaces && isspace(ch) && !feof(file))
    {
        ch = getc(file);
    }
    if (feof(file))
    {
        return FILE_END;
    }
    *ch_res = ch;
    return OK;
}

status_code fread_line(FILE* file, char** line, int skip_front_spaces)
{
    if (file == NULL || line == NULL)
    {
        return NULL_ARG;
    }
    ull iter = 0;
    ull size = 4;
    char* line_tmp = (char*) malloc(sizeof(char) * size);
    if (line_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char ch = getc(file);
    while (skip_front_spaces && isspace(ch) && !feof(file))
    {
        ch = getc(file);
    }
    if (feof(file))
    {
        free(line_tmp);
        return FILE_END;
    }
    while (!feof(file) && ch != '\n')
    {
        if (iter + 1 == size)
        {
            size *= 2;
            char* tmp = realloc(line_tmp, size);
            if (tmp == NULL)
            {
                free(line_tmp);
                return BAD_ALLOC;
            }
            line_tmp = tmp;
        }
        line_tmp[iter++] = ch;
        ch = getc(file);
    }
    line_tmp[iter] = '\0';
    *line = line_tmp;
    return OK;
}

status_code fread_line_with_comments(FILE* file, char** line, int skip_front_spaces, char line_comment, char comment_begin, char comment_end)
{
    if (file == NULL || line == NULL)
    {
        return NULL_ARG;
    }
    ull iter = 0;
    ull size = 4;
    char* line_tmp = (char*) malloc(sizeof(char) * size);
    if (line_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char ch = getc(file);
    if (skip_front_spaces)
    {
        skip_spaces(file);
    }
    if (feof(file))
    {
        free(line_tmp);
        return FILE_END;
    }
    while (!feof(file) && ch != '\n')
    {
        if (ch == line_comment)
        {
            if (iter > 0)
            {
                break;
            }
            else
            {
                skip_line(file);
                skip_spaces(file);
            }
        }
        else if (ch == comment_begin)
        {
            skip_multi_line_comment(file, comment_begin, comment_end);
        }
        else
        {
            if (iter + 1 == size)
            {
                size *= 2;
                char* tmp = realloc(line_tmp, size);
                if (tmp == NULL)
                {
                    free(line_tmp);
                    return BAD_ALLOC;
                }
                line_tmp = tmp;
            }
            line_tmp[iter++] = ch;
        }
        ch = getc(file);
    }
    line_tmp[iter] = '\0';
    *line = line_tmp;
    return OK;
}

status_code fread_word(FILE* file, char** word, int skip_front_spaces)
{
    if (file == NULL || word == NULL)
    {
        return NULL_ARG;
    }
    ull iter = 0;
    ull size = 4;
    char* word_tmp = (char*) malloc(sizeof(char) * size);
    if (word_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char ch = getc(file);
    while (skip_front_spaces && isspace(ch) && !feof(file))
    {
        ch = getc(file);
    }
    if (feof(file))
    {
        free(word_tmp);
        return FILE_END;
    }
    while (!feof(file) && !isspace(ch))
    {
        if (iter + 1 == size)
        {
            size *= 2;
            char* tmp = realloc(word_tmp, size);
            if (tmp == NULL)
            {
                free(word_tmp);
                return BAD_ALLOC;
            }
            word_tmp = tmp;
        }
        word_tmp[iter++] = ch;
        ch = getc(file);
    }
    word_tmp[iter] = '\0';
    *word = word_tmp;
    return OK;
}

status_code read_line(char** str, int skip_front_spaces)
{
    return fread_line(stdin, str, skip_front_spaces);
}

status_code sread_until(const char* src, const char* delims, int inclusive_flag, const char** end_ptr, char** str)
{
    if (src == NULL || delims == NULL || str == NULL)
    {
        return NULL_ARG;
    }
    ull cnt = 0;
    ull size = 4;
    char* str_tmp = (char*) malloc(sizeof(char) * size);
    if (str_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char flags[256];
    for (ull i = 0; i < 256; ++i)
    {
        flags[i] = 1;
    }
    for (ull i = 0; delims[i]; ++i)
    {
        flags[(int) delims[i]] = 0;
    }
    const char* ptr = src;
    while (flags[(int) *ptr] && *ptr != '\0')
    {
        if (cnt + 3 == size)
        {
            size *= 2;
            char* tmp = realloc(str_tmp, sizeof(char) * size);
            if (tmp == NULL)
            {
                free(str_tmp);
                return BAD_ALLOC;
            }
            str_tmp = tmp;
        }
        str_tmp[cnt++] = *ptr;
        ++ptr;
    }
    if (end_ptr != NULL)
    {
        *end_ptr = ptr;
    }
    if (inclusive_flag && *ptr != '\0')
    {
        str_tmp[cnt++] = *ptr;
    }
    str_tmp[cnt] = '\0';
    *str = str_tmp;
    return OK;
}

status_code sread_while(const char* src, const char* whitelist, const char** end_ptr, char** str)
{
    if (src == NULL || whitelist == NULL)
    {
        return NULL_ARG;
    }
    ull cnt = 0;
    ull size = 4;
    char* str_tmp = (char*) malloc(sizeof(char) * size);
    if (str_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char flags[256];
    for (ull i = 0; i < 256; ++i)
    {
        flags[i] = 0;
    }
    for (ull i = 0; whitelist[i]; ++i)
    {
        flags[(int) whitelist[i]] = 1;
    }
    const char* ptr = src;
    while (flags[(int) *ptr] && *ptr != '\0')
    {
        if (cnt + 2 == size)
        {
            size *= 2;
            char* tmp = realloc(str_tmp, sizeof(char) * size);
            if (tmp == NULL)
            {
                free(str_tmp);
                return BAD_ALLOC;
            }
            str_tmp = tmp;
        }
        str_tmp[cnt++] = *ptr;
        ++ptr;
    }
    if (end_ptr != NULL)
    {
        *end_ptr = ptr;
    }
    str_tmp[cnt] = '\0';
    if (str != NULL)
    {
        *str = str_tmp;
    }
    else
    {
        free(str_tmp);
    }
    return OK;
}


status_code generate_random_str(char** str, const char* alphabet, size_t max_len)
{
	if (str == NULL)
	{
		return NULL_ARG;
	}
    
	char flags[256];
    memset(flags, 0, sizeof(char) * 256);
    
    for (size_t i = 0; alphabet[i]; ++i)
    {
        if (i > 0 && alphabet[i] == '-')
        {
            if (alphabet[i+1] == '\0' || alphabet[i-1] > alphabet[i+1])
            {
                return INVALID_INPUT;
            }
            
            for (size_t j = alphabet[i-1]; j <= alphabet[i+1]; ++j)
            {
                flags[(int) j] = 1;
            }
        }
        else
        {
            flags[(int) alphabet[i]] = 1;
        }
    }
    
    size_t len = 0;
	char symbols[256];
    char* ptr = symbols;
    
    for (size_t i = 0; i < 256; ++i)
    {
        if (flags[i])
        {
            *ptr++ = (char) i;
        }
    }
    
    *ptr = '\0';
    len = ptr - symbols;
    
	size_t iter = 0;
	size_t size = 4;
	*str = (char*) malloc(sizeof(char) * size);
	if (*str == NULL)
	{
		return BAD_ALLOC;
	}
	
	char ch = symbols[rand() % (len + 1)];
	while ((ch != '\0' || iter == 0) && iter < max_len)
	{
		while (iter == 0 && ch == '\0')
		{
			ch = symbols[rand() % (len + 1)];
		}
		if (iter > size - 2)
		{
			size *= 2;
			char* temp_str = realloc(*str, size);
			if (temp_str == NULL)
			{
				free(*str);
				return BAD_ALLOC;
			}
			*str = temp_str;
		}
		(*str)[iter++] = ch;
		ch = symbols[rand() % (len + 1)];
	}
	(*str)[iter] = '\0';
	return OK;
}


unsigned rand_32()
{
	unsigned x = rand() & 255;
	x |= (rand() & 255) << 8;
	x |= (rand() & 255) << 16;
	x |= (rand() & 255) << 24;
	return x;
}

ull rand_64()
{
	ull x = (ull) rand() & 255;
	x |= ((ull) rand() & 255) << 8;
	x |= ((ull) rand() & 255) << 16;
	x |= ((ull) rand() & 255) << 24;
	x |= ((ull) rand() & 255) << 32;
	x |= ((ull) rand() & 255) << 40;
	x |= ((ull) rand() & 255) << 48;
	x |= ((ull) rand() & 255) << 56;
	return x;
}


int is_word(const char* str)
{
    if (str == NULL || str[0] == '\0')
    {
        return 0;
    }
    for (ull i = 0; str[i]; ++i)
    {
        if (!isalpha(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

int is_number(const char* str)
{
    if (str == NULL || str[0] == '\0')
    {
        return 0;
    }
    if (!isdigit(str[0]) && str[0] != '-' && str[0] != '+')
    {
        return 0;
    }
    if ((str[0] == '-' || str[0] == '+') && str[1] == '\0')
    {
        return 0;
    } 
    for (ull i = 1; str[i]; ++i)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

int ctoi(char ch)
{
    if (isdigit(ch))
    {
        return ch - '0';
    }
    else if (isalpha(ch))
    {
        return toupper(ch) - 'A' + 10;
    }
    return -1;
}

char itoc(int number)
{
    if (number >= 0 && number < 10)
    {
        return '0' + number;
    }
    else if (number >= 10 && number < 36)
    {
        return 'A' + number - 10;
    }
    return '\0';
}

status_code tolowern(const char* src, char** res)
{
    if (src == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    *res = (char*) malloc(sizeof(char) * (strlen(src) + 1));
    if (*res == NULL)
    {
        return BAD_ALLOC;
    }
    ull i = 0;
    for (; src[i]; ++i)
    {
        (*res)[i] = isalpha(src[i]) ? tolower(src[i]) : src[i];
    }
    (*res)[i] = '\0';
    return OK;
}

status_code touppern(const char* src, char** res)
{
    if (src == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    *res = (char*) malloc(sizeof(char) * (strlen(src) + 1));
    if (*res == NULL)
    {
        return BAD_ALLOC;
    }
    ull i = 0;
    for (; src[i]; ++i)
    {
        (*res)[i] = isalpha(src[i]) ? toupper(src[i]) : src[i];
    }
    (*res)[i] = '\0';
    return OK;
}

status_code erase_delims(const char* src, const char* delims, char** res)
{
    if (src == NULL || delims == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    ull cnt = 0;
    ull size = 4;
    char* str_tmp = (char*) malloc(sizeof(char) * size);
    if (str_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char flags[256];
    for (ull i = 0; i < 256; ++i)
    {
        flags[i] = 1;
    }
    for (ull i = 0; delims[i]; ++i)
    {
        flags[(int) delims[i]] = 0;
    }
    const char* ptr = src;
    while (*ptr != '\0')
    {
        if (cnt + 2 == size && flags[(int) *ptr])
        {
            size *= 2;
            char* tmp = realloc(str_tmp, sizeof(char) * size);
            if (tmp == NULL)
            {
                free(str_tmp);
                return BAD_ALLOC;
            }
            str_tmp = tmp;
        }
        if (flags[(int) *ptr])
        {
            str_tmp[cnt++] = *ptr;
        }
        ++ptr;
    }
    str_tmp[cnt] = '\0';
    *res = str_tmp;
    return OK;
}

status_code parse_llong(const char* src, int base, ll* number)
{
    if (src == NULL || number == NULL)
    {
        return NULL_ARG;
    }
    if (base < 0 || base == 1 || base > 36)
    {
        return INVALID_BASE;
    }
    if (src[0] == '\0')
    {
        return INVALID_INPUT;
    }
    errno = 0;
    char* ptr;
    *number = strtoll(src, &ptr, base);
    if (*ptr != '\0')
    {
        return INVALID_INPUT;
    }
    if (errno == ERANGE)
    {
        return MATH_OVERFLOW;
    }
    return OK;
}

status_code parse_ullong(const char* src, int base, ull* number)
{
    if (src == NULL || number == NULL)
    {
        return NULL_ARG;
    }
    if (base < 0 || base == 1 || base > 36)
    {
        return INVALID_BASE;
    }
    if (src[0] == '\0')
    {
        return INVALID_INPUT;
    }
    errno = 0;
    char* ptr;
    *number = strtoull(src, &ptr, base);
    if (*ptr != '\0')
    {
        return INVALID_INPUT;
    }
    if (errno == ERANGE)
    {
        return MATH_OVERFLOW;
    }
    return OK;
}

status_code parse_double(const char* src, double* number)
{
    if (src == NULL || number == NULL)
    {
        return NULL_ARG;
    }
    if (src[0] == '\0')
    {
        return INVALID_INPUT;
    }
    char* ptr;
    *number = strtod(src, &ptr);
    if (*ptr != '\0')
    {
        return INVALID_INPUT;
    }
    return OK;
}

status_code convert_ullong(ull number, int base, char res[65])
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (base < 2 || base > 36)
    {
        return INVALID_BASE;
    }
    for (ll i = 63; i >= 0; --i)
    {
        res[i] = itoc(number % base);
        number /= base;
    }
    res[64] = '\0';
    return OK;
}

int str_comparator(const void* ptr_1, const void* ptr_2)
{
    const char** str_ptr_1 = (const char**) ptr_1;
    const char** str_ptr_2 = (const char**) ptr_2;
    return strcmp(*str_ptr_1, *str_ptr_2);
}


int sign(ll number)
{
    return number == 0 ? 0 : (number > 0 ? 1 : -1);
}

status_code add_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (arg_2 > 0 && (arg_1 > LLONG_MAX - arg_2))
    {
        return MATH_OVERFLOW;
    }
    if (arg_2 < 0 && (arg_2 < LLONG_MIN - arg_2))
    {
        return MATH_OVERFLOW;
    }
    *res = arg_1 + arg_2;
    return OK;
}

status_code sub_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (arg_2 > 0 && (arg_2 < LLONG_MIN + arg_2))
    {
        return MATH_OVERFLOW;
    }
    if (arg_2 < 0 && (arg_1 > LLONG_MAX + arg_2))
    {
        return MATH_OVERFLOW;
    }
    *res = arg_1 - arg_2;
    return OK;
}

status_code mult_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if ((arg_1 == LLONG_MIN && arg_2 != 1) || (arg_2 == LLONG_MIN && arg_1 != 1))
    {
        return MATH_OVERFLOW;
    }
    if (llabs(arg_1) > LLONG_MAX / llabs(arg_2))
    {
        return MATH_OVERFLOW;
    }
    *res = arg_1 * arg_2;
    return OK;
}

status_code div_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (arg_2 == 0)
    {
        return DIVISION_BY_ZERO;
    }
    *res = arg_1 / arg_2;
    return OK;
}

status_code bpow_safely(ll base, ll pow, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (pow < 0)
    {
        return INVALID_INPUT;
    }
    if (base == 0 && pow == 0)
    {
        return ZERO_POWERED_ZERO;
    }
    ll res_tmp = 1;
    ll mult = base;
    while (pow > 0)
    {
        if (pow & 1)
        {
            status_code err_code = mult_safely(res_tmp, mult, &res_tmp);
            if (err_code)
            {
                return err_code;
            }
        }
        mult *= mult;
        pow >>= 1;
    }
    *res = res_tmp;
    return OK;
}

status_code fbpow_safely(double base, ll pow, double* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (base == 0 && pow == 0)
    {
        return ZERO_POWERED_ZERO;
    }
    if (pow == 0)
    {
        *res = 1;
        return OK;
    }
    int pow_sign = sign(pow);
    pow = llabs(pow);
    double res_tmp = 1;
    double mult = base;
    while (pow > 0)
    {
        if (pow & 1)
        {
            res_tmp *= mult;
        }
        mult *= mult;
        pow >>= 1;
    }
    *res = pow_sign == 1 ? res_tmp : 1.0 / res_tmp;
    return OK;
}

size_t calc_default_str_hash(const char* str)
{
    if (str == NULL)
    {
        return 0;
    }
    size_t res = 0;
    for (ull i = 0; str[i]; ++i)
    {
        res *= DEFAULT_HASH_PARAM;
        res += ctoi(str[i]) + 1;
    }
    return res;
}


