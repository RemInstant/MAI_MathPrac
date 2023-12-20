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
	// BASE
	NONE,
	CHAR,
	STRING,
	SET,
	INT,
	I_INT,
	UINT,
	OCT_UINT,
	HEX_UINT,
	CHAR_N,
	FLOAT,
	PTR,
	// SPECIFIC
	ROMAN,
	ZECKENDORF,
	BASED_INT_L,
	BASED_INT_U
} specifiers;

specifiers get_base_spec_enum(char ch)
{
	switch (ch)
	{
		case 'c':
			return CHAR;
		case 's':
			return STRING;
		case '[':
			return SET;
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
	char specs[17] = { 'c', 's', '[', 'd', 'i', 'u', 'o', 'x', 'X', 'n', 'e', 'E', 'f', 'F', 'g', 'G', 'p' };
	
	for (int i = 0; i < 17; ++i)
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
		case SET:
			return !len_mod[0];
		case INT:
		case I_INT:
		case UINT:
		case OCT_UINT:
		case HEX_UINT:
		case CHAR_N:
			return 1;
		case FLOAT:
			return !len_mod[0] || !strcmp(len_mod, "l");
		case PTR:
			return !len_mod[0];
		case ROMAN:
		case ZECKENDORF:
		case BASED_INT_L:
		case BASED_INT_U:
			return !len_mod[0];
		default:
			return 0;
	}
}

typedef enum
{
	ANY,
	LOWER,
	UPPER
} letter_cases;

int check_letter_case(char ch, letter_cases l_case)
{
	switch (l_case)
	{
		case ANY:
			return 1;
		case LOWER:
			return islower(ch);
		case UPPER:
			return isupper(ch);
		default:
			return 0;
	}
}

