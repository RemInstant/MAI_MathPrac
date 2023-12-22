#include "utility.h"

typedef enum boolean_operation
{
	INVALID_OP,
	READ_OP,
	WRITE_OP,
	CONJUCTION,
	DISJUCTION,
	INVERSION,
	IMPLICATION,
	REV_IMPLICATION,
	COIMPLICATION,
	EXCLUSIVE,
	EQUIVALENCE,
	SHEFFER_STROKE,
	WEBB_FUNCTION // a.k.a. Peirce arrow
} boolean_operation, b_op;

status_code bool_vector_read(char bvec[65], int base);
status_code bool_vector_write(const char bvec[65], int base, int nl_cnt);
status_code bool_vector_inverse(char bvec[65], char bvec_res[65]);
status_code bool_vector_calc_binary_op(char bvec_1[65], char bvec_2[65], b_op op, char bvec_res[65]);