#ifndef _POLYNOMIAL_H_
#define _POLYNOMIAL_H_

#include "utility.h"
#include "trie.h"

typedef long long ll;
typedef unsigned long long ull;

typedef struct Monome
{
	double coef;
	Trie vars;
	struct Monome* prev;
	struct Monome* next;
} Monome;

typedef struct polynomial
{
	Monome* begin;
	Monome* end;
	ull size;
} Polynomial;

status_code polynomial_set_null(Polynomial* poly);
status_code polynomial_construct(Polynomial* poly, double eps, const char* src);
status_code polynomial_copy(Polynomial* poly_copy, Polynomial poly_src);
status_code polynomial_move(Polynomial* poly_move, Polynomial* poly_src);
status_code polynomial_destruct(Polynomial* poly);
status_code polynomial_print(Polynomial poly, int new_line_flag);

status_code polynomial_add(Polynomial poly_1, Polynomial poly_2, double eps, Polynomial* summator);
status_code polynomial_sub(Polynomial poly_1, Polynomial poly_2, double eps, Polynomial* summator);
status_code polynomial_mult(Polynomial poly_1, Polynomial poly_2, double eps, Polynomial* summator);
status_code polynomial_eval(Polynomial poly, ull cnt, pair_str_double* var_values, double* value);
status_code polynomial_part_deriv(Polynomial poly, const char* var, double eps, Polynomial* summator);
status_code polynomial_print_grad(Polynomial poly, double eps, int new_line_flag);
status_code polynomial_part_antideriv(Polynomial poly, const char* var, double eps, Polynomial* summator);

#endif // _POLYNOMIAL_H_