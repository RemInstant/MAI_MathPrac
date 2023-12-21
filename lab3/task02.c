#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <stdarg.h>

typedef long long ll;
typedef unsigned long long ull;

typedef enum
{
	OK,
	INVALID_INPUT,
	INVALID_FLAG,
	INVALID_NUMBER,
	INVALID_EPS,
	FILE_OPENING_ERROR,
	OVERFLOW,
	BAD_ALLOC
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
		case INVALID_EPS:
			printf("Invalid epsilon\n");
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
		default:
			printf("Unexpected error occurred\n");
			return;
	}
}

typedef struct
{
	unsigned n;
	double* elems;
} vec_n;

typedef struct
{
	unsigned n;
	double** elems;
} matrix_n;

status_codes construct_vector(unsigned n, vec_n *vec)
{
	if (vec == NULL)
	{
		return INVALID_INPUT;
	}
	vec->n = n;
	vec->elems = (double*) malloc(sizeof(double) * n);
	if (vec->elems == NULL)
	{
		return BAD_ALLOC;
	}
	return OK;
}

status_codes fill_vector(vec_n vec, ...)
{
	if (vec.elems == NULL)
	{
		return INVALID_INPUT;
	}
	va_list arg;
	va_start(arg, vec);
	for (ull i = 0; i < vec.n; ++i)
	{
		vec.elems[i] = va_arg(arg, double);
	}
	va_end(arg);
	return OK;
}

status_codes destruct_vector(vec_n* vec)
{
	if (vec == NULL)
	{
		return INVALID_INPUT;
	}
	free(vec->elems);
	vec->elems = NULL;
	vec->n = 0;
	return OK;
}

void print_vector(vec_n vec)
{
	printf("(");
	for (ull i = 0; i < vec.n - 1; ++i)
	{
		printf("%.3lf; ", vec.elems[i]);
	}
	printf("%.3lf)", vec.elems[vec.n - 1]);
}

int is_matrix_valid(const matrix_n matrix)
{
	if (matrix.elems == NULL)
	{
		return 0;
	}
	for (ull i = 0; i < matrix.n; ++i)
	{
		if (matrix.elems[i] == NULL)
		{
			return 0;
		}
	}
	return 1;
}

status_codes construct_matrix(unsigned n, matrix_n *matrix)
{
	if (matrix == NULL)
	{
		return INVALID_INPUT;
	}
	matrix->n = n;
	matrix->elems = (double**) malloc(sizeof(double*) * n);
	if (matrix->elems == NULL)
	{
		return BAD_ALLOC;
	}
	for (ull i = 0; i < n; ++i)
	{
		matrix->elems[i] = (double*) malloc(sizeof(double) * n);
		if (matrix->elems[i] == NULL)
		{
			for (ull j = 0; j < i; ++j)
			{
				free(matrix->elems[j]);
			}
			free(matrix->elems);
			matrix->elems = NULL;
			return BAD_ALLOC;
		}
	}
	return OK;
}

status_codes fill_matrix(matrix_n matrix, ...)
{
	if (!is_matrix_valid(matrix))
	{
		return INVALID_INPUT;
	}
	va_list arg;
	va_start(arg, matrix);
	for (ull i = 0; i < matrix.n; ++i)
	{
		for(ull j = 0; j < matrix.n; ++j)
		{
			matrix.elems[i][j] = va_arg(arg, double);
		}
	}
	va_end(arg);
	return OK;
}

status_codes destruct_matrix(matrix_n* matrix)
{
	if (!is_matrix_valid(*matrix))
	{
		return INVALID_INPUT;
	}
	for (ull i = 0; i < matrix->n; ++i)
	{
		free(matrix->elems[i]);
	}
	free(matrix->elems);
	matrix->n = 0;
	matrix->elems = NULL;
	return OK;
}

status_codes sum_matrix(const matrix_n matrix_l, const matrix_n matrix_r, matrix_n res_matrix)
{
	if (!is_matrix_valid(matrix_l) || !is_matrix_valid(matrix_r) || !is_matrix_valid(res_matrix)
			|| matrix_l.n != matrix_r.n || matrix_r.n != res_matrix.n)
	{
		return INVALID_INPUT;
	}
	for (ull i = 0; i < matrix_l.n; ++i)
	{
		for (ull j = 0; j < matrix_l.n; ++j)
		{
			res_matrix.elems[i][j] = matrix_l.elems[i][j] + matrix_r.elems[i][j];
		}
	}
	return OK;
}

