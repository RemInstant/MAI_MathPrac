#include "polynomial.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <errno.h>

status_code monome_extract(const char* src, const char** end_ptr, char** monome_str)
{
	if (src == NULL || monome_str == NULL)
	{
		return INVALID_ARG;
	}
	if (!isalnum(*src) && *src != '+' && *src != '-')
	{
		return INVALID_INPUT;
	}
	ull cnt = 0;
	ull size = 4;
	*monome_str = (char*) malloc(sizeof(char) * size);
	if (*monome_str == NULL)
	{
		return BAD_ALLOC;
	}
	
	char prev = 0;
	const char* ptr = src;
	int run_flag = 1;
	while (run_flag && *ptr)
	{
		if (cnt + 1 == size)
		{
			size *= 2;
			char* temp_word = realloc(*monome_str, sizeof(char) * size);
			if (temp_word == NULL)
			{
				free(*monome_str);
				return BAD_ALLOC;
			}
			*monome_str = temp_word;
		}
		(*monome_str)[cnt++] = *ptr;
		prev = *ptr;
		++ptr;
		
		if (prev != '_' && prev != '*' && prev != '^' && (*ptr == '+' || *ptr == '-'))
		{
			run_flag = 0;
		}
	}
	if (end_ptr != NULL)
	{
		*end_ptr = ptr;
	}
	(*monome_str)[cnt] = '\0';
	return OK;
}

status_code monome_set_null(Monome* monome)
{
	if (monome == NULL)
	{
		return INVALID_ARG;
	}
	trie_set_null(&monome->vars);
	monome->coef = 0;
	monome->prev = monome->next = NULL;
	return OK;
}

status_code monome_construct(Monome* monome, const char* src)
{
	if (monome == NULL || src == NULL)
	{
		return INVALID_ARG;
	}
	// 5.114_x^2_x1^3_y12^3_y333^4_-2.1_z_x1^-1_-1
	monome->coef = 1;
	monome->prev = monome->next = NULL;
	trie_set_null(&monome->vars);
	status_code err_code = trie_construct(&monome->vars, calc_default_trie_hash);
	const char* ptr = src;
	while(!err_code && *ptr)
	{
		char* token = NULL;
		err_code = sread_until(ptr, "_*", 0, &ptr, &token);
		char* token_ptr = token;
		if (*token_ptr == '+' || *token_ptr == '-')
		{
			monome->coef *= *token_ptr == '-' ? -1 : 1;
			token_ptr++;
		}
		// HANDLE COEFFICIENT TOKEN
		if (!err_code && isdigit(*token_ptr))
		{
			double coef;
			coef = strtod(token_ptr, &token_ptr);
			if (*token_ptr)
			{
				err_code = err_code ? err_code : INVALID_INPUT;
			}
			monome->coef *= coef;
		}
		// HANDLE VARIABLE TOKEN
		else if (!err_code && isalpha(*token_ptr))
		{
			const char* const_token_ptr = token_ptr;
			char* var_name = NULL;
			char* pow_str = NULL;
			ll pow = 1;
			err_code = err_code ? err_code : sread_until(const_token_ptr, "^", 0, &const_token_ptr, &var_name);
			err_code = err_code ? err_code : validate_var_name(var_name);
			if (*const_token_ptr == '^')
			{
				err_code = err_code ? err_code : sread_until(++const_token_ptr, "", 0, &const_token_ptr, &pow_str);
				if (!err_code)
				{
					if (pow_str[0] == '\0' || ((pow_str[0] == '-' || pow_str[0] == '+') && pow_str[1] == '\0'))
					{
						err_code = INVALID_INPUT;
					}
					errno = 0;
					char* pow_ptr;
					pow = strtoll(pow_str, &pow_ptr, 10);
					if (*pow_ptr)
					{
						err_code = err_code ? err_code : INVALID_INPUT;
					}
					if (errno == ERANGE)
					{
						err_code = err_code ? err_code : OVERFLOW;
					}
				}
			}
			err_code = err_code ? err_code : trie_add(&monome->vars, var_name, pow);
			free(var_name);
			free(pow_str);
		}
		// HANDLE INVALID TOKEN
		else
		{
			err_code = err_code ? err_code : INVALID_INPUT;
		}
		if (*ptr == '_' || *ptr == '*')
		{
			++ptr;
		}
		free(token);
	}
	if (!err_code)
	{
		ull kv_cnt = 0;
		trie_key_val* kv_arr = NULL;
		err_code = trie_get_key_vals(monome->vars, &kv_cnt, &kv_arr);
		for (ull i = 0; i < kv_cnt && !err_code; ++i)
		{
			err_code = kv_arr[i].val > 0 ? OK : INVALID_INPUT;
			free(kv_arr[i].str);
		}
		free(kv_arr);
	}
	if (err_code)
	{
		trie_destruct(&monome->vars);
	}
	return err_code;
}

