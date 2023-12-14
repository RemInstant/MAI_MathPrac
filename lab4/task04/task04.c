#include "utility.h"
#include "boolean_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

status_code validate_argv(int argc, char** argv);
status_code parse_cmd(const char* src, b_op* op_code, ull* arg_cnt, char* arg_1, char* arg_2, char* arg_3);

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: cmd_path <input>\n");
		return OK;
	}
	
	status_code err_code = validate_argv(argc, argv);
	status_code file_code = OK;
	if (err_code)
	{
		print_error(err_code, 1);
		return err_code;
	}
	
	FILE* input = fopen(argv[1], "r");
	//FILE* input = fopen("input", "r");
	if (input == NULL)
	{
		print_error(FILE_OPENING_ERROR, 1);
		return FILE_OPENING_ERROR;
	}
	
	FILE* output = argc == 4 ? fopen(argv[3], "w") : stdout;
	//FILE* output = fopen("output", "w");
	//FILE* output = stdout;
	if (output == NULL)
	{
		fclose(input);
		print_error(FILE_OPENING_ERROR, 1);
		return FILE_OPENING_ERROR;
	}
	
	char init_flags[26];
	char vars[26][65];
	char* cmd = NULL;
	memset(init_flags, 0, sizeof(char) * 26);

	while (!err_code)
	{
		boolean_operation op_code = INVALID_OP;
		ull arg_cnt = 0;
		char arg_1, arg_2, arg_3;
		err_code = err_code ? err_code : fread_cmd(input, &cmd); // FIX IT
		file_code = err_code ? OK : parse_cmd(cmd, &op_code, &arg_cnt, &arg_1, &arg_2, &arg_3);
		err_code = file_code == BAD_ALLOC ? BAD_ALLOC : err_code;
		err_code = err_code ? err_code : (file_code ? FILE_INVALID_CONTENT : OK);
		
		char tmp[65];
		int ind_1 = tolower(arg_1) - 'a';
		int ind_2 = tolower(arg_2) - 'a';
		int ind_3 = tolower(arg_3) - 'a';
		if (!err_code)
		{
			switch (op_code)
			{
				case CONJUCTION:
				case DISJUCTION:
				case IMPLICATION:
				case REV_IMPLICATION:
				case COIMPLICATION:
				case EXCLUSIVE:
				case EQUIVALENCE:
				case SHEFFER_STROKE:
				case WEBB_FUNCTION:
				{
					file_code = ((init_flags[ind_2] && init_flags[ind_3]) ? OK : UNINITIALIZED_USAGE);
					file_code = file_code ? file_code : bool_vector_calc_binary_op(vars[ind_2], vars[ind_3], op_code, tmp);
					break;
				}
				case INVERSION:
				{
					file_code = (init_flags[ind_2] ? OK : UNINITIALIZED_USAGE);
					file_code = file_code ? file_code : bool_vector_inverse(vars[ind_2], tmp);
					break;
				}
				case READ_OP:
				{
					err_code = bool_vector_read(tmp, (int) arg_2);
					if (!err_code)
					{
						printf("\n");
					}
					break;
				}
				case WRITE_OP:
				{
					file_code = file_code ? file_code : (init_flags[ind_1] ? OK : UNINITIALIZED_USAGE);
					if (!file_code)
					{
						printf("Value of %c in %d-base: ", toupper(arg_1), (int) arg_2);
						err_code = bool_vector_write(vars[ind_1], (int) arg_2, 2);
					}
					break;
				}
				default:
					file_code = INVALID_INPUT;
					break;
			}
		}
		if (!err_code && file_code)
		{
			err_code = file_code == BAD_ALLOC ? BAD_ALLOC : FILE_INVALID_CONTENT;
		}
		
		if (!err_code)
		{
			fprintf(output, "TRACE: cmd: \"%s\"\n", cmd);
			switch (op_code)
			{
				case CONJUCTION:
				case DISJUCTION:
				case IMPLICATION:
				case REV_IMPLICATION:
				case COIMPLICATION:
				case EXCLUSIVE:
				case EQUIVALENCE:
				case SHEFFER_STROKE:
				case WEBB_FUNCTION:
				{
					fprintf(output, "Value of %c: %s\n", toupper(arg_2), vars[ind_2]);
					fprintf(output, "Value of %c: %s\n", toupper(arg_3), vars[ind_3]);
					fprintf(output, "Prev value of %c: %s\n", toupper(arg_1), init_flags[ind_1] ? vars[ind_1] : "unitialized");
					fprintf(output, "New value of %c: %s\n", toupper(arg_1), tmp);
					strcpy(vars[ind_1], tmp);
					init_flags[ind_1] = 1;
					break;
				}
				case INVERSION:
				{
					fprintf(output, "Value of %c: %s\n", toupper(arg_2), vars[ind_2]);
					fprintf(output, "Prev value of %c: %s\n", toupper(arg_1), init_flags[ind_1] ? vars[ind_1] : "unitialized");
					fprintf(output, "New value of %c: %s\n", toupper(arg_1), tmp);
					strcpy(vars[ind_1], tmp);
					init_flags[ind_1] = 1;
					break;
				}
				case READ_OP:
				{
					fprintf(output, "Prev value of %c: %s\n", toupper(arg_1), init_flags[ind_1] ? vars[ind_1] : "unitialized");
					fprintf(output, "New value of %c: %s\n", toupper(arg_1), tmp);
					strcpy(vars[ind_1], tmp);
					init_flags[ind_1] = 1;
					break;
				}
				case WRITE_OP:
				{
					fprintf(output, "Value of %c: %s\n", toupper(arg_1), vars[ind_1]);
					break;
				}
				default:
					break;
			}
			fprintf(output, "\n");
		}
		if (!err_code)
		{
			free(cmd);
			cmd = NULL;
		}
	}
	err_code = err_code == FILE_END ? OK : err_code;
	
	if (file_code == INVALID_ARG || file_code == INVALID_BASE || file_code == INVALID_CMD ||
			file_code == INVALID_INPUT || file_code == UNINITIALIZED_USAGE)
	{
		fprint_error(output, FILE_INVALID_CONTENT, 0);
		fprintf(output, ": \"%s\" - ", cmd);
		fprint_error(output, file_code, 1);
	}
	else
	{
		fprint_error(output, err_code, 1);
	}
	free(cmd);
	fclose(input);
	fclose(output);
	return err_code;
}

