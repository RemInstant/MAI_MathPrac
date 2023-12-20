#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#define FIBONACCI_46 1836311903
#define FIBONACCI_47 2971215073

typedef long long ll;
typedef unsigned long long ull;

typedef enum
{
	OK,
	INVALID_INPUT,
	INVALID_FLAG,
	INVALID_NUMBER,
	OVERFLOW,
	BAD_ALLOC,
	NULL_POINTER_ERROR
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
		case OVERFLOW:
			printf("An overflow occurred\n");
			return;
		case BAD_ALLOC:
			printf("Memory lack error occurred\n");
			return;
		case NULL_POINTER_ERROR:
			printf("Null pointer error occurred\n");
			return;
		default:
			printf("Unexpected error occurred\n");
			return;
	}
}

typedef enum
{
	// BASE
	NONE,
	CHAR,
	STRING,
	INT,
	UINT,
	OCT_UINT,
	HEX_UINT,
	CHAR_N,
	FLOAT,
	FLOAT_EXP,
	FLOAT_HEX_EXP,
	PTR,
	// SPECIFIC
	ROMAN,
	ZECKENDORF,
	BASED_INT_L,
	BASED_INT_U
} specifiers;

int is_base_specifier(char ch)
{
	char specs[18] = { 'c', 's', 'd', 'i', 'o', 'x', 'X', 'u', 'f', 'F', 'e', 'a', 'A', 'E', 'g', 'G', 'n', 'p' };
	for (int i = 0; i < 18; ++i)
	{
		if (ch == specs[i])
		{
			return 1;
		}
	}
	return 0;
}

int is_extra_specifier_begin(char ch)
{
	char specs[6] = { 'R', 'Z', 'C', 't', 'T', 'm' };
	for (int i = 0; i < 6; ++i)
	{
		if (ch == specs[i])
		{
			return 1;
		}
	}
	return 0;
}

// min size of string roman - 16 (MMMDCCCLXXXVIII)
void int_to_roman(int number, char roman[])
{
	if (roman == NULL)
	{
		return;
	}
	number %= 4000;
	if (number == 0)
	{
		sprintf(roman, " ");
		return;
	}
	int int_values[13] = { 1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1 };
	char* roman_values[13] = { "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" };
	int ind = 0;
	int offset = 0;
	while (number > 0)
	{
		while (number >= int_values[ind])
		{
			sprintf(roman + offset, "%s", roman_values[ind]);
			number -= int_values[ind];
			offset += roman_values[ind][1] ? 2 : 1;
		}
		ind++;
	}
}

// min size of string zeckendorf - 47
void uint_to_zeckendorf(unsigned number, char zeckendorf[])
{
	if (zeckendorf == NULL)
	{
		return;
	}
	unsigned cur = FIBONACCI_47;
	unsigned next = FIBONACCI_46;
	int max_ind = 0;
	for (int i = 45; i >= 0; --i)
	{
		if (number >= cur)
		{
			number -= cur;
			zeckendorf[i] = '1';
			max_ind = max_ind ? max_ind : i;
		}
		else
		{
			zeckendorf[i] = '0';
		}
		unsigned tmp = next;
		next = cur - next;
		cur = tmp;
	}
	zeckendorf[max_ind + 1] = '1';
	zeckendorf[max_ind + 2] = '\0';
}

int ctoi(char ch)
{
	if (isdigit(ch))
	{
		return ch - '0';
	}
	if (isalpha(ch))
	{
		return tolower(ch) - 'a' + 10;
	}
	return -1;
}

char itoc(int number, int uppercase)
{
	if (number >= 0 && number < 10)
	{
		return '0' + number;
	}
	if (number >= 10 && number < 36)
	{
		char ch = uppercase ? 'A' : 'a';
		return ch + number - 10;
	}
	return '\0';
}

