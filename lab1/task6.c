#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

int validate_string_float(char* str_float) {
	int dot_cnt = 0;
	for (int i = 0; str_float[i] != '\0'; ++i) {
		if (i == 0 && str_float[i] == '-') continue;
		if (dot_cnt > 1) return 0;
		if (str_float[i] == '.') {
			++dot_cnt;
			continue;
		}
		if (isdigit(str_float[i])) continue;
		return 0;
	}
	return 1;
}

double integrate_a(double eps) {
	// right Riemann sum
	double part_len = 1;
	double step = 1;
	double prev, cur = log(2);
	
	do {
		prev = cur;
		part_len /= 2;
		step /= 2;
		
		cur = 0;
		for (double x = 1; x > 0; x -= step)
			cur += log(1+x)/x * part_len;
		
	} while (fabs(cur - prev) >= eps);
	
	return cur;
}

double integrate_b(double eps) {
	// right Riemann sum
	double part_len = 1;
	double step = 1;
	double prev, cur = exp(-0.5);
	
	do {
		prev = cur;
		part_len /= 2;
		step /= 2;
		
		cur = 0;
		for (double x = 1; x > 0; x -= step)
			cur += exp(-x*x * 0.5) * part_len;
		
	} while (fabs(cur - prev) >= eps);
	
	return cur;
}

double integrate_c(double eps) {
	// left Riemann sum
	double part_len = 1;
	double step = 1;
	double prev, cur = 0;
	
	do {
		prev = cur;
		part_len /= 2;
		step /= 2;
		
		cur = 0;
		for (double x = 0; x < 1; x += step)
			cur += log(1.0 / (1 - x)) * part_len;
		
	} while (fabs(cur - prev) >= eps);
	
	return cur;
}

double integrate_d(double eps) {
	// right Riemann sum
	double part_len = 1;
	double step = 1;
	double prev, cur = 1;
	
	do {
		prev = cur;
		part_len /= 2;
		step /= 2;
		
		cur = 0;
		for (double x = 1; x > 0; x -= step)
			cur += pow(x, x) * part_len;
		
	} while (fabs(cur - prev) >= eps);
	
	return cur;
}

int main(int argc, char** argv) {
	if (argc == 1) {
		printf("Usage: command_name <eps>\n");
		printf("application computes some integrals named a, b, c and d...\n");
		printf("I hope, you know those four as well\n");
		return 0;
	}
	
	if (argc != 2) {
		printf("Invalid input\n");
		return 1;
	}
	
	char* string_eps = argv[1];
	double eps;
	
	if (!validate_string_float(string_eps)) {
		printf("Invalid float\n");
		return 2;
	}
	
	eps = atof(string_eps);
	
	if (eps <= 0) {
		printf("Epsilon must be positive\n");
		return 3;
	}
	
	printf("the value of integral a is %.15lf\n", integrate_a(eps));
	printf("the value of integral b is %.15lf\n", integrate_b(eps));
	printf("the value of integral c is %.15lf\n", integrate_c(eps));
	printf("the value of integral d is %.15lf\n", integrate_d(eps));
}