// DOES NOT COPY LINKS
status_code monome_copy(Monome* m_copy, Monome m_src)
{
	if (m_copy == NULL)
	{
		return INVALID_ARG;
	}
	trie_set_null(&m_copy->vars);
	status_code err_code = trie_construct(&m_copy->vars, calc_default_trie_hash);
	if (err_code)
	{
		return err_code;
	}
	m_copy->coef = m_src.coef;
	m_copy->prev = m_copy->next = NULL;
	ull kv_cnt = 0;
	trie_key_val* kv_arr = NULL;
	err_code = trie_get_key_vals(m_src.vars, &kv_cnt, &kv_arr);
	for (ull i = 0; i < kv_cnt && !err_code; ++i)
	{
		err_code = trie_set(&m_copy->vars, kv_arr[i].str, kv_arr[i].val);
	}
	if (err_code)
	{
		trie_destruct(&m_copy->vars);
	}
	for (ull i = 0; i < kv_cnt; ++i)
	{
		free(kv_arr[i].str);
	}
	free(kv_arr);
	return err_code;
}

status_code monome_destruct(Monome* monome)
{
	if (monome == NULL)
	{
		return INVALID_ARG;
	}
	trie_destruct(&monome->vars);
	monome->coef = 0;
	monome->prev = monome->next = NULL;
	return OK;
}

status_code monome_print(Monome monome)
{
	ull kv_cnt;
	trie_key_val* kv_arr = NULL;
	status_code err_code = trie_get_key_vals(monome.vars, &kv_cnt, &kv_arr);
	if (err_code)
	{
		return err_code;
	}
	if (monome.coef > 0)
	{
		printf("+");
	}
	printf("%.3lf", monome.coef);
	for (ull i = 0; i < kv_cnt; ++i)
	{
		printf("_%s", kv_arr[i].str);
		if (kv_arr[i].val != 1)
		{
			printf("^%lld", kv_arr[i].val);
		}
	}
	for (ull i = 0; i < kv_cnt; ++i)
	{
		free(kv_arr[i].str);
	}
	free(kv_arr);
	return OK;
}

// MODIFIES ARG (UNSAFELY)
status_code monome_mult_by_other(Monome* monome, Monome other_monome)
{
	if (monome == NULL)
	{
		return INVALID_ARG;
	}
	ull kv_cnt;
	trie_key_val* kv_arr = NULL;
	status_code err_code = trie_get_key_vals(other_monome.vars, &kv_cnt, &kv_arr);
	for (ull i = 0; i < kv_cnt && !err_code; ++i)
	{
		err_code = trie_add(&monome->vars, kv_arr[i].str, kv_arr[i].val);
	}
	if (!err_code)
	{
		monome->coef *= other_monome.coef;
	}
	for (ull i = 0; i < kv_cnt; ++i)
	{
		free(kv_arr[i].str);
	}
	free(kv_arr);
	return err_code;
}