// min size of strint res - 34
void int_to_str_int(int number, int base, int uppercase, char str_int[])
{
	if (str_int == NULL)
	{
		return;
	}
	if (base < 2 || base > 36)
	{
		base = 10;
	}
	int iter = 0;
	while (number != 0)
	{
		str_int[iter++] = itoc(abs(number % base), uppercase);
		if (number == -1)
		{
			str_int[iter++] = '-';
		}
		number /= base;
	}
	str_int[iter] = '\0';
	for (int i = 0; 2*i < iter-1; ++i)
	{
		char tmp = str_int[i];
		str_int[i] = str_int[iter-i-1];
		str_int[iter-i-1] = tmp;
	}
}

// min size of strint res - 34
int str_int_to_int(const char* str_int, int base, int uppercase, int* number)
{
	if (str_int == NULL)
	{
		return -1;
	}
	if (base < 2 || base > 36)
	{
		base = 10;
	}
	uppercase = uppercase ? 1 : 0;
	for (ull i = 0; str_int[i]; ++i)
	{
		if (isalpha(str_int[i]) && isupper(str_int[i]) != uppercase)
		{
			return -1;
		}
	}
	char* ptr;
	*number = strtoll(str_int, &ptr, base);
	if (*ptr != '\0')
	{
		return -1;
	}
	return 0;
}

