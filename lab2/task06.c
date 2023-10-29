#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>

typedef long long ll;
typedef unsigned long long ull;

typedef enum
{
	OK,
	INVALID_INPUT,
	INVALID_FLAG,
	INVALID_NUMBER,
	FILE_OPENING_ERROR,
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
		case FILE_OPENING_ERROR:
			printf("File cannot be opened\n");
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
	NONE,
	CHAR,
	STRING,
	INT,
	I_INT,
	UINT,
	OCT_UINT,
	HEX_UINT,
	CHAR_N,
	FLOAT,
	PTR
} specifiers;

specifiers get_base_spec_enum(char ch)
{
	switch (ch)
	{
		case 'c':
			return CHAR;
		case 's':
			return STRING;
		case 'd':
			return INT;
		case 'i':
			return I_INT;
		case 'u':
			return UINT;
		case 'o':
			return OCT_UINT;
		case 'x':
		case 'X':
			return HEX_UINT;
		case 'n':
			return CHAR_N;
		case 'e':
		case 'E':
		case 'f':
		case 'F':
		case 'g':
		case 'G':
			return FLOAT;
		case 'p':
			return PTR;
		default:
			return NONE;
	}
}

int is_separator(char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n';
}

int ctoi(char ch)
{
	if (isdigit(ch))
	{
		return ch - '0';
	}
	if (isalpha(ch))
	{
		return toupper(ch) - 'A' + 10;
	}
	return -1;
}

int is_digit_b(char ch, int base)
{
	int val = ctoi(ch);
	if (val == -1)
	{
		return 0;
	}
	return val < base;
}

int is_base_specifier(char ch)
{
	char specs[16] = { 'c', 's', 'd', 'i', 'u', 'o', 'x', 'X', 'n', 'e', 'E', 'f', 'F', 'g', 'G', 'p' };
	
	for (int i = 0; i < 16; ++i)
	{
		if (ch == specs[i])
		{
			return 1;
		}
	}
	return 0;
}

int is_spec_combination_valid(char* len_mod, specifiers spec)
{
	if (len_mod == NULL)
	{
		return 0;
	}
	switch (spec)
	{
		case CHAR:
		case STRING:
			return !len_mod[0] || !strcmp(len_mod, "l");
		case INT:
		case I_INT:
		case UINT:
		case OCT_UINT:
		case HEX_UINT:
		case CHAR_N:
			return !!strcmp(len_mod, "L");
		case FLOAT:
			return !len_mod[0] || !strcmp(len_mod, "l") || !strcmp(len_mod, "L");
		case PTR:
			return !len_mod[0];
		default:
			return 0;
	}
}