// REQUIRES SORTED VARS ARR
status_code monome_eval(Monome monome, ull cnt, pair_str_double* var_values, double* value)
{
	if (var_values == NULL || value == NULL)
	{
		return INVALID_ARG;
	}
	ull kv_cnt;
	trie_key_val* kv_arr = NULL;
	status_code err_code = trie_get_key_vals(monome.vars, &kv_cnt, &kv_arr);
	ull iter = 0;
	*value = monome.coef;
	for (ull i = 0; i < kv_cnt && !err_code; ++i)
	{
		while (iter < cnt && strcmp(kv_arr[i].str, var_values[iter].str))
		{
			++iter;
		}
		if (iter == cnt)
		{
			err_code = UNINITIALIZED_USAGE;
		}
		double tmp = 1;
		err_code = err_code ? err_code : fbpow_safely(var_values[iter].val, kv_arr[i].val, &tmp);
		*value *= tmp;
	}
	for (ull i = 0; i < kv_cnt; ++i)
	{
		free(kv_arr[i].str);
	}
	free(kv_arr);
	return err_code;
}


// DOES NOT CREATE COPY
status_code polynomial_insert_monome(Polynomial* poly, Monome* monome)
{
	if (poly == NULL || monome == NULL)
	{
		return INVALID_ARG;
	}
	status_code err_code = OK;
	if (poly->size == 0)
	{
		poly->begin = poly->end = monome;
		poly->size = 1;
	}
	else
	{
		Monome* poly_iter = poly->begin;
		Monome* target = NULL;
		while (!err_code && poly_iter != NULL && target == NULL)
		{
			int is_equal = 0;
			err_code = trie_equal(monome->vars, poly_iter->vars, &is_equal);
			if (!err_code && is_equal)
			{
				target = poly_iter;
			}
			poly_iter = poly_iter->next;
		}
		if (!err_code && target == NULL)
		{
			poly->end->next = monome;
			monome->prev = poly->end;
			poly->end = monome;
			poly->size++;
		}
		else if (!err_code)
		{
			target->coef += monome->coef;
			monome_destruct(monome);
			free(monome);
		}
	}
	return err_code;
}

// MODIFIEES ARG (SAFELY)
status_code polynomial_remove_zero(Polynomial* poly, double eps)
{
	if (poly == NULL)
	{
		return INVALID_ARG;
	}
	Monome* monome = poly->begin;
	while (monome != NULL)
	{
		Monome* tmp = monome->next;
		if (fabs(monome->coef) <= eps)
		{
			if (poly->size == 1)
			{
				poly->begin = poly->end = NULL;
			}
			else if (monome == poly->begin)
			{
				poly->begin = monome->next;
				monome->next->prev = poly->begin;
				poly->begin->prev = NULL;
			}
			else if (monome == poly->end)
			{
				poly->end = monome->prev;
				monome->prev->next = poly->end;
				poly->end->next = NULL;
			}
			else
			{
				monome->prev->next = monome->next;
				monome->next->prev = monome->prev;
			}
			monome_destruct(monome);
			free(monome);
			poly->size--;
		}
		monome = tmp;
	}
	return OK;
}

// MODIFIEES ARG (UNSAFELY)
status_code polynomial_mult_by_monome(Polynomial* poly, Monome monome)
{
	if (poly == NULL)
	{
		return INVALID_ARG;
	}
	status_code err_code = OK;
	Monome* poly_iter = poly->begin;
	while (!err_code && poly_iter != NULL)
	{
		err_code = monome_mult_by_other(poly_iter, monome);
		poly_iter = poly_iter->next;
	}
	return err_code;
}


status_code polynomial_set_null(Polynomial* poly)
{
	if (poly == NULL)
	{
		return INVALID_ARG;
	}
	poly->begin = NULL;
	poly->end = NULL;
	poly->size = 0;
	return OK;
}