status_codes multiply_matrix(const matrix_n matrix, double mult, matrix_n res_matrix)
{
	if (!is_matrix_valid(matrix) || !is_matrix_valid(res_matrix) || matrix.n != res_matrix.n)
	{
		return INVALID_INPUT;
	}
	for (ull i = 0; i < matrix.n; ++i)
	{
		for (ull j = 0; j < matrix.n; ++j)
		{
			res_matrix.elems[i][j] = mult * matrix.elems[i][j];
		}
	}
	return OK;
}

status_codes transpose_matrix(const matrix_n matrix, matrix_n res_matrix)
{
	if (!is_matrix_valid(matrix) || !is_matrix_valid(res_matrix))
	{
		return INVALID_INPUT;
	}
	for (ull i = 0; i < matrix.n; ++i)
	{
		for (ull j = 0; j < matrix.n; ++j)
		{
			res_matrix.elems[i][j] = matrix.elems[j][i];
		}
	}
	return OK;
}

status_codes factorial(ull integer, ull* result)
{
	if (result == NULL)
	{
		return INVALID_INPUT;
	}
	*result = 1;
	for (int i = 2; i <= integer; ++i)
	{
		if (*result > ULLONG_MAX / i)
		{
			return OVERFLOW;
		}
		*result *= i;
	}
	return OK;
}

status_codes ll_swap(ll* a, ll* b)
{
	if (a == NULL || b == NULL)
	{
		return INVALID_INPUT;
	}
	ll temp = *a;
	*a = *b;
	*b = temp;
	return OK;
}

status_codes get_permutations(unsigned cnt, const ll* input_arr, ull* res_cnt, ll*** res)
{
	if (res_cnt == NULL || res == NULL)
	{
		return INVALID_INPUT;
	}
	if (input_arr == NULL)
	{
		*res_cnt = 0;
		return OK;
	}
	
	status_codes fact_code = factorial(cnt, res_cnt);
	if (fact_code != OK)
	{
		return fact_code;
	}
	
	*res = (ll**) malloc(sizeof(ll*) * (*res_cnt));
	if (*res == NULL)
	{
		return BAD_ALLOC;
	}
	
	for (ull i = 0; i < *res_cnt; ++i)
	{
		(*res)[i] = (ll*) malloc(sizeof(ll) * cnt);
		
		if ((*res)[i] == NULL)
		{
			for (ull j = 0; j < i; ++j)
			{
				free((*res)[i]);
			}
			free(*res);
			*res = NULL;
			return BAD_ALLOC;
		}
	}
	
	for (ull j = 0; j < cnt; ++j)
	{
		(*res)[0][j] = input_arr[j];
	}
	
	// compute permutations in lexicographic order
	for (ull i = 1; i < *res_cnt; ++i)
	{
		for (ull j = 0; j < cnt; ++j)
		{
			(*res)[i][j] = (*res)[i - 1][j];
		}

		ll* cur_perm = (*res)[i];
		int fi_pos = cnt - 2, se_pos = cnt - 1;
		
		while (fi_pos != -1 && cur_perm[fi_pos] >= cur_perm[fi_pos + 1])
		{
			--fi_pos;
		}
		
		if (fi_pos == -1)
		{
			// from the last sequence in lexicograpic order to the first one (bcs started from non-first)
			for (int j = 0; 2 * j < cnt; ++j)
			{
				ll_swap(cur_perm + j, cur_perm + (cnt - j - 1));
			}
		}
		else
		{
			while (cur_perm[se_pos] <= cur_perm[fi_pos]) 
			{
				--se_pos;
			}
			
			ll_swap(cur_perm + fi_pos, cur_perm + se_pos);
			
			for (int j = 1; 2 * j < cnt - fi_pos; ++j)
			{
				ll_swap(cur_perm + (fi_pos + j), cur_perm + (cnt - j));
			}	
		}
	}
	return OK;
}

status_codes calc_matrix_det(unsigned n, const matrix_n matrix, double* det)
{
	if (!is_matrix_valid(matrix) || det == NULL || n > matrix.n)
	{
		return INVALID_INPUT;
	}
	
	ll* indexes = (ll*) malloc(sizeof(ll) * n);
	if (indexes == NULL)
	{
		return BAD_ALLOC;
	}
	for (ull i = 0; i < n; ++i)
	{
		indexes[i] = i;
	}
	
	ull perm_cnt;
	ll** perms;
	status_codes perm_code = get_permutations(n, indexes, &perm_cnt, &perms);
	free(indexes);
	if (perm_code != OK)
	{
		return perm_code;
	}
	
	*det = 0;
	for (ull i = 0; i < perm_cnt; ++i)
	{
		indexes = perms[i];
		double monomial = 1;
		for (ull j = 0; j < n; ++j)
		{
			monomial *= matrix.elems[j][indexes[j]];
		}
		if (((i + 1) / 2) & 1)
		{
			monomial *= -1;
		}
		*det += monomial;
	}
	
	for (ull i = 0; i < perm_cnt; ++i)
	{
		free(perms[i]);
	}
	free(perms);
	return OK;
}