status_codes sreadll(const char* src_ptr, int base, ll field_width, ll* res, const char** res_ptr)
{
	if (src_ptr == NULL || res == NULL || res_ptr == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	const char* start_ptr = src_ptr;
	int neg_flag = 0;
	ll num = 0;
	if (*src_ptr == '-')
	{
		neg_flag = 1;
		++src_ptr;
	}
	int error_flag = 1;
	int auto_base = 0;
	if (*src_ptr == '0')
	{
		auto_base = 8;
		error_flag = 0; // "0" "%i" - is ok
		++src_ptr;
		if ((*src_ptr == 'x' || *src_ptr == 'X') && (base == 0 || base == 16))
		{
			auto_base = 16;
			error_flag = 1; // "0x" "%i" - is not ok
			++src_ptr;
		}
	}
	else
	{
		auto_base = 10;
	}
	base = base == 0 ? auto_base : base;
	
	while (is_digit_b(*src_ptr, base) && (field_width == -1 || (src_ptr - start_ptr) < field_width))
	{
		num *= base;
		num += ctoi(*src_ptr);
		++src_ptr;
		error_flag = 0;
	}
	if (neg_flag)
	{
		num *= -1;
	}
	if (error_flag)
	{
		*res_ptr = src_ptr;
		return INVALID_INPUT;
	}
	*res = num;
	*res_ptr = src_ptr;
	return OK;
}

status_codes sreadllf(const char* src_ptr, ll field_width, long double* res, const char** res_ptr)
{
	if (src_ptr == NULL || res == NULL || res_ptr == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	const char* start_ptr = src_ptr;
	int neg_flag = 0;
	long double num = 0;
	int error_flag = 1;
	if (*src_ptr == '-')
	{
		neg_flag = 1;
		++src_ptr;
	}
	error_flag = 1;
	while (isdigit(*src_ptr) && (field_width == -1 || (src_ptr - start_ptr) < field_width))
	{
		num *= 10;
		num += ctoi(*src_ptr);
		++src_ptr;
		error_flag = 0;
	}
	if (*src_ptr == '.')
	{
		++src_ptr;
		long double numer = 0;
		long double denom = 1;
		while (isdigit(*src_ptr) && (field_width == -1 || (src_ptr - start_ptr) < field_width))
		{
			numer *= 10;
			numer += ctoi(*src_ptr);
			denom *= 10;
			++src_ptr;
			error_flag = 0;
		}
		num += numer / denom;
	}
	if (*src_ptr == 'e' || *src_ptr == 'E')
	{
		++src_ptr;
		ll power = 0;
		ll new_width = field_width == -1 ? -1 : field_width - (src_ptr - start_ptr);
		if (sreadll(src_ptr, 10, new_width, &power, &src_ptr) == INVALID_INPUT)
		{
			*res_ptr = src_ptr;
			return INVALID_INPUT;
		}
		num *= pow(10, power);
	}
	if (neg_flag)
	{
		num *= -1;
	}
	if (error_flag)
	{
		*res_ptr = src_ptr;
		return INVALID_INPUT;
	}
	*res = num;
	*res_ptr = src_ptr;
	return OK;
}

int is_digit_ro(char ch)
{
	return ch == 'I' || ch == 'V' || ch == 'X' || ch == 'L' || ch == 'C' || ch == 'D' || ch == 'M';
}

int rotoi(char ch)
{
	switch (ch)
	{
		case 'I':
			return 1;
		case 'V':
			return 5;
		case 'X':
			return 10;
		case 'L':
			return 50;
		case 'C':
			return 100;
		case 'D':
			return 500;
		case 'M':
			return 1000;
		default:
			return 0;
	}
}

status_codes sreadro(const char* src_ptr, ll field_width, int* res, const char** res_ptr)
{
	if (src_ptr == NULL || res == NULL || res_ptr == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	const char* start_ptr = src_ptr;
	ll num = 0;
	int error_flag = 1;
	int run_flag = 1;
	
	char prev = ' ';
	char row_cnt = 1;
	int decr = 0;
	
	while (run_flag && is_digit_ro(*src_ptr) && (field_width == -1 || (src_ptr - start_ptr) < field_width))
	{
		row_cnt = (*src_ptr == prev) ? (row_cnt + 1) : 1;
		
		if (row_cnt > 3)
		{
			run_flag = 0;
		}
		
		if (prev != ' ')
		{
			if (rotoi(prev) < rotoi(*src_ptr))
			{
				if (decr || row_cnt > 1)
				{
					run_flag = 0;
				}
				else
				{
					decr = rotoi(prev);
				}
			}
			else
			{
				if (decr && (rotoi(prev) == 2 * rotoi(*src_ptr)))
				{
					run_flag = 0;
				}
				else
				{
					num += rotoi(prev) - decr;
					decr = 0;
				}
			}
		}
		
		prev = *src_ptr;
		if (run_flag)
		{
			++src_ptr;
			error_flag = 0;
		}
	}
	num += rotoi(prev) - decr;
	
	if (error_flag)
	{
		*res_ptr = src_ptr;
		return INVALID_INPUT;
	}
	*res = num;
	*res_ptr = src_ptr;
	return OK;
}

int oversscanf(const char* src, const char* format, ...)
{
	int read_cnt = 0;
	const char* src_ptr = src;
	const char* frm_ptr = format;
	
	va_list arg;
	va_start(arg, format);
	
	while (*frm_ptr)
	{
		if (is_separator(*frm_ptr))
		{
			while (is_separator(*src_ptr))
			{
				++src_ptr;
			}
			++frm_ptr;
		}
		else if (*frm_ptr != '%')
		{
			if (*frm_ptr != *src_ptr)
			{
				return read_cnt;
			}
			++frm_ptr;
			++src_ptr;
		}
		// HANDLE CONVERSION SPECIFICATION
		else
		{
			// PART 1: GET AND VALIDATE CONV SPEC
			int suppress = 0;
			ll field_width = -1;
			char* length_modifier = (char*) calloc(3, sizeof(char));
			specifiers spec = NONE;
			
			++frm_ptr;
			// HANDLE ASSIGNMENT-SUPPRESSING CHARACTER
			if (*frm_ptr == '*')
			{
				suppress = 1;
				++frm_ptr;
			}
			// HANDLE MAXIMUM FIELD WIDTH
			if (isdigit(*frm_ptr))
			{
				field_width = 0;
				while (isdigit(*frm_ptr))
				{
					field_width *= 10;
					field_width += *frm_ptr - '0';
					++frm_ptr;
				}
			}
			// HANDLE LENGTH MODIFIER
			if (*frm_ptr == 'h' || *frm_ptr == 'l')
			{
				length_modifier[0] = *frm_ptr;
				++frm_ptr;
			}
			if (length_modifier[0] == *frm_ptr && (*frm_ptr == 'h' || *frm_ptr == 'l'))
			{
				length_modifier[1] = *frm_ptr;
				++frm_ptr;
			}
			// HANDLE CONVERSION FORMAT SPECIFIER
			if (is_base_specifier(*frm_ptr))
			{
				spec = get_base_spec_enum(*frm_ptr);
				++frm_ptr;
			}
			// HANDLE EXTRA CONVERSION FORMAT SPECIFIER
			if (0)
			{
				// TODO
			}
			
			if (spec == NONE || !is_spec_combination_valid(length_modifier, spec) || field_width == 0)
			{
				free(length_modifier);
				va_end(arg);
				return read_cnt;
			}
			
			// PART 2: WRITE DATA
			void* write_ptr = NULL;
			if (!suppress)
			{
				write_ptr = va_arg(arg, void*);
				if (write_ptr == NULL)
				{
					free(length_modifier);
					va_end(arg);
					return read_cnt;
				}
			}
			int error_flag = 0;
			while (spec != CHAR && spec != CHAR_N && is_separator(*src_ptr))
			{
				++src_ptr;
			}
			switch(spec)
			{
				case CHAR:
				{
					if (!suppress)
					{
						char* ch_ptr = (char*) write_ptr;
						*ch_ptr = *src_ptr;
					}
					++src_ptr;
					break;
				}
				case STRING:
				{
					char* str_ptr = suppress ? NULL : (char*) write_ptr;
					ull i = 0;
					error_flag = 1;
					while (*src_ptr && !is_separator(*src_ptr) && (field_width == -1 || i < field_width))
					{
						if (!suppress)
						{
							str_ptr[i] = *src_ptr;
						}
						error_flag = 0;
						++src_ptr;
						++i;
					}
					if (!suppress)
					{
						str_ptr[i] = '\0';
					}
					break;
				}
				case INT:
				{
					int base = 0;
					ll num = 0;
					if (sreadll(src_ptr, base, field_width, &num, &src_ptr) == INVALID_INPUT)
					{
						error_flag = 1;
					}
					if (!suppress)
					{
						if (!strcmp(length_modifier, "hh"))
						{
							char* ch_ptr = (char*) write_ptr;
							*ch_ptr = UCHAR_MAX & num;
						}
						else if (!strcmp(length_modifier, "h"))
						{
							short* sh_ptr = (short*) write_ptr;
							*sh_ptr = USHRT_MAX & num;
						}
						else if (!strcmp(length_modifier, ""))
						{
							int* int_ptr = (int*) write_ptr;
							*int_ptr = UINT_MAX & num;
						}
						else if (!strcmp(length_modifier, "l"))
						{
							long* l_ptr = (long*) write_ptr;
							*l_ptr = ULONG_MAX & num;
						}
						else if (!strcmp(length_modifier, "ll"))
						{
							long long* ll_ptr = (long long*) write_ptr;
							*ll_ptr = ULLONG_MAX & num;
						}
					}
					break;
				}
				case I_INT:
				{
					int base = 0;
					ll num = 0;
					if (sreadll(src_ptr, base, field_width, &num, &src_ptr) == INVALID_INPUT)
					{
						error_flag = 1;
					}
					if (!suppress)
					{
						if (!strcmp(length_modifier, "hh"))
						{
							char* ch_ptr = (char*) write_ptr;
							*ch_ptr = UCHAR_MAX & num;
						}
						else if (!strcmp(length_modifier, "h"))
						{
							short* sh_ptr = (short*) write_ptr;
							*sh_ptr = USHRT_MAX & num;
						}
						else if (!strcmp(length_modifier, ""))
						{
							int* int_ptr = (int*) write_ptr;
							*int_ptr = UINT_MAX & num;
						}
						else if (!strcmp(length_modifier, "l"))
						{
							long* l_ptr = (long*) write_ptr;
							*l_ptr = ULONG_MAX & num;
						}
						else if (!strcmp(length_modifier, "ll"))
						{
							long long* ll_ptr = (long long*) write_ptr;
							*ll_ptr = ULLONG_MAX & num;
						}
					}
					break;
				}
				case UINT:
				case OCT_UINT:
				case HEX_UINT:
				{
					int base = spec == UINT ? 10 : (spec == OCT_UINT ? 8 : 16);
					ll num = 0;
					if (sreadll(src_ptr, base, field_width, &num, &src_ptr) == INVALID_INPUT)
					{
						error_flag = 1;
					}
					if (!suppress)
					{
						if (!strcmp(length_modifier, "hh"))
						{
							unsigned char* ch_ptr = (unsigned char*) write_ptr;
							*ch_ptr = UCHAR_MAX & num;
						}
						else if (!strcmp(length_modifier, "h"))
						{
							unsigned short* sh_ptr = (unsigned short*) write_ptr;
							*sh_ptr = USHRT_MAX & num;
						}
						else if (!strcmp(length_modifier, ""))
						{
							unsigned int* int_ptr = (unsigned int*) write_ptr;
							*int_ptr = UINT_MAX & num;
						}
						else if (!strcmp(length_modifier, "l"))
						{
							unsigned long* l_ptr = (unsigned long*) write_ptr;
							*l_ptr = ULONG_MAX & num;
						}
						else if (!strcmp(length_modifier, "ll"))
						{
							unsigned long long* ll_ptr = (unsigned long long*) write_ptr;
							*ll_ptr = ULLONG_MAX & num;
						}
					}
					break;
				}
				case CHAR_N:
				{
					if (!suppress)
					{
						if (!strcmp(length_modifier, "hh"))
						{
							unsigned char* ch_ptr = (unsigned char*) write_ptr;
							*ch_ptr = UCHAR_MAX & (src_ptr - src);
						}
						else if (!strcmp(length_modifier, "h"))
						{
							unsigned short* sh_ptr = (unsigned short*) write_ptr;
							*sh_ptr = USHRT_MAX & (src_ptr - src);
						}
						else if (!strcmp(length_modifier, ""))
						{
							unsigned int* int_ptr = (unsigned int*) write_ptr;
							*int_ptr = UINT_MAX & (src_ptr - src);
						}
						else if (!strcmp(length_modifier, "l"))
						{
							unsigned long* l_ptr = (unsigned long*) write_ptr;
							*l_ptr = ULONG_MAX & (src_ptr - src);
						}
						else if (!strcmp(length_modifier, "ll"))
						{
							unsigned long long* ll_ptr = (unsigned long long*) write_ptr;
							*ll_ptr = ULLONG_MAX & (src_ptr - src);
						}
					}
					break;
				}
				case FLOAT:
				{
					long double num = 0;
					if (sreadllf(src_ptr, field_width, &num, &src_ptr) == INVALID_INPUT)
					{
						error_flag = 1;
					}
					if (!suppress)
					{
						if (!strcmp(length_modifier, ""))
						{
							float* f_ptr = (float*) write_ptr;
							*f_ptr = (float) num;
						}
						else if (!strcmp(length_modifier, "l"))
						{
							double* lf_ptr = (double*) write_ptr;
							*lf_ptr = (double) num;
						}
						else if (!strcmp(length_modifier, "L"))
						{
							long double* llf_ptr = (long double*) write_ptr;
							*llf_ptr = (long double) num;
						}
					}
					break;
				}
				case PTR:
				{
					int base = 16;
					ll num = 0;
					if (sreadll(src_ptr, base, field_width, &num, &src_ptr) == INVALID_INPUT)
					{
						error_flag = 1;
					}
					if (!suppress)
					{
						void** void_ptr = (void**) write_ptr;
						*void_ptr = (void*) num;
					}
					break;
				}
				default:
					error_flag = 1;
					break;
			}
			
			free(length_modifier);
			if (error_flag)
			{
				return read_cnt;
			}
			if (!suppress && spec != CHAR_N)
			{
				++read_cnt;
			}
		}
	}
	
	va_end(arg);
	return read_cnt;
}

int main(int argc, char** argv)
{
	char ch;
	char str[100] = "NO DATA", str2[100] = "NO DATA";
	int d, octd, hexd;
	unsigned u, o, x, xx, n;
	
	int cnt = oversscanf("c lambda", "%c %s", &ch, str);
	cnt += oversscanf("-12 34 10 10 skip_me 0x20", "%d %u %o %x %*s %X %n", &d, &u, &o, &x, &xx, &n);
	printf("1: cnt=%d\n", cnt);
	printf("%c %s %d %d %d %d %d %d\n", ch, str, d, u, o, x, xx, n);
	
	cnt = oversscanf("-10 -010 -0x10", "%i%i%i", &d, &octd, &hexd);
	printf("2: cnt=%d\n%d %d %d\n", cnt, d, octd, hexd);
	
	signed char q;
	cnt = oversscanf("     128", "%hhd", &q);
	printf("3: cnt=%d\n%d\n", cnt, q);
	
	cnt = oversscanf("1234567", "%3s%2d%s", str, &d, str2);
	printf("4: cnt=%d\n%s %d %s\n", cnt, str, d, str2);
	
	cnt = oversscanf("abc", "%s%s", str, str2);
	printf("5: cnt=%d\n%s %s\n", cnt, str, str2);
	
	cnt = oversscanf("abc", "%lls", str);
	printf("6: cnt=%d\n%s\n", cnt, str);
	
	double a = 0, b = 0, c = 0;
	float fa = 0;
	cnt = oversscanf(".123 5.5 1.2e3", "%lf %lf %lf", &a, &b, &c);
	printf("7: cnt=%d\n%lf %lf %lf\n", cnt, a, b, c);
	cnt = oversscanf("1e45 1e45 1e45", "%f %lf %Lf", &fa, &a);
	printf("cnt=%d\n%lf %lf\n", cnt, fa, a);
	
	
	cnt = oversscanf("skip read 123", "%*s %s %d", str, &d);
	printf("111 cnt=%d\n%s %d\n", cnt, str, d);
}