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

int sum_a(double eps, double x, double* ans) {
	// this series converges for any x
	if(ans == NULL) return 0;
	int n = 0;
	double sum = 0, elem = 1;
	
	do {
		sum += elem;
		++n;
		elem *= x / n;
		
	} while (fabs(elem) >= eps);
	
	*ans = sum;
	
	return 1;
}

int sum_b(double eps, double x, double* ans) {
	// this series converges for any x
	if(ans == NULL) return 0;
	int n = 0;
	double sum = 0, elem = 1;
	
	do {
		sum += elem;
		++n;
		elem *= -x*x / (2.0*n * (2.0*n-1));
		
	} while (fabs(elem) >= eps);
	
	*ans = sum;
	
	return 1;
}

int sum_c(double eps, double x, double* ans) {
	// this series converges for |x| < 1
	if(fabs(x) >= 1 || ans == NULL) return 0;
	int n = 0;
	double sum = 0, elem = 1;
	
	do {
		sum += elem;
		++n;
		elem *= 9.0 * n*n * x*x / ((3.0*n-1) * (3.0*n-2));
		
	} while (fabs(elem) >= eps);
	
	*ans = sum;
	
	return 1;
}

int sum_d(double eps, double x, double* ans) {
	// this series converges for |x| < 1
	if(fabs(x) >= 1 || ans == NULL) return 0;
	int n = 1;
	double sum = 0, elem = -x*x / 2;
	
	do {
		sum += elem;
		++n;
		elem *= -(2.0*n - 1) * x*x / (2.0*n);
		
	} while (fabs(elem) >= eps);
	
	*ans = sum;
	
	return 1;
}

int main(int argc, char** argv) {
	if (argc == 1) {
		printf("Usage: command_name <eps> <x>\n");
		printf("application computes some series that are depend on real number x and named a, b, c and d...\n");
		printf("you know them, yeah?\n");
		printf("(series c and d converges for |x| < 1)\n");
		return 0;
	}
	
	if (argc != 3) {
		printf("Invalid input\n");
		return 1;
	}
	
	char* string_eps = argv[1];
	char* string_x = argv[2];
	
	if (!validate_string_float(string_eps) || !validate_string_float(string_x)) {
		printf("Invalid float\n");
		return 2;
	}
	
	double eps = atof(string_eps);
	double x = atof(string_x);
	
	if (eps <= 0) {
		printf("Epsilon must be positive\n");
		return 3;
	}
	
	double ans;
	
	if(sum_a(eps, x, &ans))
		printf("The sum of the series a is %lf\n", ans);
	else
		printf("The series a disconverges\n");
	
	if(sum_b(eps, x, &ans))
		printf("The sum of the series b is %lf\n", ans);
	else
		printf("The series b disconverges\n");
	
	if(sum_c(eps, x, &ans))
		printf("The sum of the series c is %lf\n", ans);
	else
		printf("The series c disconverges\n");
	
	if(sum_d(eps, x, &ans))
		printf("The sum of the series d is %lf\n", ans);
	else
		printf("The series d disconverges\n");
}