status_codes fread_ll(FILE* src_file, int base, ll field_width, letter_cases l_case, ll* res, ull* char_cnt, char* last_ch)
{
	if (src_file == NULL || res == NULL || char_cnt == NULL || last_ch == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	*char_cnt = 0;
	ull counter = 0;
	int error_flag = 1;
	int neg_flag = 0;
	int auto_base = 0;
	ll num = 0;
	char ch = getc(src_file);
	if (ch == '-')
	{
		neg_flag = 1;
		ch = getc(src_file);
		++counter;
	}
	if (ch == '0')
	{
		auto_base = 8;
		error_flag = 0; // "0" "%i" - is ok
		ch = getc(src_file);
		++counter;
		if ((ch == 'x' || ch == 'X') && (base == 0 || base == 16))
		{
			auto_base = 16;
			error_flag = 1; // "0x" "%i" - is not ok
			ch = getc(src_file);
			++counter;
		}
	}
	else
	{
		auto_base = 10;
	}
	base = base == 0 ? auto_base : base;
	
	while (is_digit_b(ch, base) && (isdigit(ch) || check_letter_case(ch, l_case))
			&& (field_width == -1 || counter < field_width))
	{
		num *= base;
		num += ctoi(ch);
		ch = getc(src_file);
		++counter;
		error_flag = 0;
	}
	if (neg_flag)
	{
		num *= -1;
	}
	if (error_flag)
	{
		return INVALID_INPUT;
	}
	*res = num;
	*char_cnt = counter;
	*last_ch = ch;
	return OK;
}

status_codes fread_lf(FILE* src_file, ll field_width, double* res, ull* char_cnt, char* last_ch)
{
	if (src_file == NULL || res == NULL || char_cnt == NULL || last_ch == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	*char_cnt = 0;
	ull counter = 0;
	int error_flag = 1;
	int neg_flag = 0;
	double num = 0;
	char ch = getc(src_file);
	if (ch == '-')
	{
		neg_flag = 1;
		ch = getc(src_file);
		++counter;
	}
	while (isdigit(ch) && (field_width == -1 || counter < field_width))
	{
		num *= 10;
		num += ctoi(ch);
		ch = getc(src_file);
		++counter;
		error_flag = 0;
	}
	if (ch == '.')
	{
		ch = getc(src_file);
		++counter;
		double numer = 0;
		double denom = 1;
		while (isdigit(ch) && (field_width == -1 || counter < field_width))
		{
			numer *= 10;
			numer += ctoi(ch);
			denom *= 10;
			ch = getc(src_file);
			++counter;
			error_flag = 0;
		}
		num += numer / denom;
	}
	if (ch == 'e' || ch == 'E')
	{
		ull local_counter = 0;
		ll power = 0;
		ll new_width = field_width == -1 ? -1 : field_width - counter;
		if (fread_ll(src_file, 10, new_width, ANY, &power, &local_counter, &ch) == INVALID_INPUT)
		{
			return INVALID_INPUT;
		}
		num *= pow(10, power);
		counter += local_counter;
	}
	if (neg_flag)
	{
		num *= -1;
	}
	if (error_flag)
	{
		return INVALID_INPUT;
	}
	*res = num;
	*char_cnt = counter;
	*last_ch = ch;
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

status_codes fread_ro(FILE* src_file, ll field_width, int* res, ull* char_cnt, char* last_ch)
{
	if (src_file == NULL || res == NULL || char_cnt == NULL || last_ch == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	*char_cnt = 0;
	ull counter = 0;
	ll num = 0;
	char prev = ' ';
	char cur = getc(src_file);
	
	if(is_digit_ro(cur))
	{
		prev = cur;
		cur = getc(src_file);
		++counter;
	}
	else
	{
		return INVALID_INPUT;
	}
	
	while(is_digit_ro(cur) && (field_width == -1 || counter < field_width))
	{
		if (rotoi(prev) < rotoi(cur))
		{
			num -= rotoi(prev);
		}
		else
		{
			num += rotoi(prev);
		}
		prev = cur;
		cur = getc(src_file);
		++counter;
	}
	num += rotoi(prev);
	
	*res = num;
	*char_cnt = counter;
	*last_ch = cur;
	return OK;
}

status_codes fread_zr(FILE* src_file, ll field_width, unsigned int* res, ull* char_cnt, char* last_ch)
{
	if (src_file == NULL || res == NULL || char_cnt == NULL || last_ch == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	*char_cnt = 0;
	ull counter = 0;
	ll prev_fib = 1;
	ll cur_fib = 1;
	ll num = 0;
	int prev_coef = 0;
	int coef = 0;
	int error_flag = 1;
	char ch = getc(src_file);
	
	while (isdigit(ch) && (coef = ctoi(ch)) < 2 && (field_width == -1 || counter < field_width))
	{
		if (coef == 1)
		{
			if (prev_coef == 1)
			{
				*res = num;
				*char_cnt = counter;
				*last_ch = getc(src_file);
				return OK;
			}
			num += cur_fib;
		}
		cur_fib += prev_fib;
		prev_fib = cur_fib - prev_fib;
		prev_coef = coef;
		error_flag = 0;
		ch = getc(src_file);
		++counter;
	}
	if (prev_coef != 1)
	{
		error_flag = 1;
	}
	if (error_flag)
	{
		return INVALID_INPUT;
	}
	*res = num;
	*char_cnt = counter;
	*last_ch = ch;
	return OK;
}

int overfscanf(FILE* src_file, const char* format, ...)
{
	if (src_file == NULL || format == NULL)
	{
		return -1;
	}
	int read_cnt = 0;
	ull counter = 0;
	char ch = getc(src_file);
	const char* frm_ptr = format;
	
	va_list arg;
	va_start(arg, format);
	
	while (*frm_ptr)
	{
		if (is_separator(*frm_ptr))
		{
			while (is_separator(ch))
			{
				ch = getc(src_file);
				++counter;
			}
			++frm_ptr;
		}
		else if (ch == EOF)
		{
			return read_cnt ? read_cnt : -1;
		}
		else if (*frm_ptr != '%')
		{
			if (*frm_ptr != ch)
			{
				ungetc(ch, src_file);
				return read_cnt ? read_cnt : -1;
			}
			ch = getc(src_file);
			++counter;
			++frm_ptr;
		}
		else
		{
			++frm_ptr;
			// HANDLE DOUBLE PERCENT
			if (*frm_ptr == '%')
			{
				if (*frm_ptr != ch)
				{
					ungetc(ch, src_file);
					return read_cnt;
				}
				ch = getc(src_file);
				++counter;
				++frm_ptr;
			}
			// HANDLE CONVERSION SPECIFICATION
			else
			{
				// PART 1: GET AND VALIDATE CONV SPEC
				int suppress = 0;
				ll field_width = -1;
				char length_modifier[3] = "\0\0";
				specifiers spec = NONE;
				
				int set_inv = 0;
				char set_elems[CHAR_MAX];
				for (int i = 0; i < CHAR_MAX; ++i)
				{
					set_elems[i] = 0;
				}
				
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
				else if(*frm_ptr == 'R' || *frm_ptr == 'Z' || *frm_ptr == 'C')
				{
					char frm_ch = *frm_ptr;
					++frm_ptr;
					if (frm_ch == 'R' && *frm_ptr == 'o')
					{
						spec = ROMAN;
						++frm_ptr;
					}
					else if (frm_ch == 'Z' && *frm_ptr == 'r')
					{
						spec = ZECKENDORF;
						++frm_ptr;
					}
					else if (frm_ch == 'C' && (*frm_ptr == 'v' || *frm_ptr == 'V'))
					{
						spec = *frm_ptr == 'v' ? BASED_INT_L : BASED_INT_U;
						++frm_ptr;
					}
				}
				
				if (spec == SET)
				{
					int set_settings = 1;
					int prev_char = ' ';
					int interval_flag = 0;
					if (*frm_ptr == '^')
					{
						set_inv = 1;
						++frm_ptr;
					}
					while (set_settings || *frm_ptr != ']')
					{
						if (*frm_ptr == '\0')
						{
							ungetc(ch, src_file);
							va_end(arg);
							return read_cnt;
						}
						else if (set_settings && *frm_ptr == '-')
						{
							set_elems[(int) *frm_ptr] = 1;
						}
						else if (set_settings && *frm_ptr == ']')
						{
							set_elems[(int) *frm_ptr] = 1;
						}
						else if (*frm_ptr == '-')
						{
							interval_flag = 1;
						}
						else
						{
							if (interval_flag)
							{
								while(prev_char != *frm_ptr)
								{
									set_elems[(int) prev_char] = 1;
									++prev_char;
								}
							}
							set_elems[(int) *frm_ptr] = 1;
							set_settings = 0;
						}
						prev_char = *frm_ptr == '-' ? prev_char : *frm_ptr;
						++frm_ptr;
					}
					++frm_ptr;
				}
				
				// VALIDATE CONVERSION SPECIFICATION IN GENERAL
				if (spec == NONE || !is_spec_combination_valid(length_modifier, spec) || field_width == 0)
				{
					va_end(arg);
					ungetc(ch, src_file);
					return read_cnt;
				}
				
				// PART 2: WRITE DATA
				void* write_ptr = NULL;
				if (!suppress)
				{
					write_ptr = va_arg(arg, void*);
					if (write_ptr == NULL)
					{
						ungetc(ch, src_file);
						va_end(arg);
						return read_cnt ? read_cnt : -1;
					}
				}
				// SKIP SEPARATORS
				while (spec != CHAR && spec != CHAR_N && spec != SET && is_separator(ch))
				{
					ch = getc(src_file);
					++counter;
				}
				// HANDLE END OF FILE
				if (ch == EOF && spec != CHAR_N)
				{
					va_end(arg);
					return read_cnt;
				}
				int error_flag = 0;
				switch(spec)
				{
					case CHAR:
					{
						if (!suppress)
						{
							char* ch_ptr = (char*) write_ptr;
							*ch_ptr = ch;
						}
						ch = getc(src_file);
						++counter;
						break;
					}
					case STRING:
					{
						char* str_ptr = (char*) write_ptr;
						ull i = 0;
						error_flag = 1;
						while (ch && !is_separator(ch) && (field_width == -1 || i < field_width))
						{
							if (!suppress)
							{
								str_ptr[i] = ch;
							}
							error_flag = 0;
							ch = getc(src_file);
							++counter;
							++i;
						}
						if (!suppress)
						{
							str_ptr[i] = '\0';
						}
						break;
					}
					case SET:
					{
						char* str_ptr = (char*) write_ptr;
						ull i = 0;
						error_flag = 1;
						while ((set_elems[(int) ch] ^ set_inv) && (field_width == -1 || i < field_width))
						{
							if (!suppress)
							{
								str_ptr[i] = ch;
							}
							error_flag = 0;
							ch = getc(src_file);
							++counter;
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
						ull local_counter = 0;
						int base = 0;
						ll num = 0;
						ungetc(ch, src_file);
						if (fread_ll(src_file, base, field_width, ANY, &num, &local_counter, &ch) == INVALID_INPUT)
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
						counter += local_counter;
						break;
					}
					case I_INT:
					{
						ull local_counter = 0;
						int base = 0;
						ll num = 0;
						ungetc(ch, src_file);
						if (fread_ll(src_file, base, field_width, ANY, &num, &local_counter, &ch) == INVALID_INPUT)
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
						counter += local_counter;
						break;
					}
					case UINT:
					case OCT_UINT:
					case HEX_UINT:
					{
						ull local_counter = 0;
						int base = spec == UINT ? 10 : (spec == OCT_UINT ? 8 : 16);
						ll num = 0;
						ungetc(ch, src_file);
						if (fread_ll(src_file, base, field_width, ANY, &num, &local_counter, &ch) == INVALID_INPUT)
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
						counter += local_counter;
						break;
					}
					case CHAR_N:
					{
						if (!suppress)
						{
							if (!strcmp(length_modifier, "hh"))
							{
								unsigned char* ch_ptr = (unsigned char*) write_ptr;
								*ch_ptr = UCHAR_MAX & counter;
							}
							else if (!strcmp(length_modifier, "h"))
							{
								unsigned short* sh_ptr = (unsigned short*) write_ptr;
								*sh_ptr = USHRT_MAX & counter;
							}
							else if (!strcmp(length_modifier, ""))
							{
								unsigned int* int_ptr = (unsigned int*) write_ptr;
								*int_ptr = UINT_MAX & counter;
							}
							else if (!strcmp(length_modifier, "l"))
							{
								unsigned long* l_ptr = (unsigned long*) write_ptr;
								*l_ptr = ULONG_MAX & counter;
							}
							else if (!strcmp(length_modifier, "ll"))
							{
								unsigned long long* ll_ptr = (unsigned long long*) write_ptr;
								*ll_ptr = ULLONG_MAX & counter;
							}
						}
						break;
					}
					case FLOAT:
					{
						ull local_counter = 0;
						double num = 0;
						ungetc(ch, src_file);
						if (fread_lf(src_file, field_width, &num, &local_counter, &ch) == INVALID_INPUT)
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
						}
						counter += local_counter;
						break;
					}
					case PTR:
					{
						ull local_counter = 0;
						int base = 16;
						ll num = 0;
						ungetc(ch, src_file);
						if (fread_ll(src_file, base, field_width, ANY, &num, &local_counter, &ch) == INVALID_INPUT)
						{
							error_flag = 1;
						}
						if (!suppress)
						{
							void** void_ptr = (void**) write_ptr;
							*void_ptr = (void*) num;
						}
						counter += local_counter;
						break;
					}
					case ROMAN:
					{
						ull local_counter = 0;
						int num = 0;
						ungetc(ch, src_file);
						if (fread_ro(src_file, field_width, &num, &local_counter, &ch) == INVALID_INPUT)
						{
							error_flag = 1;
						}
						if (!suppress)
						{
							int* void_ptr = (int*) write_ptr;
							*void_ptr = num;
						}
						counter += local_counter;
						break;
					}
					case ZECKENDORF:
					{
						ull local_counter = 0;
						unsigned int num = 0;
						ungetc(ch, src_file);
						if (fread_zr(src_file, field_width, &num, &local_counter, &ch) == INVALID_INPUT)
						{
							error_flag = 1;
						}
						if (!suppress)
						{
							unsigned int* void_ptr = (unsigned int*) write_ptr;
							*void_ptr = num;
						}
						counter += local_counter;
						break;
					}
					case BASED_INT_L:
					case BASED_INT_U:
					{
						ull local_counter;
						int base = 10;
						if (!suppress)
						{
							base = va_arg(arg, int);
						}
						if (base < 2 || base > 36)
						{
							base = 10;
						}
						
						letter_cases l_case = spec == BASED_INT_L ? LOWER : UPPER;
						
						ll num = 0;
						ungetc(ch, src_file);
						if (fread_ll(src_file, base, field_width, l_case, &num, &local_counter, &ch) == INVALID_INPUT)
						{
							error_flag = 1;
						}
						if (!suppress)
						{
							int* void_ptr = (int*) write_ptr;
							*void_ptr = num;
						}
						counter += local_counter;
						break;
					}
					default:
						error_flag = 1;
						break;
				}
				
				if (error_flag)
				{
					ungetc(ch, src_file);
					va_end(arg);
					return read_cnt;
				}
				if (!suppress && spec != CHAR_N)
				{
					++read_cnt;
				}
			}
		}
	}
	
	ungetc(ch, src_file);
	va_end(arg);
	return read_cnt;
}

int main(int argc, char** argv)
{
	FILE* input = fopen("lab2/t6_test", "r");
	if (input == NULL)
	{
		print_error(FILE_OPENING_ERROR);
		return FILE_OPENING_ERROR;
	}
	
	// BASE SPECIFIERS
	char ch;
	char str[100] = "NO DATA", str2[100] = "NO DATA";
	int d, octd, hexd;
	unsigned u, o, x, xx, n;
	
	int cnt = overfscanf(input, "%c %s\n", &ch, str);
	cnt += overfscanf(input, "%d %u %o %x %*s %X%n\n", &d, &u, &o, &x, &xx, &n);
	printf("1: cnt=%d\n", cnt);
	printf("%c %s %d %d %d %d %d %d\n", ch, str, d, u, o, x, xx, n);
	
	cnt = overfscanf(input, "%i%i%i\n", &d, &octd, &hexd);
	printf("2: cnt=%d\n%d %d %d\n", cnt, d, octd, hexd);
	
	signed char q;
	cnt = overfscanf(input, "%hhd\n", &q);
	printf("3: cnt=%d\n%d\n", cnt, q);
	
	cnt = overfscanf(input, "%3s%2d%s\n", str, &d, str2);
	printf("4: cnt=%d\n%s %d %s\n", cnt, str, d, str2);
	
	cnt = overfscanf(input, "%[a-d]%5[^a-d] %*[^\n]\n", str, str2);
	printf("5: cnt=%d\n%s %s\n", cnt, str, str2);
	
	cnt = overfscanf(input, "%s|%s\n", str, str2);
	printf("6: cnt=%d\n%s %s\n", cnt, str, str2);
	
	cnt = overfscanf(input, "%lls\n", str);
	printf("7: cnt=%d\n%s\n", cnt, str);
	
	double a = 0, b = 0, c = 0;
	float fa = 0;
	cnt = overfscanf(input, "%lf %lf %lf\n", &a, &b, &c);
	printf("8: cnt=%d\n%lf %lf %lf\n", cnt, a, b, c);
	cnt = overfscanf(input, "%f %lf\n", &fa, &a);
	printf("9 cnt=%d\n%lf %lf\n", cnt, fa, a);
	
	cnt = overfscanf(input, "1%%2%s\n", str);
	printf("10: cnt=%d\n%s\n", cnt, str);
	
	// SPECIFIC SPECIFIERS
	int ro1, ro2;
	cnt = overfscanf(input, "%2Ro%*Ro%Ro\n", &ro1, &ro2);
	printf("11: cnt=%d\n%d %d\n", cnt, ro1, ro2);
	
	unsigned int zr1, zr2;
	cnt = overfscanf(input, "%Zr%5Zr %*[^\n]\n", &zr1, &zr2);
	printf("12: cnt=%d\n%d %d\n", cnt, zr1, zr2);
	
	int cvl1, cvl2;
	cnt = overfscanf(input, "%Cv%Cv %*[^\n]\n", &cvl1, 3, &cvl2, 16);
	printf("13: cnt=%d\n%d %d\n", cnt, cvl1, cvl2);
	
	int cvu1, cvu2;
	cnt = overfscanf(input, "%CV%CV %*[^\n]\n", &cvu1, 4, &cvu2, 32);
	printf("14: cnt=%d\n%d %d\n", cnt, cvu1, cvu2);
	
	fclose(input);
}