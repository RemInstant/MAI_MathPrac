#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int validate_string_integer(char* str_int) {
	for (int i = 0; str_int[i] != '\0'; ++i) {
		if (i == 0 && str_int[i] == '-') continue;
		if ('0' <= str_int[i] && str_int[i] <= '9') continue;
		return 0;
	}
	return 1;
}

int validate_string_float(char* str_float) {
	int dot_cnt = 0;
	for (int i = 0; str_float[i] != '\0'; ++i) {
		if (i == 0 && str_float[i] == '-') continue;
		if (dot_cnt > 1) return 0;
		if (str_float[i] == '.') {
			++dot_cnt;
			continue;
		}
		if ('0' <= str_float[i] && str_float[i] <= '9') continue;
		return 0;
	}
	return 1;
}

typedef enum {
	fsc_ok,
	fsc_overflow,
	fsc_invalid_input
} factorial_status_codes;

factorial_status_codes factorial(long long integer, unsigned long long* result) {
	if (integer < 0) return fsc_invalid_input;
	
	*result = 1ull;
	
	for (int i = 2; i <= integer; ++i) {
		if (*result > ULONG_LONG_MAX / i)
			return fsc_overflow;
		
		*result *= i;
	}
	
	return fsc_ok;
}

void farr_swap(double* farr, int i, int j) {
	double temp = farr[i];
	farr[i] = farr[j];
	farr[j] = temp;
}

void permutations(double eps, unsigned int cnt, double* numbers, unsigned long long* res_cnt, double*** res) {
	if (res_cnt == NULL || res == NULL) return;
	
	if (numbers == NULL) {
		*res_cnt = 0;
		return;
	}
	
	if (factorial(cnt, res_cnt) != fsc_ok) {
		*res_cnt = 0;
		return;
	}
	
	*res = malloc(sizeof(double*) * (*res_cnt));
	
	for (int i = 0; i < *res_cnt; ++i) {
		(*res)[i] = malloc(sizeof(double) * cnt);
	}
	
	for (int j = 0; j < cnt; ++j) {
		(*res)[0][j] = numbers[j];
	}
	
	// compute permutations in lexicographic order
	for (int i = 1; i < *res_cnt; ++i) {
		
		for (int j = 0; j < cnt; ++j) {
			(*res)[i][j] = (*res)[i-1][j];
		}

		double* cur_perm = (*res)[i];
		int fi_pos = cnt-2, se_pos = cnt-1;
		
		while (fi_pos != -1 && cur_perm[fi_pos] >= cur_perm[fi_pos+1])
			--fi_pos;
		
		if (fi_pos == -1) {
			for (int j = 0; 2*j < cnt; ++j)
				farr_swap(cur_perm, j, cnt-j-1);
			continue;
		}
		
		while (cur_perm[se_pos] < cur_perm[fi_pos]) --se_pos;
		
		farr_swap(cur_perm, fi_pos, se_pos);
		
		for (int j = 1; 2*j < cnt-fi_pos; ++j)
			farr_swap(cur_perm, fi_pos+j, cnt-j);
	}
	
	int bound = *res_cnt - 1;
		
	// turn array into [unique permutations | repeating permutations]
	for (int i = 0; i < bound; ++i) {
		
		for (int j = i+1; j <= bound; ++j) {
			int flag = fabs((*res)[i][0] - (*res)[j][0]) < eps;
			flag = flag && fabs((*res)[i][1] - (*res)[j][1]) < eps;
			flag = flag && fabs((*res)[i][2] - (*res)[j][2]) < eps;
			
			if (flag) {
				double* temp = (*res)[bound];
				(*res)[bound] = (*res)[j];
				(*res)[j] = temp;
				--bound; --j;
			}
		}
	}
	
	// free repeating permutations
	for (int i = bound+1; i < *res_cnt; ++i) {
		free((*res)[i]);
	}
	
	*res_cnt = bound+1;
	*res = realloc(*res, sizeof(double*) * (bound+1));
}

void solve_quadratic_equation(double eps, double a, double b, double c, int* cnt, double* x1, double* x2) {
	if (cnt == NULL || x1 == NULL || x2 == NULL) {
		return;
	}
	
	if (fabs(a) < eps) {
		if (fabs(b) < eps) {
			*cnt = 0;
		} else {
			*cnt = 1;
			*x1 = -c/b;
		}
		return;
	}
	
	double discr = b*b - 4*a*c;
	
	if (discr < 0) {
		*cnt = 0;
	} else if (fabs(discr) < eps) {
		*cnt = 1;
		*x1 = (-b + sqrt(discr)) / (2*a);
	} else {
		*cnt = 2;
		*x1 = (-b - sqrt(discr)) / (2*a);
		*x2 = (-b + sqrt(discr)) / (2*a);
	}
}