status_code polynomial_construct(Polynomial* poly, double eps, const char* src)
{
	if (poly == NULL || src == NULL)
	{
		return INVALID_ARG;
	}
	if (eps <= 0)
	{
		return INVALID_EPSILON;
	}
	poly->begin = NULL;
	poly->end = NULL;
	poly->size = 0;
	status_code err_code = OK;
	const char* ptr = src;
	while (!err_code && *ptr)
	{
		char* monome_str = NULL;
		Monome* monome = (Monome*) malloc(sizeof(Monome));
		err_code = monome != NULL ? OK : BAD_ALLOC;
		err_code = err_code ? err_code : monome_set_null(monome);
		err_code = err_code ? err_code : monome_extract(ptr, &ptr, &monome_str);
		err_code = err_code ? err_code : monome_construct(monome, monome_str);
		err_code = err_code ? err_code : polynomial_insert_monome(poly, monome);
		if (err_code)
		{
			monome_destruct(monome);
			free(monome);
		}
		free(monome_str);
	}
	err_code = err_code ? err_code : polynomial_remove_zero(poly, eps);
	if (err_code)
	{
		polynomial_destruct(poly);
	}
	return err_code;
}

status_code polynomial_copy(Polynomial* poly_copy, Polynomial poly_src)
{
	if (poly_copy == NULL)
	{
		return INVALID_ARG;
	}
	poly_copy->begin = poly_copy->end = NULL;
	poly_copy->size = 0;
	
	status_code err_code = OK;
	Monome* poly_iter = poly_src.begin;
	while (!err_code && poly_iter != NULL)
	{
		Monome* monome = (Monome*) malloc(sizeof(Monome));
		err_code = monome != NULL ? OK : BAD_ALLOC;
		err_code = err_code ? err_code : monome_copy(monome, *poly_iter);
		if (!err_code && poly_copy->size == 0)
		{
			poly_copy->begin = poly_copy->end = monome;
			poly_copy->size = 1;
		}
		else if (!err_code)
		{
			poly_copy->end->next = monome;
			monome->prev = poly_copy->end;
			poly_copy->end = monome;
			poly_copy->size++;
		}
		if (err_code)
		{
			monome_destruct(monome);
			free(monome);
		}
		poly_iter = poly_iter->next;
	}
	if (err_code)
	{
		polynomial_destruct(poly_copy);
	}
	return err_code;
}

status_code polynomial_move(Polynomial* poly_move, Polynomial* poly_src)
{
	if (poly_move == NULL || poly_src == NULL)
	{
		return INVALID_ARG;
	}
	poly_move->begin = poly_src->begin;
	poly_move->end = poly_src->end;
	poly_move->size = poly_src->size;
	poly_src->begin = poly_src->end = NULL;
	poly_src->size = 0;
	return OK;
}

status_code polynomial_destruct(Polynomial* poly)
{
	if (poly == NULL)
	{
		return INVALID_INPUT;
	}
	Monome* monome = poly->begin;
	while (monome != NULL)
	{
		Monome* tmp = monome;
		monome = monome->next;
		monome_destruct(tmp);
		free(tmp);
	}
	poly->begin = poly->end = NULL;
	poly->size = 0;
	return OK;
}

status_code polynomial_print(Polynomial poly, int new_line_flag)
{
	if (poly.begin == NULL)
	{
		printf("Empty polynomial\n");
		return OK;
	}
	Monome* monome = poly.begin;
	while (monome != NULL)
	{
		status_code err_code = monome_print(*monome);
		if (err_code)
		{
			return err_code;
		}
		monome = monome->next;
	}
	if (new_line_flag)
	{
		printf("\n");
	}
	return OK;
}


