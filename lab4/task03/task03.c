#include "utility.h"
#include "trie.h"
#include "polynomial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum command_code
{
	INVALID_CMD,
	ADD,
	SUB,
	MULT,
	EVAL,
	PART_DERIV,
	GRAD,
	PART_ANTIDERIV,
} command_code;

status_code parse_cmd_name(const char* cmd_name, command_code* cmd_code);

int main(int argc, char** argv)
{
	status_code err_code = OK;
	
	double eps = 1e-9;
	
	FILE* input = fopen("input", "r");
	if (input == NULL)
	{
		print_error(FILE_OPENING_ERROR);
		return FILE_OPENING_ERROR;
	}
	
	Polynomial summator;
	polynomial_set_null(&summator);
	while (!err_code)
	{
		char* cmd = NULL;
		command_code cmd_code = INVALID_CMD;
		char* cmd_name = NULL;
		ull arg_cnt = 0;
		char** args = NULL;
		err_code = err_code ? err_code : fread_cmd(input, &cmd);
		err_code = err_code ? err_code : parse_cmd(cmd, &cmd_name, &arg_cnt, &args);
		err_code = err_code ? err_code : parse_cmd_name(cmd_name, &cmd_code);
		free(cmd);
		free(cmd_name);
		cmd = cmd_name = NULL;
		
		Polynomial arg_1, arg_2;
		char* var_name = NULL;
		char* dict_str = NULL;
		polynomial_set_null(&arg_1);
		polynomial_set_null(&arg_2);
		switch (cmd_code)
		{
			case ADD:
			case SUB:
			case MULT:
			{
				if (arg_cnt == 2)
				{
					err_code = err_code ? err_code : polynomial_construct(&arg_1, eps, args[0]);
					err_code = err_code ? err_code : polynomial_construct(&arg_2, eps, args[1]);
					polynomial_destruct(&summator);
				}
				else if (arg_cnt == 1)
				{
					err_code = err_code ? err_code : polynomial_move(&arg_1, &summator);
					err_code = err_code ? err_code : polynomial_construct(&arg_2, eps, args[0]);
				}
				else
				{
					err_code = err_code ? err_code : FILE_INVALID_CONTENT;
				}
				break;
			}
			case EVAL:
			{
				if (arg_cnt == 2)
				{
					err_code = err_code ? err_code : polynomial_construct(&arg_1, eps, args[0]);
					dict_str = args[1];
				}
				else if (arg_cnt == 1)
				{
					err_code = err_code ? err_code : polynomial_copy(&arg_1, summator);
					dict_str = args[0];
				}
				else
				{
					err_code = err_code ? err_code : FILE_INVALID_CONTENT;
				}
				break;
			}
			case PART_DERIV:
			case PART_ANTIDERIV:
			{
				if (arg_cnt == 2)
				{
					err_code = err_code ? err_code : polynomial_construct(&arg_1, eps, args[0]);
					var_name = args[1];
					polynomial_destruct(&summator);
				}
				else if (arg_cnt == 1)
				{
					err_code = err_code ? err_code : polynomial_move(&arg_1, &summator);
					var_name = args[0];
				}
				else
				{
					err_code = err_code ? err_code : FILE_INVALID_CONTENT;
				}
				break;
			}
			case GRAD:
			{
				if (arg_cnt == 1)
				{
					err_code = err_code ? err_code : polynomial_construct(&arg_1, eps, args[0]);
					polynomial_destruct(&summator);
				}
				else if (arg_cnt == 0)
				{
					err_code = err_code ? err_code : polynomial_copy(&arg_1, summator);
				}
				else
				{
					err_code = err_code ? err_code : FILE_INVALID_CONTENT;
				}
				break;
			}
			default:
				err_code = err_code ? err_code : FILE_INVALID_CONTENT;
				break;
		}
		
		
		ull dict_cnt = 0;
		pair_str_double* dict = NULL;
		double value = 0;
		switch (cmd_code)
		{
			case ADD:
				err_code = err_code ? err_code : polynomial_add(arg_1, arg_2, eps, &summator);
				err_code = err_code ? err_code : polynomial_print(summator, 1);
				break;
			case SUB:
				err_code = err_code ? err_code : polynomial_sub(arg_1, arg_2, eps, &summator);
				err_code = err_code ? err_code : polynomial_print(summator, 1);
				break;
			case MULT:
				err_code = err_code ? err_code : polynomial_mult(arg_1, arg_2, eps, &summator);
				err_code = err_code ? err_code : polynomial_print(summator, 1);
				break;
			case EVAL:
				err_code = err_code ? err_code : parse_dict_str_double(dict_str, &dict_cnt, &dict);
				err_code = err_code ? err_code : polynomial_eval(arg_1, dict_cnt, dict, &value);
				if (!err_code)
				{
					printf("Value of summator at %s is %lf\n", dict_str, value);
				}
				break;
			case PART_DERIV:
				err_code = err_code ? err_code : polynomial_part_deriv(arg_1, var_name, eps, &summator);
				err_code = err_code ? err_code : polynomial_print(summator, 1);
				break;
			case GRAD:
				err_code = err_code ? err_code : polynomial_print_grad(arg_1, eps, 1);
				break;
			case PART_ANTIDERIV:
				err_code = err_code ? err_code : polynomial_part_antideriv(arg_1, var_name, eps, &summator);
				err_code = err_code ? err_code : polynomial_print(summator, 1);
				break;
			default:
				break;
		}
		
		polynomial_destruct(&arg_1);
		polynomial_destruct(&arg_2);
		for (ull i = 0; i < dict_cnt; ++i)
		{
			free(dict[i].str);
		}
		free(dict);
		for (ull i = 0; i < arg_cnt; ++i)
		{
			free(args[i]);
		}
		free(args);
	}
	err_code = err_code != FILE_END ? err_code : OK;
	err_code = err_code != INVALID_INPUT ? err_code : FILE_INVALID_CONTENT;
	fclose(input);
	polynomial_destruct(&summator);
	print_error(err_code);
	return err_code;
}

status_code parse_cmd_name(const char* cmd_name, command_code* cmd_code)
{
	if (cmd_name == NULL || cmd_code == NULL)
	{
		return INVALID_ARG;
	}
	if (!strcmp(cmd_name, "Add"))
	{
		*cmd_code = ADD;
	}
	else if (!strcmp(cmd_name, "Sub"))
	{
		*cmd_code = SUB;
	}
	else if (!strcmp(cmd_name, "Mult"))
	{
		*cmd_code = MULT;
	}
	else if (!strcmp(cmd_name, "Eval"))
	{
		*cmd_code = EVAL;
	}
	else if (!strcmp(cmd_name, "PartDeriv"))
	{
		*cmd_code = PART_DERIV;
	}
	else if (!strcmp(cmd_name, "Grad"))
	{
		*cmd_code = GRAD;
	}
	else if (!strcmp(cmd_name, "PartAntideriv"))
	{
		*cmd_code = PART_ANTIDERIV;
	}
	else
	{
		*cmd_code = INVALID_CMD;
	}
	return OK;
}