int main(int argc, char** argv) {
	if (argc == 1) {
		printf("Usage: command_name <flag>\n");
		printf("flags:\n");
		printf("-q <epsilon> <a> <b> <c>  -  solve quadratic equation for all permutations of real coefficients a, b, c.\n");
		printf("                             Epsilon specifies the accuracy of comparison of float point numbers.\n");
		printf("-m  <a> <b>               -  check if integer a is a multiple of integer b (integers must be non-zero)\n");
		printf("-t <epsilon> <a> <b> <c>  -  check if real numbers a, b, c can be sides of a right triangle.\n");
		printf("                             Epsilon specifies the accuracy of comparison of float point numbers.\n");
		return 0;
	}
	
	char* flag = argv[1];
	
	if (((flag[0] != '-') && (flag[0] != '/')) || flag[2] != '\0' 
			|| (flag[1] != 'q' && flag[1] != 'm' && flag[1] != 't')) {
		printf("Invalid flag\n");
		return 2;
	}
	
	switch (flag[1]) {
		case 'q':
		case 't':
			
			if (argc != 6) {
				printf("Invalid input\n");
				return 1;
			}
			
			for (int i = 2; i < 6; ++i) {
				if (!validate_string_float(argv[i])) {
					printf("Invalid float\n");
					return 3;
				}
			}
			
			if (atof(argv[3]) <= 0) {
				printf("Epsilon must be positive\n");
				return 4;
			}
			
			break;
			
		case 'm':
			
			if (argc != 4) {
				printf("Invalid input\n");
				return 1;
			}
			
			for (int i = 2; i < 4; ++i) {
				if (!validate_string_integer(argv[i])) {
					printf("Invalid integer\n");
					return 3;
				}
				
				if (atoi(argv[i]) == 0) {
					printf("Integers must be non-zero\n");
					return 4;
				}
			}
			
			break;
	}
	
	switch (flag[1]) {
		case 'q': {
			
			double eps = atof(argv[2]);
			double* eq_coefs = malloc(sizeof(double)*3);
			eq_coefs[0] = atof(argv[3]);
			eq_coefs[1] = atof(argv[4]);
			eq_coefs[2] = atof(argv[5]);
			
			double** eq_coefs_perm;
			unsigned long long perm_cnt;
			
			permutations(eps, 3, eq_coefs, &perm_cnt, &eq_coefs_perm);
			
			for (int i = 0; i < perm_cnt; ++i) {
				int root_cnt;
				double x1, x2;
				
				double a = eq_coefs_perm[i][0];
				double b = eq_coefs_perm[i][1];
				double c = eq_coefs_perm[i][2];
				
				solve_quadratic_equation(eps, a, b, c, &root_cnt, &x1, &x2);
				
				printf("Equation %0.4lf*x^2 + %0.4lf*x + %0.4lf = 0   ", a, b, c);
				
				if (root_cnt == 0) printf("has no roots\n");
				if (root_cnt == 1) printf("has one root: x1 = %lf\n", x1);
				if (root_cnt == 2) printf("has two roots: x1 = %lf   x2 = %lf\n", x1, x2);
			}
			
			for (int i = 0; i < perm_cnt; ++i)
				free(eq_coefs_perm[i]);	
			if (perm_cnt != 0)
				free(eq_coefs_perm);
			free(eq_coefs);
			
			break;
		}
		case 'm': {
			
			int first_number = atoi(argv[2]);
			int second_number = atoi(argv[3]);
			
			if (second_number % first_number == 0)
				printf("%d is a multiple of %d\n", first_number, second_number);
			else
				printf("%d is not a multiple of %d\n", first_number, second_number);

			break;
		}
		case 't': {
			
			double eps = atof(argv[2]);
			double* tr_sides = malloc(sizeof(double)*3);
			tr_sides[0] = atof(argv[3]);
			tr_sides[1] = atof(argv[4]);
			tr_sides[2] = atof(argv[5]);
			
			double** tr_sides_perm;
			unsigned long long perm_cnt;
			
			permutations(eps, 3, tr_sides, &perm_cnt, &tr_sides_perm);
			
			int tr_flag = 0;
			
			for (int i = 0; i < perm_cnt; ++i) {
				
				double a = tr_sides_perm[i][0];
				double b = tr_sides_perm[i][1];
				double c = tr_sides_perm[i][2];
				
				if (fabs(a*a + b*b - c*c) < eps) {
					tr_flag = 1;
					break;
				}
			}
			
			if (tr_flag)
				printf("numbers %lf, %lf, %lf can be sides of a right triangle\n", tr_sides[0], tr_sides[1], tr_sides[2]);
			else
				printf("numbers %lf, %lf, %lf cannot be sides of a right triangle\n", tr_sides[0], tr_sides[1], tr_sides[2]);
			
			for (int i = 0; i < perm_cnt; ++i)
				free(tr_sides_perm[i]);
			if (perm_cnt != 0)
				free(tr_sides_perm);
			free(tr_sides);
			
			break;
		}
	}
}