int overfprintf(FILE* file, const char* format, ...)
{
	if (file == NULL || format == NULL)
	{
		return -1;
	}
	
	ull spec_len = 1;
	ull spec_size = 4;
	char* spec = (char*) malloc(sizeof(char) * 4);
	if (spec == NULL)
	{
		return -1;
	}
	spec[0] = '%';
	
	int print_cnt = 0;
	va_list va;
	va_start(va, format);
	
	int spec_flag = 0;
	const char* frm_ptr = format;
	while (*frm_ptr)
	{
		if (!spec_flag)
		{
			if (*frm_ptr == '%')
			{
				++frm_ptr;
				spec_flag = 1;
				if (*frm_ptr == '%')
				{
					spec_flag = 0;
					putc(*frm_ptr++, file);
					++print_cnt;
				}
			}
			else
			{
				putc(*frm_ptr++, file);
				++print_cnt;
			}
		}
		else
		{
			if (spec_len + 2 == spec_size)
			{
				spec_size *= 2;
				char* tmp = (char*) realloc(spec, sizeof(char) * spec_size);
				if (tmp == NULL)
				{
					free(spec);
					va_end(va);
					return -1;
				}
				spec = tmp;
			}
			spec[spec_len++] = *frm_ptr++;
			char last_ch = spec[spec_len - 1];
			// --- HANDLE BASE SPECIFIERS ---
			if (is_base_specifier(last_ch))
			{
				spec[spec_len] = '\0';
				int cnt = vfprintf(file, spec, va);
				va_arg(va, void*);
				if (cnt == -1)
				{
					va_end(va);
					free(spec);
					return -1;
				}
				print_cnt += cnt;
				spec_len = 1;
				spec_flag = 0;
			}
			// --- HANDLE EXTRA SPECIFIERS ---
			else if (is_extra_specifier_begin(last_ch))
			{
				spec[spec_len++] = *frm_ptr++;
				spec[spec_len] = '\0';
				// -- HANDLE ROMAN NUMERALS --
				if (!strcmp(spec, "%Ro"))
				{
					int number = va_arg(va, int);
					char roman[16];
					int_to_roman(number, roman);
					print_cnt += fprintf(file, "%s", roman);
				}
				// -- HANDLE ZECKENDORF RESPRESENTATION --
				else if (!strcmp(spec, "%Zr"))
				{
					unsigned number = va_arg(va, unsigned);
					char zeckendorf[49];
					uint_to_zeckendorf(number, zeckendorf);
					print_cnt += fprintf(file, "%s", zeckendorf);
				}
				// -- HANDLE CONVERTING INTEGER TO CERTAIN BASE --
				else if (!strcmp(spec, "%Cv") || !strcmp(spec, "%CV"))
				{
					int number = va_arg(va, int);
					int base = va_arg(va, int);
					int uppercase = *frm_ptr == 'V';
					char str_int[34];
					int_to_str_int(number, base, uppercase, str_int);
					print_cnt += fprintf(file, "%s", str_int);
				}
				// -- HANDLE CONVERTING INTEGER FROM CERTAIN BASE --
				else if (!strcmp(spec, "%to") || !strcmp(spec, "%TO"))
				{
					char* str_int = va_arg(va, char*);
					int base = va_arg(va, int);
					int uppercase = *frm_ptr == 'O';
					int number;
					if (str_int_to_int(str_int, base, uppercase, &number))
					{
						return -1;
					}
					print_cnt += fprintf(file, "%d", number);
				}
				// -- HANDLE INT DUMP --
				else if (!strcmp(spec, "%mi"))
				{
					ull data = va_arg(va, int);
					for (ull i = 0; i < 8 * sizeof(int); ++i)
					{
						char bit = (data & (1 << (8 * sizeof(int) - i - 1))) ? '1' : '0';
						putc(bit, file);
						if (i > 0 && i % 8 == 0)
						{
							putc(' ', file);
						}
					}
					print_cnt += 35;
				}
				// -- HANDLE UINT DUMP --
				else if (!strcmp(spec, "%mu"))
				{
					ull data = va_arg(va, unsigned);
					for (ull i = 0; i < 8 * sizeof(unsigned); ++i)
					{
						char bit = (data & (1 << (8 * sizeof(unsigned) - i - 1))) ? '1' : '0';
						putc(bit, file);
						if (i > 0 && i % 8 == 0)
						{
							putc(' ', file);
						}
					}
					print_cnt += 35;
				}
				// -- HANDLE DOUBLE DUMP --
				else if (!strcmp(spec, "%md"))
				{
					ull data = va_arg(va, double);
					for (ull i = 0; i < 8 * sizeof(double); ++i)
					{
						char bit = (data & (1 << (8 * sizeof(double) - i - 1))) ? '1' : '0';
						putc(bit, file);
						if (i > 0 && i % 8 == 0)
						{
							putc(' ', file);
						}
					}
					print_cnt += 35;
				}
				// -- HANDLE FLOAT DUMP --
				else if (!strcmp(spec, "%mf"))
				{
					ull data = va_arg(va, double);
					for (ull i = 0; i < 8 * sizeof(float); ++i)
					{
						char bit = (data & (1 << (8 * sizeof(float) - i - 1))) ? '1' : '0';
						putc(bit, file);
						if (i > 0 && i % 8 == 0)
						{
							putc(' ', file);
						}
					}
					print_cnt += 35;
				}
				else
				{
					va_end(va);
					free(spec);
					return -1;
				}
				spec_len = 1;
				spec_flag = 0;
			}
			// --- HANDLE INVALID SPECIFIERS ---
			else if (isalpha(last_ch) && last_ch != 'l' && last_ch != 'h')
			{
				va_end(va);
				free(spec);
				return -1;
			}
		}
	}
	va_end(va);
	free(spec);
	if (spec_len != 1)
	{
		return -1;
	}
	return print_cnt;
}

