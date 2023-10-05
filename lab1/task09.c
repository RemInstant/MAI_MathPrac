#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define B_ELEM_MIN -10 	// -1000
#define B_ELEM_MAX 10 	//  1000
#define B_COUNT_MIN 10 	//  10
#define B_COUNT_MAX 15 	//  1000

typedef enum
{
	OK,
	INVALID_INPUT,
	OVERFLOW,
	BAD_ALLOC
} status_codes;

status_codes validate_string_integer(char* str_int)
{
	int integer = 0;
	
	if (str_int[0] != '-')
	{
		for (int i = 0; str_int[i]; ++i)
		{
			if (!isdigit(str_int[i]))
			{
				return INVALID_INPUT;
			}
			
			int add = str_int[i] - '0';
			if (integer > (INT_MAX - add) / 10)
			{
				return OVERFLOW;
			}
			
			integer = integer * 10 + add;
		}
	}
	else
	{
		if (!isdigit(str_int[1]))
		{
			return INVALID_INPUT;
		}
		for (int i = 1; str_int[i]; ++i)
		{
			if (!isdigit(str_int[i]))
			{
				return INVALID_INPUT;
			}
			
			int subtr = str_int[i] - '0';
			if (integer < (INT_MIN + subtr) / 10)
			{
				return OVERFLOW;
			}
			
			integer = integer * 10 - subtr;
		}
	}
	return OK;
}

unsigned rand_unsigned()
{
	unsigned x = rand() & 255;
	x |= (rand() & 255) << 8;
	x |= (rand() & 255) << 16;
	x |= (rand() & 255) << 24;
    return x;
}

unsigned rand_range(int mn, int mx)
{
	return mn + rand_unsigned() % (mx - mn + 1);
}

void fill_rand(int* arr, int size, int mn, int mx)
{
	if (arr == NULL)
	{
		return;
	}
	for (int i = 0; i < size; ++i)
	{
		arr[i] = rand_range(mn, mx);
	}
}

void quicksort(int* arr, int begin, int end)
{
	if (arr == NULL || begin == end)
	{
		return;
	}
	
	int pivot = arr[(begin + end) / 2];
	int i = begin;
	int j = end;
	while (i < j)
	{
		while (arr[i] < pivot)
		{
			++i;
		}
		while (arr[j] > pivot)
		{
			--j;
		}
		if (i < j)
		{
			int temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
			++i;
			--j;
		}
	}
	while (arr[j] > pivot)
	{
		--j;
	}
	quicksort(arr, begin, j);
	quicksort(arr, j+1, end);
}

int lower_bound(int* arr, int begin, int end, int x)
{
	if (arr == NULL)
	{
		return -1;
	}
	if (x > arr[end])
	{
		return end+1;
	}
	
	int l = begin;
	int r = end;
	while (l < r)
	{
		int m = (l + r) / 2;
		if (arr[m] <= x)
		{
			l = m+1;
		}
		else
		{
			r = m;
		}
	}
	return r;
}

void do_a(int* arr, int size)
{
	int mx_pos = 0, mn_pos = 0;
	for (int i = 0; i < size; ++i)
	{
		if (arr[i] >= arr[mx_pos])
		{
			mx_pos = i;
		}
		if (arr[i] <= arr[mn_pos])
		{
			mn_pos = i;
		}
	}
	int temp = arr[mx_pos];
	arr[mx_pos] = arr[mn_pos];
	arr[mn_pos] = temp;
}