status_codes is_matrix_positive_definite(const matrix_n matrix, double eps, int* ans)
{
	if (!is_matrix_valid(matrix) || ans == NULL)
	{
		return INVALID_INPUT;
	}
	if (eps <= 0)
	{
		return INVALID_EPS;
	}
	double det;
	for (ull i = 1; i <= matrix.n; ++i)
	{
		status_codes det_code = calc_matrix_det(i, matrix, &det);
		if (det_code != OK)
		{
			return det_code;
		}
		if (det < eps)
		{
			*ans = 0;
			return OK;
		}
	}
	*ans = 1;
	return OK;
}

status_codes calc_norm_inf(const vec_n vec, double* norm)
{
	if (norm == NULL || vec.elems == NULL)
	{
		return INVALID_INPUT;
	}
	double mx = 0;
	for (ull i = 0; i < vec.n; ++i)
	{
		mx = fmax(mx, fabs(vec.elems[i]));
	}
	*norm = mx;
	return OK;
}

status_codes calc_norm_p(const vec_n vec, ll p, double* norm)
{
	if (norm == NULL || vec.elems == NULL)
	{
		return INVALID_INPUT;
	}
	if (p < 1)
	{
		return INVALID_INPUT;
	}
	
	double sum = 0;
	for (ull i = 0; i < vec.n; ++i)
	{
		sum += pow(fabs(vec.elems[i]), p);
	}
	*norm = pow(sum, 1.0 / p);
	return OK;
}

status_codes calc_norm_matrix(const vec_n vec, const matrix_n matrix, double eps, double* norm)
{
	if (vec.elems == NULL || !is_matrix_valid(matrix) || norm == NULL || vec.n != matrix.n)
	{
		return INVALID_INPUT;
	}
	if (eps <= 0)
	{
		return INVALID_EPS;
	}
	matrix_n tmp_matrix_1, tmp_matrix_2;
	status_codes code = construct_matrix(matrix.n, &tmp_matrix_1);
	if (code != OK)
	{
		return code;
	}
	code = construct_matrix(matrix.n, &tmp_matrix_2);
	if (code != OK)
	{
		return code;
	}
	transpose_matrix(matrix, tmp_matrix_1);
	sum_matrix(matrix, tmp_matrix_1, tmp_matrix_2);
	multiply_matrix(tmp_matrix_2, 0.5, tmp_matrix_1);
	
	int is_posit_def;
	code = is_matrix_positive_definite(tmp_matrix_2, eps, &is_posit_def);
	destruct_matrix(&tmp_matrix_1);
	destruct_matrix(&tmp_matrix_2);
	if (code != OK)
	{
		return code;
	}
	if (!is_posit_def)
	{
		return INVALID_INPUT;
	}
	
	// CALC
	vec_n vec_l;
	vec_l.n = vec.n;
	vec_l.elems = (double*) malloc(sizeof(double) * vec.n);
	if (vec_l.elems == NULL)
	{
		return BAD_ALLOC;
	}
	
	for (ull i = 0; i < vec.n; ++i)
	{
		vec_l.elems[i] = 0;
		for (ull j = 0; j < vec.n; ++j)
		{
			vec_l.elems[i] += matrix.elems[i][j] * vec.elems[j];
		}
	}
	
	*norm = 0;
	for (ull i = 0; i < vec.n; ++i)
	{
		*norm += vec_l.elems[i] * vec.elems[i];
 	}
	*norm = sqrt(*norm);
	
	free(vec_l.elems);
	return OK;
}

