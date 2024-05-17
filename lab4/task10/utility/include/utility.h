#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stdio.h>

#define DEFAULT_HASH_PARAM 37

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
    FILE_INVALID_CONFIG,
    FILE_KEY_DUPLICATE,
    FILE_END,
    MATH_OVERFLOW,
    UNINITIALIZED_USAGE,
    DIVISION_BY_ZERO,
    ZERO_POWERED_ZERO,
    BAD_ALLOC,
    BAD_ACCESS,
    USER_INPUT_END,
} status_code;

void print_error(status_code code, int nl_cnt);
void fprint_error(FILE* file, status_code code, int nl_cnt);


#define OPERATION_COUNT 12

typedef enum
{
    OP_NOT,
    OP_INPUT,
    OP_OUTPUT,
    OP_ADD,
    OP_MULT,
    OP_SUB,
    OP_POW,
    OP_DIV,
    OP_REM,
    OP_XOR,
    OP_AND,
    OP_OR,
    OP_ASSIGN
} operation;

typedef enum
{
    OP_ORDER_PREFIX,
    OP_ORDER_INFIX,
    OP_ORDER_POSTFIX
} operation_order;

typedef struct
{
    operation_order orders[OPERATION_COUNT];
    char* aliases[OPERATION_COUNT];
} config_data;

status_code parse_operation(const char* op_name, operation* op);
status_code parse_operation_alias(const char* op_alias, config_data config, operation* op);

void free_all(ull cnt, ...);

status_code skip_spaces(FILE* file);
status_code fread_char(FILE* file, char* ch, int skip_front_spaces);
status_code fread_line(FILE* file, char** line, int skip_front_spaces);
status_code fread_line_with_comments(FILE* file, char** line, int skip_front_spaces, char line_comment, char comment_begin, char comment_end);
status_code fread_word(FILE* file, char** word, int skip_front_spaces);
status_code read_line(char** line, int skip_front_spaces);
status_code sread_until(const char* src, const char* delims, int inclusive_flag, const char** end_ptr, char** str);
status_code sread_while(const char* src, const char* whitelist, const char** end_ptr, char** str);

status_code generate_random_str(char** str, const char* alphabet, size_t max_len);

unsigned rand_32();
ull rand_64();

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

size_t calc_default_str_hash(const char* str);

#endif // _UTILITY_H_