int oversprintf(char* dest, const char* format, ...)
{
	if (dest == NULL || format == NULL)
	{
		return -1;
	}
	
	ull spec_len = 1;
	ull spec_size = 4;
	char* spec = (char*) malloc(sizeof(char) * 4);
	if (spec == NULL)
	{
		return -1;
	}
	spec[0] = '%';
	
	ull dest_len = 0;
	va_list va;
	va_start(va, format);
	
	int spec_flag = 0;
	const char* frm_ptr = format;
	while (*frm_ptr)
	{
		if (!spec_flag)
		{
			if (*frm_ptr == '%')
			{
				++frm_ptr;
				spec_flag = 1;
				if (*frm_ptr == '%')
				{
					spec_flag = 0;
					dest[dest_len++] = *frm_ptr++;
				}
			}
			else
			{
				dest[dest_len++] = *frm_ptr++;
			}
		}
		else
		{
			if (spec_len + 2 == spec_size)
			{
				spec_size *= 2;
				char* tmp = (char*) realloc(spec, sizeof(char) * spec_size);
				if (tmp == NULL)
				{
					free(spec);
					va_end(va);
					return -1;
				}
				spec = tmp;
			}
			spec[spec_len++] = *frm_ptr++;
			char last_ch = spec[spec_len - 1];
			// --- HANDLE BASE SPECIFIERS ---
			if (is_base_specifier(last_ch))
			{
				spec[spec_len] = '\0';
				int cnt = vsprintf(dest + dest_len, spec, va);
				va_arg(va, void*);
				if (cnt == -1)
				{
					va_end(va);
					free(spec);
					return -1;
				}
				dest_len += cnt;
				spec_len = 1;
				spec_flag = 0;
			}
			// --- HANDLE EXTRA SPECIFIERS ---
			else if (is_extra_specifier_begin(last_ch))
			{
				spec[spec_len++] = *frm_ptr++;
				spec[spec_len] = '\0';
				// -- HANDLE ROMAN NUMERALS --
				if (!strcmp(spec, "%Ro"))
				{
					int number = va_arg(va, int);
					char roman[16];
					int_to_roman(number, roman);
					dest_len += sprintf(dest + dest_len, "%s", roman);
				}
				// -- HANDLE ZECKENDORF RESPRESENTATION --
				else if (!strcmp(spec, "%Zr"))
				{
					unsigned number = va_arg(va, unsigned);
					char zeckendorf[49];
					uint_to_zeckendorf(number, zeckendorf);
					dest_len += sprintf(dest + dest_len, "%s", zeckendorf);
				}
				// -- HANDLE CONVERTING INTEGER TO CERTAIN BASE --
				else if (!strcmp(spec, "%Cv") || !strcmp(spec, "%CV"))
				{
					int number = va_arg(va, int);
					int base = va_arg(va, int);
					int uppercase = *frm_ptr == 'V';
					char str_int[34];
					int_to_str_int(number, base, uppercase, str_int);
					dest_len += sprintf(dest + dest_len, "%s", str_int);
				}
				// -- HANDLE CONVERTING INTEGER FROM CERTAIN BASE --
				else if (!strcmp(spec, "%to") || !strcmp(spec, "%TO"))
				{
					char* str_int = va_arg(va, char*);
					int base = va_arg(va, int);
					int uppercase = *frm_ptr == 'O';
					int number;
					if (str_int_to_int(str_int, base, uppercase, &number))
					{
						return -1;
					}
					dest_len += sprintf(dest + dest_len, "%d", number);
				}
				// -- HANDLE INT DUMP --
				else if (!strcmp(spec, "%mi"))
				{
					ull data = va_arg(va, int);
					for (ull i = 0; i < 8 * sizeof(int); ++i)
					{
						char bit = (data & (1 << (8 * sizeof(int) - i - 1))) ? '1' : '0';
						dest[dest_len++] = bit;
						if (i > 0 && i % 8 == 0)
						{
							dest[dest_len++] = ' ';
						}
					}
				}
				// -- HANDLE UINT DUMP --
				else if (!strcmp(spec, "%mu"))
				{
					ull data = va_arg(va, unsigned);
					for (ull i = 0; i < 8 * sizeof(unsigned); ++i)
					{
						char bit = (data & (1 << (8 * sizeof(unsigned) - i - 1))) ? '1' : '0';
						dest[dest_len++] = bit;
						if (i > 0 && i % 8 == 0)
						{
							dest[dest_len++] = ' ';
						}
					}
				}
				// -- HANDLE DOUBLE DUMP --
				else if (!strcmp(spec, "%md"))
				{
					ull data = va_arg(va, double);
					for (ull i = 0; i < 8 * sizeof(double); ++i)
					{
						char bit = (data & (1 << (8 * sizeof(double) - i - 1))) ? '1' : '0';
						dest[dest_len++] = bit;
						if (i > 0 && i % 8 == 0)
						{
							dest[dest_len++] = ' ';
						}
					}
				}
				// -- HANDLE FLOAT DUMP --
				else if (!strcmp(spec, "%mf"))
				{
					ull data = va_arg(va, double);
					for (ull i = 0; i < 8 * sizeof(float); ++i)
					{
						char bit = (data & (1 << (8 * sizeof(float) - i - 1))) ? '1' : '0';
						dest[dest_len++] = bit;
						if (i > 0 && i % 8 == 0)
						{
							dest[dest_len++] = ' ';
						}
					}
				}
				else
				{
					va_end(va);
					free(spec);
					return -1;
				}
				spec_len = 1;
				spec_flag = 0;
			}
			// --- HANDLE INVALID SPECIFIERS ---
			else if (isalpha(last_ch) && last_ch != 'l' && last_ch != 'h')
			{
				va_end(va);
				free(spec);
				return -1;
			}
		}
	}
	dest[dest_len] = '\0';
	va_end(va);
	free(spec);
	if (spec_len != 1)
	{
		return -1;
	}
	return dest_len;
}

