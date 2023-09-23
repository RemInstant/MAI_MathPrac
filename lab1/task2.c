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

int is_prime(long long integer) {
	if (integer == 0 || integer == 1) return 0;
	if (integer == 2) return 1;
	if (integer % 2 == 0) return 0;
	for (int i = 3; i*i <= integer; i += 2)
		if (integer % i == 0) return 0;
	return 1;
}

double combination(int k, int n) {
	if(k > n) return 0;
	double res = 1.0;
	//for(int i = k+1; i <= n; ++i) res *= i;
	//for(int i = 2; i <= (n-k); ++i) res /= i;
	
	for(int i = 1; i <= k; ++i) {
		res *= n - k + i;
		res /= i;
	}
	
	return res;
}

double calc_e_with_lim(double eps) {
	int n = 1;
	double prev = 0; 
	double cur = 2; // function with n equal to 1
	do {
		++n;
		prev = cur;
		cur = 1.0;
		for(int i = 0; i < n; ++i) {
			cur *= 1 + 1.0/n;
		}
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_e_with_sum(double eps) {
	int n = 0;
	double factorial = 1;
	double next_elem = 1;
	double ans = 0;
	do {
		ans += next_elem;
		
		++n;
		factorial *= n;
		next_elem = 1 / factorial;
		
	} while (next_elem >= eps);
	return ans;
}

double calc_e_with_equation(double eps) {
	// ln(x) = 1 using the Newton's method
	double prev = 0; 
	double cur = 3;
	do {
		prev = cur;	
		// f(x) = log(x) - 1
		// f(x) / f'(x) = x * (log(x) - 1)
		cur -= cur*(log(cur)-1);
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_pi_with_lim(double eps) {
	int n = 1;
	double prev = 0;
	double cur = 4; // function with n equal to 1;
	
	do {
		++n;
		prev = cur;
		// f(n) = f(n-1) * f(n) / f(n-1) = f(n-1) * (4n(n-1)/(2n-1)^2)
		cur *= 4.0 * n * (n-1) / ((2.0*n-1) * (2.0*n-1)) ;
	} while (fabsl(cur-prev) >= eps);
	return cur;
}

double calc_pi_with_sum(double eps) {
	int n = 1;
	double next_elem = 4;
	double ans = 0;
	do {
		ans += next_elem;
		
		++n;
		next_elem = 4 / (2.0*n-1);
		if(n % 2 == 0) next_elem *= -1;
		
	} while (fabsl(next_elem) >= eps);
	return ans;
}

double calc_pi_with_equation(double eps) {
	// cos(x) = -1 using the Newton's method
	double prev = 0; 
	double cur = 3;
	do {
		prev = cur;	
		// f(x) = cos(x) + 1
		// f(x) / f'(x) = -(cos(x) + 1) / sin(x)
		cur += (cos(cur)+1) / sin(cur);
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_ln2_with_lim(double eps) {
	int n = 1;
	double prev = 0; 
	double cur = 1; // function with n equal to 1
	do {
		++n;
		prev = cur;
		cur = n * (pow(2, 1.0/n) - 1);
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_ln2_with_sum(double eps) {
	int n = 1;
	double next_elem = 1;
	double ans = 0;
	do {
		ans += next_elem;
		
		++n;
		next_elem = 1.0 / n;
		if(n % 2 == 0) next_elem *= -1;
		
	} while (fabsl(next_elem) >= eps);
	return ans;
}

double calc_ln2_with_equation(double eps) {
	// exp(x) = 2 using the Newton's method
	double prev = 0; 
	double cur = 1;
	do {
		prev = cur;	
		// f(x) = exp(x) - 2
		// f(x) / f'(x) = (exp(x) - 2) / exp(x) = 1 - 2/exp(x)
		cur -= 1 - 2/exp(cur);
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_sqrt2_with_lim(double eps) {
	double prev = 0; 
	double cur = -0.5;
	do {
		prev = cur;
		cur += -cur * cur / 2 + 1;
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_sqrt2_with_prod(double eps) {
	double next_elem = pow(2, 0.25);
	double prev = 1;
	double cur = 1;
	do {
		prev = cur;
		cur *= next_elem;
		next_elem = pow(next_elem, 0.5);
			
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_sqrt2_with_equation(double eps) {
	// x^2 = 2 using the Newton's method
	double prev = 0; 
	double cur = 1;
	do {
		prev = cur;	
		// f(x) = x^2 - 2
		// f(x) / f'(x) = (x^2 - 2) / (2x)
		cur -= (cur*cur-2) / (2*cur);
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_gamma_with_lim(double eps) {
	int m = 1;
	double prev = 0; 
	double cur = 0; // function with n equal to 1
	
	do {
		++m;
		prev = cur;
		cur = 0;
		
		double elem = 0; // second element of sum (first equal to 0)
		
		double* logarithms = malloc(sizeof(double)*(m+1));
		logarithms[1] = 0;
		for(int i = 2; i <= m; ++i) {
			logarithms[i] = logarithms[i-1] + log(i);
		}
		
		double comb = m;
		
		cur = 0;
		
		for(int k = 2; k <= m; ++k) {
			
			comb *= (m-k+1.0) / k;
			
			elem = comb * logarithms[k] / k;
			if(k % 2) elem *= -1;
			
			
			//if(k == 47) printf("CHECK %.15lf %.15lf %.15lf\n", comb, comb * logarithms[k], comb * logarithms[k] / k);
			
			cur += elem;
			
			//if(m == 47) printf("k=%d: %.15lf (+%.15lf)\n", k, cur, elem);
		}
		
		free(logarithms);
		
		//printf("%d: %.10lf -> %.10lf (%.20lf)\n", m, prev, cur, fabs(cur-prev));
		
	} while (fabsl(cur - prev) >= eps);
	return cur;
}

double calc_gamma_with_sum(double eps) {
	int n = 2;
	long long temp;
	double next_elem = 0.5;
	double pi = calc_pi_with_equation(1e-12);
	double ans = -pi*pi / 6;
	do {
		ans += next_elem;
		
		++n;
		next_elem = pow(floor(sqrt(n)), -2) - 1.0/n;
		
		temp = (int) sqrt(n);
		
	} while (fabsl(next_elem) >= eps || (temp * temp == n));
	return ans;
}

double calc_gamma_with_equation(double eps) {
	
	int t = 2;
	double prod = 0.5;
	
	double prev = 0; 
	double cur = -log(0.5 * log(2)); // function with t equal to 2
	
	//printf("%d: %lf\n", t, cur);
	
	do {
		++t;
		prev = cur;	
		
		if(is_prime(t)) prod *= (t-1.0)/t;
		
		cur = -log(prod * log(t));

		//printf("%d(%d): %lf\n", t, is_prime(t), cur);

	} while (fabsl(cur - prev) >= eps);
	return cur;
}

int main(int argc, char** argv) {
	if (argc != 4) {
		printf("Invalid input\n");
		printf("Usage: command_name <constant_id> <flag> <eps>\n");
		printf("application computes <constant> using method defined by flag with accuracy of <eps>\n");
		printf("available constants:\n1: e    2: pi    3: ln2    4: sqrt2    5: gamma\n");
		printf("flags:\n");
		printf("-l  -  compute using a limit\n");
		printf("-s  -  compute using a sum/product of series\n");
		printf("-e  -  compute using an equation\n");
		printf("-a  -  compute using all methods\n");
		printf("-u  -  computes all constants using all methods\n");
		return 1;
	}
	
	char* string_const_id = argv[1];
	char* flag = argv[2];
	char* string_eps = argv[3];
	
	int const_id;
	double eps;
	
	if (!validate_string_integer(string_const_id)) {
		printf("Invalid integer\n");
		return 2;
	}
	
	const_id = atoi(string_const_id);
	
	if(const_id < 1 || const_id > 5) {
		printf("Invalid constant id");
		return 4;
	}
	
	if (((flag[0] != '-') && (flag[0] != '/')) || flag[2] != '\0' 
			|| (flag[1] != 'l' && flag[1] != 's' && flag[1] != 'e'
			&& flag[1] != 'a' && flag[1] != 'u')) {
		printf("Invalid flag\n");
		return 3;
	}
	
	if (!validate_string_float(string_eps)) {
		printf("Invalid float\n");
		return 2;
	}
	
	eps = atof(string_eps);
	
	if (flag[1] == 'u') {
		printf("e = %.10lf (limit)\n", calc_e_with_lim(eps));
		printf("e = %.10lf (series)\n", calc_e_with_sum(eps));
		printf("e = %.10lf (equation)\n\n", calc_e_with_equation(eps));
		printf("pi = %.10lf (limit)\n", calc_pi_with_sum(eps));
		printf("pi = %.10lf (series)\n", calc_pi_with_lim(eps));
		printf("pi = %.10lf (equation)\n\n", calc_pi_with_equation(eps));
		printf("ln2 = %.10lf (limit)\n", calc_ln2_with_lim(eps));
		printf("ln2 = %.10lf (series)\n", calc_ln2_with_sum(eps));
		printf("ln2 = %.10lf (equation)\n\n", calc_ln2_with_equation(eps));
		printf("sqrt2 = %.10lf (limit)\n", calc_sqrt2_with_lim(eps));
		printf("sqrt2 = %.10lf (series)\n", calc_sqrt2_with_prod(eps));
		printf("sqrt2 = %.10lf (equation)\n\n", calc_sqrt2_with_equation(eps));
		printf("gamma = %.10lf (limit)\n", calc_gamma_with_lim(eps));
		printf("gamma = %.10lf (series)\n", calc_gamma_with_sum(eps));
		printf("gamma = %.10lf (equation)\n", calc_gamma_with_equation(eps));
		return 0;
	}
	
	switch (const_id) {
		case 1: {
			
			if (flag[1] == 'l' || flag[1] == 'a') printf("e = %.10lf (limit)\n", calc_e_with_lim(eps));
			if (flag[1] == 's' || flag[1] == 'a') printf("e = %.10lf (series)\n", calc_e_with_sum(eps));
			if (flag[1] == 'e' || flag[1] == 'a') printf("e = %.10lf (equation)\n", calc_e_with_equation(eps));
			break;
			
		}
		case 2: {
			
			if (flag[1] == 's' || flag[1] == 'a') printf("pi = %.10lf (limit)\n", calc_pi_with_sum(eps));
			if (flag[1] == 'l' || flag[1] == 'a') printf("pi = %.10lf (series)\n", calc_pi_with_lim(eps));
			if (flag[1] == 'e' || flag[1] == 'a') printf("pi = %.10lf (equation)\n", calc_pi_with_equation(eps));
			break;
			
		}
		case 3: {
			
			if (flag[1] == 'l' || flag[1] == 'a') printf("ln2 = %.10lf (limit)\n", calc_ln2_with_lim(eps));
			if (flag[1] == 's' || flag[1] == 'a') printf("ln2 = %.10lf (series)\n", calc_ln2_with_sum(eps));
			if (flag[1] == 'e' || flag[1] == 'a') printf("ln2 = %.10lf (equation)\n", calc_ln2_with_equation(eps));
			break;
			
		}
		case 4: {
			
			if (flag[1] == 'l' || flag[1] == 'a') printf("sqrt2 = %.10lf (limit)\n", calc_sqrt2_with_lim(eps));
			if (flag[1] == 's' || flag[1] == 'a') printf("sqrt2 = %.10lf (series)\n", calc_sqrt2_with_prod(eps));
			if (flag[1] == 'e' || flag[1] == 'a') printf("sqrt2 = %.10lf (equation)\n", calc_sqrt2_with_equation(eps));
			break;
			
		}
		case 5: {
			
			if (flag[1] == 'l' || flag[1] == 'a') printf("gamma = %.10lf (limit)\n", calc_gamma_with_lim(eps));
			if (flag[1] == 's' || flag[1] == 'a') printf("gamma = %.10lf (series)\n", calc_gamma_with_sum(eps));
			if (flag[1] == 'e' || flag[1] == 'a') printf("gamma = %.10lf (equation)\n", calc_gamma_with_equation(eps));
			break;
			
		}
		default:
			break;
	}
}