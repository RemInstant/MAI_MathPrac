#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stdio.h>

typedef unsigned char uchar;
typedef long long ll;
typedef unsigned long long ull;

typedef enum status_code
{
	OK,
	NULL_ARG,
	INVALID_INPUT,
	INVALID_ARG,
	INVALID_FLAG,
	INVALID_NUMBER,
	INVALID_BASE,
	INVALID_EPSILON,
	INVALID_CMD,
	INVALID_BRACKET_ORDER,
	FILE_OPENING_ERROR,
	FILE_INVALID_CONTENT,
	FILE_END,
	OVERFLOW,
	UNINITIALIZED_USAGE,
	DIVISION_BY_ZERO,
	ZERO_POWERED_ZERO,
	BAD_ALLOC,
	BAD_ACCESS,
} status_code;

void print_error(status_code code, int nl_cnt);
void fprint_error(FILE* file, status_code code, int nl_cnt);

typedef struct pair_str_double
{
	char* str;
	double val;
} pair_str_double;

int pair_str_double_comparator(const void* ptr_1, const void* ptr_2);

status_code fread_line(FILE* file, char** str);
status_code read_line(char** str);
status_code sread_until(const char* src, const char* delims, int inclusive_flag, const char** end_ptr, char** str);

status_code fread_cmd(FILE* file, char** str);
status_code validate_var_name(const char* var_name);
status_code parse_dict_str_double(const char* src, ull* cnt, pair_str_double** dict);

int is_word(const char* str);
int is_number(const char* str);
int ctoi(char ch);
char itoc(int number);
status_code tolowern(const char* src, char** res);
status_code touppern(const char* src, char** res);
status_code erase_delims(const char* src, const char* delims, char** res);
status_code parse_llong(const char* src, int base, ll* number);
status_code parse_ullong(const char* src, int base, ull* number);
status_code parse_double(const char* src, double* number);
status_code convert_ullong(ull number, int base, char res[65]);

int str_comparator(const void* ptr_1, const void* ptr_2);

int sign(ll number);
status_code add_safely(ll arg_1, ll arg_2, ll* res);
status_code sub_safely(ll arg_1, ll arg_2, ll* res);
status_code mult_safely(ll arg_1, ll arg_2, ll* res);
status_code div_safely(ll arg_1, ll arg_2, ll* res);
status_code bpow_safely(ll base, ll pow, ll* res);
status_code fbpow_safely(double base, ll pow, double* res);

#endif // _UTILITY_H_