status_codes solve
(
	unsigned n, double eps,
	status_codes (*calc_norm1)(const vec_n, double*),
	status_codes (*calc_norm2)(const vec_n, ll, double*), ll p,
	status_codes (*calc_norm3)(const vec_n, const matrix_n, double, double*), const matrix_n matrix,
	ull* res1_cnt, vec_n** res1_vecs,
	ull* res2_cnt, vec_n** res2_vecs,
	ull* res3_cnt, vec_n** res3_vecs,
	unsigned input_cnt, ...
)
{
	ull size[3] = { 2, 2, 2};
	ull cnt[3] = { 0, 0, 0 };
	double max_norm[3] = { -DBL_MAX, -DBL_MAX, -DBL_MAX};
	vec_n* res[3];
	res[0] = (vec_n*) malloc(sizeof(vec_n) * 2);
	res[1] = (vec_n*) malloc(sizeof(vec_n) * 2);
	res[2] = (vec_n*) malloc(sizeof(vec_n) * 2);
	if (res[0] == NULL || res[1] == NULL || res[2] == NULL)
	{
		free(res[0]);
		free(res[1]);
		free(res[2]);
		return BAD_ALLOC;
	}
	
	va_list arg;
	va_start(arg, input_cnt);
	
	int error_flag = 0;
	status_codes res_code;
	for (ull i = 0; i < input_cnt && !error_flag; ++i)
	{
		vec_n vec = va_arg(arg, vec_n);
		double norm[3];
		
		status_codes code1 = calc_norm1(vec, &(norm[0]));
		status_codes code2 = calc_norm2(vec, p, &(norm[1]));
		status_codes code3 = calc_norm3(vec, matrix, eps, &(norm[2]));
		if (code1 || code2 || code3)
		{
			error_flag = 1;
			res_code = code1 ? code1 : (code2 ? code2 : code3);
		}
		else
		{
			for (ull j = 0; j < 3 && !error_flag; ++j)
			{
				if (norm[j] - max_norm[j] > eps)
				{
					cnt[j] = 1;
					res[j][0] = vec;
					max_norm[j] = norm[j];
				}
				else if (fabs(norm[j] - max_norm[j]) <= eps)
				{
					if (cnt[j] == size[j])
					{
						size[j] *= 2;
						vec_n* tmp = (vec_n*) realloc(res[j], sizeof(vec_n) * size[j]);
						if (tmp == NULL)
						{
							free(res[j]);
							error_flag = 1;
							res_code = BAD_ALLOC;
						}
						res[j] = tmp;
					}
					if (res[j] != NULL)
					{
						res[j][cnt[j]++] = vec;
					}
				}
			}
		}
	}
	if (error_flag)
	{
		free(res[0]);
		free(res[1]);
		free(res[2]);
		return res_code;
	}
	
	for (ull i = 0; i < 3; ++i)
	{
		if (cnt[i] < size[i])
		{
			vec_n* tmp = (vec_n*) realloc(res[i], sizeof(vec_n) * cnt[i]);
			if (tmp == NULL)
			{
				free(res[0]);
				free(res[1]);
				free(res[2]);
				return BAD_ALLOC;
			}
			res[i] = tmp;
		}
	}
	
	*res1_cnt = cnt[0];
	*res2_cnt = cnt[1];
	*res3_cnt = cnt[2];
	*res1_vecs = res[0];
	*res2_vecs = res[1];
	*res3_vecs = res[2];
	return OK;
}

int main(int argc, char** argv)
{
	status_codes code;
	unsigned n = 2;
	matrix_n matrix;
	construct_matrix(n, &matrix);
	fill_matrix(matrix, 5.0, 0.0, -8.0, 5.0);
	
	vec_n vec1, vec2, vec3;
	construct_vector(n, &vec1);
	construct_vector(n, &vec2);
	construct_vector(n, &vec3);
	fill_vector(vec1, 2.0, 3.0);
	fill_vector(vec2, 1.0, 1.0);
	fill_vector(vec3, 2.001, 3.001);
	
	ull cnt1, cnt2, cnt3;
	vec_n* vecs1;
	vec_n* vecs2;
	vec_n* vecs3;
	
	code = solve(n, 1e-5, calc_norm_inf, calc_norm_p, 5, calc_norm_matrix, matrix,
			&cnt1, &vecs1, &cnt2, &vecs2, &cnt3, &vecs3, 3, vec1, vec2, vec3);
	
	if (code != OK)
	{
		print_error(code);
		return code;
	}
	
	printf("%llu %llu %llu\n", cnt1, cnt2, cnt3);
	printf("vecs with max norm1: ");
	for (ull i = 0; i < cnt1; ++i)
	{
		print_vector(vecs1[i]);
		printf(" ");
	}
	printf("\nvecs with max norm2: ");
	for (ull i = 0; i < cnt2; ++i)
	{
		print_vector(vecs2[i]);
		printf(" ");
	}
	printf("\nvecs with max norm3: ");
	for (ull i = 0; i < cnt3; ++i)
	{
		print_vector(vecs3[i]);
		printf(" ");
	}
	printf("\n");
	
	destruct_vector(&vec1);
	destruct_vector(&vec2);
	destruct_vector(&vec3);
	destruct_matrix(&matrix);
	free(vecs1);
	free(vecs2);
	free(vecs3);
}