void do_b(int* arr_a, int size_a, int* arr_b, int size_b, int* arr_c)
{
	quicksort(arr_b, 0, size_b-1);
	for (int i = 0; i < size_a; ++i)
	{
		int pos = lower_bound(arr_b, 0, size_b-1, arr_a[i]);
		
		if (pos == size_b)
		{
			--pos;
		}
		else if (pos > 0 && (arr_a[i] - arr_b[pos - 1]) < (arr_b[pos] - arr_a[i]))
		{
			--pos;
		}
		arr_c[i] = arr_a[i] + arr_b[pos];
	}
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		// TODO
		printf("Usage: command_name <input file> <output file\n");
		printf("(Files containing only digits and letters can be processed)\n");
		printf("For each number of input file program figures out the minimum base (X) of numeral system");
		printf(" this number can be represented in.\n");
		printf("Then, the number is converted from this numeral system into decimal one\n");
		printf("Output format:\n");
		printf("<base-X number> <X> <base-10 number>\n");
		return 0;
	}
	
	// -a <size> <a> <b>
	// -b
	
	char* flag = argv[1];
	
	if (((flag[0] != '-') && (flag[0] != '/')) || flag[2] != '\0' 
			|| (flag[1] != 'a' && flag[1] != 'b'))
	{
		printf("Invalid flag\n");
		return 2;
	}
	
	switch (flag[1])
	{
		case 'a':
		{
			if (argc != 5)
			{
				printf("Invalid input\n");
				return 1;
			}
			if(validate_string_integer(argv[2]) != OK
					|| validate_string_integer(argv[3]) != OK
					|| validate_string_integer(argv[4]) != OK)
			{
				printf("Invalid integer\n");
				return 2;
			}
			break;
		}
		case 'b':
		{
			if (argc != 2)
			{
				printf("Invalid input\n");
				return 1;
			}
			break;
		}
	}
	
	srand(time(NULL));
	switch (flag[1])
	{
		case 'a':
		{
			int size = atoi(argv[2]);
			int mn_border = atoi(argv[3]);
			int mx_border = atoi(argv[4]);
			
			int* arr = (int*) malloc(sizeof(int) * size);
			if (arr == NULL)
			{
				printf("Memory lack error\n");
				return 3;
			}
			
			fill_rand(arr, size, mn_border, mx_border);
			
			printf("Generated array:\n");
			for (int i = 0; i < size; ++i)
			{
				printf("%d ", arr[i]);
			}
			printf("\n\n");
			
			do_a(arr, size);
			
			printf("Processed array:\n");
			for (int i = 0; i < size; ++i)
			{
				printf("%d ", arr[i]);
			}
			printf("\n");
			
			free(arr);
			break;
		}
		case 'b':
		{
			int size_a = rand_range(B_COUNT_MIN, B_COUNT_MAX);
			int size_b = rand_range(B_COUNT_MIN, B_COUNT_MAX);
			
			int* arr_a = (int*) malloc(sizeof(int) * size_a);
			int* arr_b = (int*) malloc(sizeof(int) * size_b);
			int* arr_c = (int*) malloc(sizeof(int) * size_a);
			
			if(arr_a == NULL || arr_b == NULL || arr_c == NULL)
			{
				if (arr_a != NULL)
				{
					free(arr_a);
				}
				if (arr_b != NULL)
				{
					free(arr_b);
				}
				if (arr_c != NULL)
				{
					free(arr_c);
				}
				printf("Memory lack error\n");
				return 3;
			}
			
			fill_rand(arr_a, size_a, B_ELEM_MIN, B_ELEM_MAX);
			fill_rand(arr_b, size_b, B_ELEM_MIN, B_ELEM_MAX);
			
			printf("Array A:\n");
			for (int i = 0; i < size_a; ++i)
			{
				printf("%d ", arr_a[i]);
			}
			printf("\nArray B:\n");
			for (int i = 0; i < size_b; ++i)
			{
				printf("%d ", arr_b[i]);
			}
			printf("\n");
			
			do_b(arr_a, size_a, arr_b, size_b, arr_c);
			
			printf("Array C:\n");
			for (int i = 0; i < size_a; ++i)
			{
				printf("%d ", arr_c[i]);
			}
			printf("\n");
			
			free(arr_a);
			free(arr_b);
			free(arr_c);
			break;
		}
	}
}