int main(int argc, char** argv)
{
	int x = 0;
	char dest[512];
	x = overfprintf(stdout, "%lld %c %s %E\n", 5, 'q', "str", 123.456);
	x = overfprintf(stdout, "\"%Ro\" %Ro %Ro %Ro\n", 0, 4, 99, 3888);
	x = overfprintf(stdout, "%Zr %Zr %Zr %Zr %Zr %Zr\n", 0, 1, 2, 3, 4, UINT_MAX);
	x = overfprintf(stdout, "%Cv %CV %Cv\n", INT_MAX, 2,  INT_MIN, 2,  INT_MIN+1, 2);
	x = overfprintf(stdout, "%Cv %CV %Cv\n", 255, 16,  107, 36,  15, 4);
	x = overfprintf(stdout, "%to %TO %to\n", "1y", 36,  "FF", 16,  "33", 4);
	x = overfprintf(stdout, "%mi\n");
	x = overfprintf(stdout, "%mu\n");
	x = overfprintf(stdout, "%md\n");
	x = overfprintf(stdout, "%mf\n");
	printf("%d\n\n", x);
	
	x = oversprintf(dest, "%lld %c %s %E\n", 5, 'q', "str", 123.456);
	printf("%s", dest);
	x = oversprintf(dest, "\"%Ro\" %Ro %Ro %Ro\n", 0, 4, 99, 3888);
	printf("%s", dest);
	x = oversprintf(dest, "%Zr %Zr %Zr %Zr %Zr %Zr\n", 0, 1, 2, 3, 4, UINT_MAX);
	printf("%s", dest);
	x = oversprintf(dest, "%Cv %CV %Cv\n", INT_MAX, 2,  INT_MIN, 2,  INT_MIN+1, 2);
	printf("%s", dest);
	x = oversprintf(dest, "%Cv %CV %Cv\n", 255, 16,  107, 36,  15, 4);
	printf("%s", dest);
	x = oversprintf(dest, "%to %TO %to\n", "1y", 36,  "FF", 16,  "33", 4);
	printf("%s", dest);
	x = oversprintf(dest, "%mi\n");
	printf("%s", dest);
	x = oversprintf(dest, "%mu\n");
	printf("%s", dest);
	x = oversprintf(dest, "%md\n");
	printf("%s", dest);
	x = oversprintf(dest, "%mf\n");
	printf("%s", dest);
	printf("%d\n", x);
}