status_code validate_argv(int argc, char** argv)
{
	if (argv == NULL)
	{
		return NULL_ARG;
	}
	if (argc == 2)
	{
		return OK;
	}
	if (argc == 4)
	{
		if (strcmp(argv[2], "/trace"))
		{
			return INVALID_FLAG;
		}
		return OK;
	}
	return INVALID_INPUT;
}

status_code parse_cmd(const char* src, b_op* cmd_code, ull* arg_cnt, char* arg_1, char* arg_2, char* arg_3)
{
	if (src == NULL || cmd_code == NULL || arg_cnt == NULL || arg_1 == NULL || arg_2 == NULL || arg_3 == NULL)
	{
		return NULL_ARG;
	}
	if (src[0] == '\0')
	{
		*cmd_code = INVALID_OP;
		*arg_cnt = 0;
		return INVALID_CMD;
	}
	*cmd_code = INVALID_OP;
	char* low_src = NULL;
	char* good_src = NULL;
	status_code err_code = tolowern(src, &low_src);
	err_code = err_code ? err_code : erase_delims(low_src, " \t\r\n", &good_src);
	free(low_src);
	const char* ptr = good_src;
	char* str_cmd = NULL;
	char* str_arg_1 = NULL;
	char* str_arg_2 = NULL;
	char* str_arg_3 = NULL;
	
	err_code = err_code ? err_code : sread_until(ptr, "(:", 0, &ptr, &str_cmd);
	err_code = err_code ? err_code : (str_cmd[0] ? OK : INVALID_INPUT);
	if (!err_code && str_cmd[1] == '\0')
	{
		str_arg_1 = str_cmd;
		str_cmd = NULL;
		err_code = err_code ? err_code : (*(ptr++) == ':' ? OK : INVALID_INPUT);
		err_code = err_code ? err_code : (*(ptr++) == '=' ? OK : INVALID_INPUT);
		if (*ptr == '\\')
		{
			*cmd_code = INVERSION;
			++ptr;
			err_code = err_code ? err_code : sread_until(ptr, ";", 0, &ptr, &str_arg_2);
			err_code = err_code ? err_code : (str_arg_2[0] ? OK : INVALID_INPUT);
		}
		else
		{
			err_code = err_code ? err_code : sread_until(ptr, "+&-<~?!", 0, &ptr, &str_arg_2);
			err_code = err_code ? err_code : (str_arg_2[0] ? OK : INVALID_INPUT);
			err_code = err_code ? err_code : sread_until(ptr, "abcdefghijklmnopqrstuvwxyz", 0, &ptr, &str_cmd);
			err_code = err_code ? err_code : (str_cmd[0] ? OK : INVALID_INPUT);
			err_code = err_code ? err_code : sread_until(ptr, ";", 0, &ptr, &str_arg_3);
			err_code = err_code ? err_code : (str_arg_3[0] ? OK : INVALID_INPUT);
		}
	}
	else if (!err_code)
	{
		err_code = err_code ? err_code : (*(ptr++) == '(' ? OK : INVALID_INPUT);
		err_code = err_code ? err_code : sread_until(ptr, ",", 0, &ptr, &str_arg_1);
		err_code = err_code ? err_code : (str_arg_1[0] ? OK : INVALID_INPUT);
		err_code = err_code ? err_code : (*(ptr++) == ',' ? OK : INVALID_INPUT);
		err_code = err_code ? err_code : sread_until(ptr, ")", 0, &ptr, &str_arg_2);
		err_code = err_code ? err_code : (str_arg_2[0] ? OK : INVALID_INPUT);
		err_code = err_code ? err_code : (*(ptr++) == ')' ? OK : INVALID_INPUT);
	}
	err_code = err_code ? err_code : (*(ptr++) == ';' ? OK : INVALID_INPUT);
	err_code = err_code ? err_code : (*(ptr++) == '\0' ? OK : INVALID_INPUT);
	
	if (!err_code && str_cmd != NULL && !strcmp(str_cmd, "+"))
	{
		*cmd_code = DISJUCTION;
	}
	else if (!err_code && str_cmd != NULL && !strcmp(str_cmd, "&"))
	{
		*cmd_code = CONJUCTION;
	}
	else if (!err_code && str_cmd != NULL && !strcmp(str_cmd, "~"))
	{
		*cmd_code = EQUIVALENCE;
	}
	else if (!err_code && str_cmd != NULL && !strcmp(str_cmd, "?"))
	{
		*cmd_code = SHEFFER_STROKE;
	}
	else if (!err_code && str_cmd != NULL && !strcmp(str_cmd, "!"))
	{
		*cmd_code = WEBB_FUNCTION;
	}
	else if (!err_code && str_cmd != NULL && !strcmp(str_cmd, "->"))
	{
		*cmd_code = IMPLICATION;
	}
	else if (!err_code && str_cmd != NULL && !strcmp(str_cmd, "<-"))
	{
		*cmd_code = REV_IMPLICATION;
	}
	else if (!err_code && str_cmd != NULL && !strcmp(str_cmd, "<>"))
	{
		*cmd_code = EXCLUSIVE;
	}
	else if (!err_code && str_cmd != NULL && !strcmp(str_cmd, "+>"))
	{
		*cmd_code = COIMPLICATION;
	}
	else if (!err_code && str_cmd != NULL && !strcmp(str_cmd, "read"))
	{
		*cmd_code = READ_OP;
	}
	else if (!err_code && str_cmd != NULL && !strcmp(str_cmd, "write"))
	{
		*cmd_code = WRITE_OP;
	}
	else if (*cmd_code != INVERSION)
	{
		err_code = err_code ? err_code : INVALID_CMD;
	}
	
	if (!err_code && (*cmd_code == READ_OP || *cmd_code == WRITE_OP))
	{
		ull number = 0;
		err_code = err_code ? err_code : (str_arg_1[1] == '\0' ? OK : INVALID_ARG);
		err_code = err_code ? err_code : (!parse_ullong(str_arg_2, 10, &number) ? OK : INVALID_ARG);
		err_code = err_code ? err_code : ((number >= 2 && number <= 36) ? OK : INVALID_BASE);
		*arg_cnt = 2;
		*arg_1 = str_arg_1[0];
		*arg_2 = (char) number;
	}
	else if (!err_code && *cmd_code == INVERSION)
	{
		err_code = err_code ? err_code : (str_arg_1[1] == '\0' ? OK : INVALID_ARG);
		err_code = err_code ? err_code : (str_arg_2[1] == '\0' ? OK : INVALID_ARG);
		*arg_cnt = 2;
		*arg_1 = str_arg_1[0];
		*arg_2 = str_arg_2[0];
	}
	else if (!err_code)
	{
		err_code = err_code ? err_code : (str_arg_1[1] == '\0' ? OK : INVALID_ARG);
		err_code = err_code ? err_code : (str_arg_2[1] == '\0' ? OK : INVALID_ARG);
		err_code = err_code ? err_code : (str_arg_3[1] == '\0' ? OK : INVALID_ARG);
		*arg_cnt = 3;
		*arg_1 = str_arg_1[0];
		*arg_2 = str_arg_2[0];
		*arg_3 = str_arg_3[0];
	}
	free(good_src);
	free(str_cmd);
	free(str_arg_1);
	free(str_arg_2);
	free(str_arg_3);
	return err_code;
}