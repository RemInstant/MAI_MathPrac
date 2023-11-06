#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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

ll increment(ll* a)
{
	if (a == NULL)
	{
		return 0;
	}
	ll mask = 1;
	ll c = 1; // carry digit
	while (*a & mask)
	{
		*a ^= c;
		c <<= 1;
		mask <<= 1;
	}
	*a |= c;
	return *a;
}

ll decrement(ll* a)
{
	if (a == NULL)
	{
		return 0;
	}
	ll mask = 1;
	ll c = 1; // carry digit
	while (~(*a) & mask)
	{
		*a ^= c;
		c <<= 1;
		mask <<= 1;
	}
	*a ^= c;
	return *a;
}

ll negative(ll a)
{
	a = ~a;
	return increment(&a);
}

ll plus(ll a, ll b)
{
	ll res = 0;
	ll mask = 1;
	ll c = 0; // carry digit
	while (mask)
	{
		res |= (a ^ b ^ c) & mask;
		c = ((a & b) | (a & c) | (b & c)) & mask;
		c <<= 1;
		mask <<= 1;
	}
	return res;
}

ll minus(ll a, ll b)
{
	return plus(a, negative(b));
}

ll product(ll a, ll b)
{
	ll res = 0;
	while(b)
	{
		if(b & 1)
		{
			res = plus(res, a);
		}
		b >>= 1;
		a <<= 1;
	}
	return res;
}

int ctoi(char ch)
{
	if (isdigit(ch))
	{
		return minus(ch, '0');
	}
	if (isalpha(ch))
	{
		return plus(minus(toupper(ch), 'A'), 10);
	}
	return negative(1);
}

char itoc(int integer)
{
	if (integer < 0)
	{
		return negative(1);
	}
	if (integer < 10)
	{
		return plus('0', integer);
	}
	if (integer < 36)
	{
		return minus(plus('A', integer), 10);
	}
	return negative(1);
}

status_codes convert_to_base_2r(ll num, int r, char* res)
{
	if (res == NULL || r < 1 || r > 5)
	{
		return INVALID_INPUT;
	}
	
	res[64] = '\0';
	for (ll i = 0; i < 64; increment(&i))
	{
		res[i] = '0';
	}
	
	int mask = minus(1 << r, 1);
	for (ll i = 0; product(r, i) < 64; increment(&i))
	{
		res[minus(63, i)] = itoc((num >> product(r, i)) & mask);
	}
	
	ll zero_cnt = 0;
	while (res[zero_cnt] == '0' && zero_cnt < 63)
	{
		increment(&zero_cnt);
	}
	if (zero_cnt != 0)
	{	
		for (ll i = zero_cnt; res[i]; increment(&i))
		{
			res[minus(i, zero_cnt)] = res[i];
		}
		res[minus(64, zero_cnt)] = '\0';
	}
	return OK;
}

int main(int argc, char** argv)
{
	char res[65];
	status_codes code = convert_to_base_2r(LLONG_MAX, 4, res);
	if (code != OK)
	{
		print_error(code);
		return code;
	}
	printf("%s\n", res);
}