status_code polynomial_add(Polynomial poly_1, Polynomial poly_2, double eps, Polynomial* summator)
{
	if (summator == NULL)
	{
		return INVALID_ARG;
	}
	if (eps <= 0)
	{
		return INVALID_EPSILON;
	}
	polynomial_set_null(summator);
	status_code err_code = polynomial_copy(summator, poly_1);
	Monome* poly_2_iter = poly_2.begin;
	while (!err_code && poly_2_iter != NULL)
	{
		Monome* monome = (Monome*) malloc(sizeof(Monome));
		err_code = monome != NULL ? OK : BAD_ALLOC;
		err_code = err_code ? err_code : monome_copy(monome, *poly_2_iter);
		err_code = err_code ? err_code : polynomial_insert_monome(summator, monome);
		if (err_code)
		{
			monome_destruct(monome);
			free(monome);
		}
		poly_2_iter = poly_2_iter->next;
	}
	err_code = err_code ? err_code : polynomial_remove_zero(summator, eps);
	if (err_code)
	{
		polynomial_destruct(summator);
	}
	return err_code;
}

status_code polynomial_sub(Polynomial poly_1, Polynomial poly_2, double eps, Polynomial* summator)
{
	if (summator == NULL)
	{
		return INVALID_ARG;
	}
	if (eps <= 0)
	{
		return INVALID_EPSILON;
	}
	polynomial_set_null(summator);
	status_code err_code = polynomial_copy(summator, poly_1);
	Monome* poly_2_iter = poly_2.begin;
	while (!err_code && poly_2_iter != NULL)
	{
		Monome* monome = (Monome*) malloc(sizeof(Monome));
		err_code = monome != NULL ? OK : BAD_ALLOC;
		err_code = err_code ? err_code : monome_copy(monome, *poly_2_iter);
		if (!err_code)
		{
			monome->coef *= -1;
		}
		err_code = err_code ? err_code : polynomial_insert_monome(summator, monome);
		if (err_code)
		{
			monome_destruct(monome);
			free(monome);
		}
		poly_2_iter = poly_2_iter->next;
	}
	err_code = err_code ? err_code : polynomial_remove_zero(summator, eps);
	if (err_code)
	{
		polynomial_destruct(summator);
	}
	return err_code;
}

status_code polynomial_mult(Polynomial poly_1, Polynomial poly_2, double eps, Polynomial* summator)
{
	if (summator == NULL)
	{
		return INVALID_ARG;
	}
	if (eps <= 0)
	{
		return INVALID_EPSILON;
	}
	polynomial_set_null(summator);
	status_code err_code = OK;
	Monome* poly_1_iter = poly_1.begin;
	while (!err_code && poly_1_iter != NULL)
	{
		Polynomial poly_tmp;
		polynomial_set_null(&poly_tmp);
		err_code = err_code ? err_code : polynomial_copy(&poly_tmp, poly_2);
		err_code = err_code ? err_code : polynomial_mult_by_monome(&poly_tmp, *poly_1_iter);

		Monome* tmp_iter = err_code ? NULL : poly_tmp.begin;
		while (!err_code && tmp_iter != NULL)
		{
			Monome* monome = (Monome*) malloc(sizeof(Monome));
			err_code = monome != NULL ? OK : BAD_ALLOC;
			err_code = err_code ? err_code : monome_copy(monome, *tmp_iter);
			err_code = err_code ? err_code : polynomial_insert_monome(summator, monome);
			tmp_iter = tmp_iter->next;
		}
		polynomial_destruct(&poly_tmp);
		poly_1_iter = poly_1_iter->next;
	}
	err_code = err_code ? err_code : polynomial_remove_zero(summator, eps);
	if (err_code)
	{
		polynomial_destruct(summator);
	}
	return err_code;
}

status_code polynomial_eval(Polynomial poly, ull cnt, pair_str_double* var_values, double* value)
{
	if (var_values == NULL || value == NULL)
	{
		return INVALID_ARG;
	}
	*value = 0;
	qsort(var_values, cnt, sizeof(pair_str_double), pair_str_double_comparator);
	status_code err_code = OK;
	Monome* monome = poly.begin;
	while (!err_code && monome != NULL)
	{
		double tmp = 0;
		err_code = monome_eval(*monome, cnt, var_values, &tmp);
		*value += tmp;
		monome = monome->next;
	}
	return err_code;
}

