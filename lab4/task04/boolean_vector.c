#include "boolean_vector.h"
#include <stdlib.h>
#include <string.h>

status_code bool_vector_read(char bvec[65], int base)
{
	if (bvec == NULL)
	{
		return NULL_ARG;
	}
	if (base < 2 || base > 36)
	{
		return INVALID_BASE;
	}
	char* str = NULL;
	ull number = 0;
	printf("Enter boolean vector value in %d-base: ", base);
	status_code err_code = read_line(&str);
	err_code = err_code ? err_code : parse_ullong(str, base, &number);
	err_code = err_code ? err_code : convert_ullong(number, 2, bvec);
	free(str);
	return err_code;
}

status_code bool_vector_write(const char bvec[65], int base, int nl_cnt)
{
	if (bvec == NULL)
	{
		return NULL_ARG;
	}
	if (base < 2 || base > 36)
	{
		return INVALID_BASE;
	}
	char str[65];
	ull number = 0;
	status_code err_code = parse_ullong(bvec, 2, &number);
	err_code = err_code ? err_code : convert_ullong(number, base, str);
	if (!err_code)
	{
		printf("%s", str + strspn(str, "0"));
		for (int i = 0; i < nl_cnt; ++i)
		{
			printf("\n");
		}
	}
	return err_code;
}

status_code bool_vector_inverse(char bvec[65], char bvec_res[65])
{
	if (bvec == NULL || bvec_res == NULL)
	{
		return NULL_ARG;
	}
	for (ull i = 0; i < 64; ++i)
	{
		bvec_res[i] = !(bvec[i] - '0') + '0';
	}
	return OK;
}

status_code bool_vector_calc_binary_op(char bvec_1[65], char bvec_2[65], b_op op, char bvec_res[65])
{
	if (bvec_1 == NULL || bvec_2 == NULL || bvec_res == NULL)
	{
		return NULL_ARG;
	}
	for (ull i = 0; i < 64; ++i)
	{
		switch (op)
		{
			case CONJUCTION:
				bvec_res[i] = ((bvec_1[i] - '0') && (bvec_2[i] - '0')) + '0';
				break;
			case DISJUCTION:
				bvec_res[i] = ((bvec_1[i] - '0') || (bvec_2[i] - '0')) + '0';
				break;
			case IMPLICATION:
				bvec_res[i] = ((bvec_1[i] - '0') <= (bvec_2[i] - '0')) + '0';
				break;
			case REV_IMPLICATION:
				bvec_res[i] = ((bvec_1[i] - '0') >= (bvec_2[i] - '0')) + '0';
				break;
			case COIMPLICATION:
				bvec_res[i] = ((bvec_1[i] - '0') > (bvec_2[i] - '0')) + '0';
				break;
			case EXCLUSIVE:
				bvec_res[i] = ((bvec_1[i] - '0') ^ (bvec_2[i] - '0')) + '0';
				break;
			case EQUIVALENCE:
				bvec_res[i] = ((bvec_1[i] - '0') == (bvec_2[i] - '0')) + '0';
				break;
			case SHEFFER_STROKE:
				bvec_res[i] = !((bvec_1[i] - '0') && (bvec_2[i] - '0')) + '0';
				break;
			case WEBB_FUNCTION:
				bvec_res[i] = !((bvec_1[i] - '0') || (bvec_2[i] - '0')) + '0';
				break;
			default:
				return INVALID_INPUT;
		}
	}
	return OK;
}