status_code polynomial_part_deriv(Polynomial poly, const char* var, double eps, Polynomial* summator)
{
	if (var == NULL || summator == NULL)
	{
		return INVALID_ARG;
	}
	if (eps <= 0)
	{
		return INVALID_EPSILON;
	}
	status_code err_code = OK;
	err_code = err_code ? err_code : validate_var_name(var);
	err_code = err_code ? err_code : polynomial_copy(summator, poly);
	Monome* iter = summator->begin;
	while (!err_code && iter != NULL)
	{
		int is_contained = 0;
		ll val = 0;
		err_code = trie_contains(iter->vars, var, &is_contained);
		if (is_contained)
		{
			err_code = err_code ? err_code : trie_get(iter->vars, var, &val);
			err_code = err_code ? err_code : trie_add(&iter->vars, var, -1);
		}
		iter->coef *= val;
		iter = iter->next;
	}
	err_code = err_code ? err_code : polynomial_remove_zero(summator, eps);
	if (err_code)
	{
		polynomial_destruct(summator);
	}
	return err_code;
}

status_code polynomial_print_grad(Polynomial poly, double eps, int new_line_flag)
{
	if (eps <= 0)
	{
		return INVALID_EPSILON;
	}
	Trie all_vars;
	trie_set_null(&all_vars);
	status_code err_code = trie_construct(&all_vars, calc_default_trie_hash);
	Monome* iter = poly.begin;
	while (!err_code && iter != NULL)
	{
		ull kv_cnt;
		trie_key_val* kv_arr = NULL;
		err_code = trie_get_key_vals(iter->vars, &kv_cnt, &kv_arr);
		for (ull i = 0; i < kv_cnt; ++i)
		{
			err_code = err_code ? err_code : trie_set(&all_vars, kv_arr[i].str, 1);
			free(kv_arr[i].str);
		}
		free(kv_arr);
		iter = iter->next;
	}
	if (!err_code)
	{
		ull kv_cnt;
		trie_key_val* kv_arr = NULL;
		err_code = trie_get_key_vals(all_vars, &kv_cnt, &kv_arr);
		printf("(");
		for (ull i = 0; i < kv_cnt && !err_code; ++i)
		{
			Polynomial tmp;
			polynomial_set_null(&tmp);
			printf("%s:", kv_arr[i].str);
			err_code = err_code ? err_code : polynomial_part_deriv(poly, kv_arr[i].str, eps, &tmp);
			err_code = err_code ? err_code : polynomial_print(tmp, 0);
			if (i + 1 != kv_cnt)
			{
				printf(", ");
			}
			polynomial_destruct(&tmp);
		}
		printf("%s", new_line_flag ? ")\n" : ")");
		for (ull i = 0; i < kv_cnt; ++i)
		{
			free(kv_arr[i].str);
		}
		free(kv_arr);
	}
	trie_destruct(&all_vars);
	return OK;
}

status_code polynomial_part_antideriv(Polynomial poly, const char* var, double eps, Polynomial* summator)
{
	if (var == NULL || summator == NULL)
	{
		return INVALID_ARG;
	}
	if (eps <= 0)
	{
		return INVALID_EPSILON;
	}
	status_code err_code = OK;
	err_code = err_code ? err_code : validate_var_name(var);
	err_code = err_code ? err_code : polynomial_copy(summator, poly);
	Monome* iter = summator->begin;
	while (!err_code && iter != NULL)
	{
		int is_contained = 0;
		ll val = 0;
		err_code = trie_contains(iter->vars, var, &is_contained);
		if (is_contained)
		{
			err_code = err_code ? err_code : trie_get(iter->vars, var, &val);
		}
		err_code = err_code ? err_code : trie_add(&iter->vars, var, 1);
		iter->coef /= val + 1;
		iter = iter->next;
	}
	err_code = err_code ? err_code : polynomial_remove_zero(summator, eps);
	if (err_code)
	{
		polynomial_destruct(summator);
	}
	return err_code;
}
