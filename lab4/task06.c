#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>

typedef long long ll;
typedef unsigned long long ull;

typedef enum
{
	OK,
	INVALID_ARG,
	INVALID_INPUT,
	INVALID_FLAG,
	INVALID_NUMBER,
	FILE_OPENING_ERROR,
	FILE_CONTENT_ERROR,
	OVERFLOW,
	UNINITIALIZED_USAGE,
	DIVISION_BY_ZERO,
	BAD_ALLOC,
	CORRUPTED_MEMORY
} status_code;

void print_error(status_code code)
{
	switch (code)
	{
		case OK:
			return;
		case INVALID_ARG:
			printf("Invalid function argument\n");
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
		case FILE_CONTENT_ERROR:
			printf("Invalid content of file\n");
			return;
		case OVERFLOW:
			printf("An overflow occurred\n");
			return;
		case UNINITIALIZED_USAGE:
			printf("Uninitialized variable was used\n");
			return;
		case DIVISION_BY_ZERO:
			printf("Division by zero occurred\n");
			return;
		case BAD_ALLOC:
			printf("Memory lack error occurred\n");
			return;
		case CORRUPTED_MEMORY:
			printf("Memory was corrupted\n");
			return;
		default:
			printf("Unexpected error occurred\n");
			return;
	}
}

typedef enum
{
	NONE,
	VAR,
	L_BRACKET,
	R_BRACKET,
	CONST,
	CONJUCTION,
	DISJUCTION,
	INVERSION,
	IMPLICATION,
	COIMPLICATION,
	EXCLUSIVE,
	EQUIVALENCE,
	SHEFFER_STROKE,
	WEBB_FUNCTION
} node_content;

typedef struct tree_node
{
	node_content content;
	char data;
	struct tree_node* left;
	struct tree_node* right;
} tree_node;

status_code tree_node_dynamic_construct(tree_node** node, node_content content, char data);
status_code tree_node_destruct(tree_node* node);

typedef struct stack_node
{
	tree_node* node;
	struct stack_node* next;
} stack_node;

typedef struct
{
	stack_node* top;
	stack_node* bottom;
	ull size;
} Stack;

status_code stack_set_null(Stack* stack);
status_code stack_construct(Stack* stack);
status_code stack_destruct(Stack* stack);
status_code stack_empty(Stack* stack, int* empty);
status_code stack_top(Stack stack, tree_node** node);
status_code stack_push(Stack* stack, tree_node* node);
status_code stack_pop(Stack* stack, tree_node** node);

typedef struct
{
	tree_node* root;
	char var_cnt;
	bool var_flag[26];  // if var_flag[i] == 1, then tree contains ('A' + i) variable
} Expression_tree;

status_code get_expr_token(const char* src, const char** end_ptr, node_content* content, char* data);
status_code expr_tree_set_null(Expression_tree* etree);
status_code expr_tree_construct(Expression_tree* etree, const char* infix);
status_code expr_tree_destruct(Expression_tree* etree);
status_code expr_tree_destruct(Expression_tree* etree);
status_code expr_tree_node_calc(const tree_node* node, const bool var_table[26], bool* res);
status_code expr_tree_calc(Expression_tree etree, const bool* var_values, bool* res);
status_code expr_tree_fprint_table(FILE* file, Expression_tree etree);

status_code fread_line(FILE* file, char** str);
status_code generate_random_str(char** str);
status_code construct_output_path(const char* input, const char* output_name, char** output);

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: cmd_path <input>\n");
		return OK;
	}
	if (argc != 2)
	{
		print_error(INVALID_INPUT);
		return INVALID_INPUT;
	}
	
	status_code err_code = OK;
	srand(time(NULL));
	char* output_name = NULL;
	char* input_path = argv[1];
	char* output_path = NULL;
	
	FILE* input = fopen(input_path, "r");
	if (input == NULL)
	{
		print_error(FILE_OPENING_ERROR);
		return FILE_OPENING_ERROR;
	}
	
	while (!err_code && (output_path == NULL || !strcmp(input_path, output_path)))
	{
		free(output_name);
		free(output_path);
		err_code = generate_random_str(&output_name);
		err_code = err_code ? err_code : construct_output_path(input_path, output_name, &output_path);
	}
	free(output_name);
	if (err_code)
	{
		fclose(input);
		print_error(err_code);
		return err_code;
	}
	
	FILE* output = fopen(output_path, "w");
	if (output == NULL)
	{
		fclose(output);
		free(output_path);
		print_error(FILE_OPENING_ERROR);
		return FILE_OPENING_ERROR;
	}
	printf("Output file path: %s\n", output_path);
	free(output_path);
	
	char* infix_expr;
	err_code = fread_line(input, &infix_expr);
	
	Expression_tree etree;
	expr_tree_set_null(&etree);
	err_code = err_code ? err_code : expr_tree_construct(&etree, infix_expr);
	err_code = err_code ? err_code : expr_tree_fprint_table(output, etree);
	
	expr_tree_destruct(&etree);
	free(infix_expr);
	fclose(input);
	fclose(output);
	if (err_code)
	{
		print_error(err_code);
	}
	return err_code;
}

status_code tree_node_dynamic_construct(tree_node** node, node_content content, char data)
{
	if (node == NULL)
	{
		return INVALID_ARG;
	}
	*node = (tree_node*) malloc(sizeof(tree_node));
	if (*node == NULL)
	{
		return BAD_ALLOC;
	}
	(*node)->content = content;
	(*node)->data = data;
	(*node)->left = (*node)->right = NULL;
	return OK;
}

status_code tree_node_destruct(tree_node* node)
{
	if (node == NULL)
	{
		return INVALID_ARG;
	}
	if (node->left != NULL)
	{
		tree_node_destruct(node->left);
		free(node->left);
	}
	if (node->right != NULL)
	{
		tree_node_destruct(node->right);
		free(node->right);
	}
	return OK;
}

status_code stack_set_null(Stack* stack)
{
	if (stack == NULL)
	{
		return INVALID_ARG;
	}
	stack->top = stack->bottom = NULL;
	stack->size = 0;
	return OK;
}

status_code stack_construct(Stack* stack)
{
	if (stack == NULL)
	{
		return INVALID_ARG;
	}
	stack_node* terminator = (stack_node*) malloc(sizeof(stack_node));
	if (terminator == NULL)
	{
		return BAD_ALLOC;
	}
	terminator->node = NULL;
	terminator->next = terminator;
	stack->top = stack->bottom = terminator;
	stack->size = 0;
	return OK;
}

status_code stack_destruct(Stack* stack)
{
	if (stack == NULL)
	{
		return INVALID_ARG;
	}
	stack_node* cur = stack->top;
	while (cur != stack->bottom)
	{
		stack_node* tmp = cur;
		cur = cur->next;
		tree_node_destruct(tmp->node);
		free(tmp->node);
		free(tmp);
	}
	free(cur);
	stack->top = stack->bottom = NULL;
	stack->size = 0;
	return OK;
}

status_code stack_empty(Stack* stack, int* empty)
{
	if (stack == NULL || empty == NULL)
	{
		return INVALID_ARG;
	}
	*empty = stack->top == stack->bottom;
	return OK;
}

status_code stack_top(Stack stack, tree_node** node)
{
	if (node == NULL)
	{
		return INVALID_ARG;
	}
	*node = stack.top->node;
	return OK;
}

status_code stack_push(Stack* stack, tree_node* tr_node)
{
	if (stack == NULL)
	{
		return INVALID_ARG;
	}
	stack_node* st_node = (stack_node*) malloc(sizeof(stack_node));
	if (st_node == NULL)
	{
		return BAD_ALLOC;
	}
	st_node->node = tr_node;
	st_node->next = stack->top;
	stack->top = st_node;
	stack->size++;
	return OK;
}

status_code stack_pop(Stack* stack, tree_node** node)
{
	if (stack == NULL)
	{
		return INVALID_ARG;
	}
	if (stack->top == stack->bottom)
	{
		return INVALID_INPUT;
	}
	if (node != NULL)
	{
		*node = stack->top->node;
	}
	stack_node* tmp = stack->top;
	stack->top = stack->top->next;
	stack->size--;
	free(tmp);
	return OK;
}

int is_operation(node_content content)
{
	switch (content)
	{
		case INVERSION:
		case WEBB_FUNCTION:
		case SHEFFER_STROKE:
		case COIMPLICATION:
		case CONJUCTION:
		case DISJUCTION:
		case IMPLICATION:
		case EXCLUSIVE:
		case EQUIVALENCE:
			return 1;
		default:
			return 0;
	}
}

int get_operation_priority(node_content operation)
{
	switch (operation)
	{
		case INVERSION:
			return 3;
		case WEBB_FUNCTION:
		case SHEFFER_STROKE:
		case COIMPLICATION:
		case CONJUCTION:
			return 2;
		case DISJUCTION:
		case IMPLICATION:
		case EXCLUSIVE:
		case EQUIVALENCE:
			return 1;
		default:
			return 0;
	}
}

status_code get_expr_token(const char* src, const char** end_ptr, node_content* content, char* data)
{
	if (src == NULL || end_ptr == NULL || content == NULL || data == NULL)
	{
		return INVALID_ARG;
	}
	status_code err_code = OK;
	if (isalpha(*src))
	{
		*content = VAR;
		*data = toupper(*src);
	}
	else if (*src == '1' || *src == '0')
	{
		*content = CONST;
		*data = *src;
	}
	else if (*src == '&')
	{
		*content = CONJUCTION;
	}
	else if (*src == '|')
	{
		*content = DISJUCTION;
	}
	else if (*src == '~')
	{
		*content = INVERSION;
	}
	else if (*src == '=')
	{
		*content = EQUIVALENCE;
	}
	else if (*src == '!')
	{
		*content = SHEFFER_STROKE;
	}
	else if (*src == '?')
	{
		*content = CONJUCTION;
	}
	else if (*src == '(')
	{
		*content = L_BRACKET;
	}
	else if (*src == ')')
	{
		*content = R_BRACKET;
	}
	else if (*src == '-')
	{
		++src;
		if (*src == '>')
		{
			*content = IMPLICATION;
		}
		else
		{
			err_code = INVALID_INPUT;
		}
	}
	else if (*src == '+')
	{
		++src;
		if (*src == '>')
		{
			*content = COIMPLICATION;
		}
		else
		{
			err_code = INVALID_INPUT;
		}
	}
	else if (*src == '<')
	{
		++src;
		if (*src == '>')
		{
			*content = EXCLUSIVE;
		}
		else
		{
			err_code = INVALID_INPUT;
		}
	}
	else
	{
		err_code = INVALID_INPUT;
	}
	if (err_code)
	{
		*end_ptr = src;
		return err_code;
	}
	++src;
	*end_ptr = src;
	return OK;
}

status_code validate_token_combination(node_content prev, node_content cur)
{
	if (is_operation(prev) && is_operation(cur) && cur != INVERSION)
	{
		return INVALID_INPUT;
	}
	if ((prev == CONST || prev == VAR) && (cur == CONST || cur == VAR))
	{
		return INVALID_INPUT;
	}
	if ((prev == L_BRACKET && cur == R_BRACKET) || (prev == R_BRACKET && cur == L_BRACKET))
	{
		return INVALID_INPUT;
	}
	if ((is_operation(prev) && cur == R_BRACKET) || (prev == L_BRACKET && is_operation(cur)))
	{
		return INVALID_INPUT;
	}
	if (((prev == CONST || prev == VAR) && cur == L_BRACKET)
			|| (prev == R_BRACKET && (cur == CONST || cur == VAR)))
	{
		return INVALID_INPUT;
	}
	return OK;
}

status_code expr_tree_set_null(Expression_tree* etree)
{
	etree->root = NULL;
	return OK;
}

status_code expr_tree_construct(Expression_tree* etree, const char* infix)
{
	if (etree == NULL || infix == NULL)
	{
		return INVALID_ARG;
	}
	
	status_code err_code = OK;
	Expression_tree etree_tmp;
	etree_tmp.var_cnt = 0;
	for (ull i = 0; i < 26; ++i)
	{
		etree_tmp.var_flag[i] = false;
	}
	
	Stack args, opers;
	expr_tree_set_null(&etree_tmp);
	stack_set_null(&args);
	stack_set_null(&opers);
	err_code = err_code ? err_code : stack_construct(&args);
	err_code = err_code ? err_code : stack_construct(&opers);
	
	node_content prev_content = NONE;
	const char* ptr = infix;
	while (!err_code && *ptr)
	{
		node_content content = NONE;
		char data = 0;
		err_code = get_expr_token(ptr, &ptr, &content, &data);
		err_code = err_code ? err_code : validate_token_combination(prev_content, content);
		// --- HANDLE VARIABLE AND CONSTANT ---
		if (!err_code && (content == VAR || content == CONST))
		{
			if (content == VAR)
			{
				if (!etree_tmp.var_flag[data - 'A'])
				{
					etree_tmp.var_cnt++;
				}
				etree_tmp.var_flag[data - 'A'] = 1;
			}
			tree_node* tnode = NULL;
			err_code = tree_node_dynamic_construct(&tnode, content, data);
			err_code = err_code ? err_code : stack_push(&args, tnode);
		}
		// --- HANDLE OPERATION ---
		else if (!err_code && is_operation(content))
		{
			tree_node* tnode = NULL;
			stack_top(opers, &tnode);
			node_content other_content = tnode != NULL ? tnode->content : NONE;
			int cur_prior = get_operation_priority(content);
			int other_prior = get_operation_priority(other_content);
			while (!err_code && other_prior >= cur_prior && content != INVERSION)
			{
				int op_arg_cnt = other_content == INVERSION ? 1 : 2;
				err_code = args.size >= op_arg_cnt ? OK : INVALID_INPUT;
				// combine operation with 1/2 args
				if (op_arg_cnt == 2)
				{
					err_code = err_code ? err_code : stack_pop(&args, &tnode->right);
				}
				err_code = err_code ? err_code : stack_pop(&args, &tnode->left);
				err_code = err_code ? err_code : stack_push(&args, tnode);
				// prepare to check next operation
				err_code = err_code ? err_code : stack_pop(&opers, NULL);
				stack_top(opers, &tnode);
				other_content = tnode != NULL ? tnode->content : NONE;
				other_prior = get_operation_priority(other_content);
			}
			err_code = err_code ? err_code : tree_node_dynamic_construct(&tnode, content, data);
			err_code = err_code ? err_code : stack_push(&opers, tnode);
		}
		// --- HANDLE LEFT BRACKET ---
		else if (!err_code && content == L_BRACKET)
		{
			tree_node* tnode = NULL;
			err_code = tree_node_dynamic_construct(&tnode, content, data);
			err_code = err_code ? err_code : stack_push(&opers, tnode);
		}
		// --- HANDLE RIGHT BRACKET ---
		else if (!err_code && content == R_BRACKET)
		{
			tree_node* tnode = NULL;
			stack_top(opers, &tnode);
			err_code = tnode != NULL ? OK : INVALID_INPUT;
			while (!err_code && tnode->content != L_BRACKET)
			{
				int op_arg_cnt = tnode->content == INVERSION ? 1 : 2;
				err_code = args.size >= op_arg_cnt ? OK : INVALID_INPUT;
				// combine operation with 1/2 args
				if (op_arg_cnt == 2)
				{
					err_code = err_code ? err_code : stack_pop(&args, &tnode->right);
				}
				err_code = err_code ? err_code : stack_pop(&args, &tnode->left);
				err_code = err_code ? err_code : stack_push(&args, tnode);
				// prepare to check next operation
				err_code = err_code ? err_code : stack_pop(&opers, NULL);
				stack_top(opers, &tnode);
				err_code = err_code ? err_code : (tnode != NULL ? OK : INVALID_INPUT);
			}
			tnode = NULL;
			err_code = err_code ? err_code : stack_pop(&opers, &tnode);
			free(tnode);
		}
		prev_content = content;
	}
	while (!err_code && opers.size > 0)
	{
		int op_arg_cnt;
		tree_node* tnode = NULL;
		err_code = stack_pop(&opers, &tnode);
		if (!err_code)
		{
			op_arg_cnt = tnode->content == INVERSION ? 1 : 2;
		}
		err_code = err_code ? err_code : (tnode->content != L_BRACKET ? OK : INVALID_INPUT);
		err_code = err_code ? err_code : (args.size >= op_arg_cnt ? OK : INVALID_INPUT);
		// combine operation with 1/2 args
		if (op_arg_cnt == 2)
		{
			err_code = err_code ? err_code : stack_pop(&args, &tnode->right);
		}
		err_code = err_code ? err_code : stack_pop(&args, &tnode->left);
		err_code = err_code ? err_code : stack_push(&args, tnode);
		if (err_code)
		{
			tree_node_destruct(tnode);
			free(tnode);
		}
	}
	if (!err_code)
	{
		err_code = args.size == 1 ? OK : INVALID_INPUT;
		tree_node* tnode = NULL;
		err_code = err_code ? err_code : stack_pop(&args, &tnode);
		etree_tmp.root = tnode;
	}
	stack_destruct(&args);
	stack_destruct(&opers);
	if (err_code)
	{
		expr_tree_destruct(&etree_tmp);
		return err_code;
	}
	*etree = etree_tmp;
	memcpy(etree->var_flag, etree_tmp.var_flag, sizeof(bool) * 26);
	return OK;
}

status_code expr_tree_destruct(Expression_tree* etree)
{
	if (etree == NULL)
	{
		return INVALID_ARG;
	}
	tree_node_destruct(etree->root);
	free(etree->root);
	return OK;
}

status_code expr_tree_node_calc(const tree_node* node, const bool var_table[26], bool* res)
{
	if (res == NULL)
	{
		return INVALID_ARG;
	}
	
	switch (node->content)
	{
		case NONE:
		case L_BRACKET:
		case R_BRACKET:
			return INVALID_INPUT;
		case VAR:
			if (node->data - 'A' < 0 || node->data - 'A' > 25)
			{
				return INVALID_INPUT;
			}
			*res = var_table[node->data - 'A'];
			return OK;
		case CONST:
			if (node->data != '0' && node->data != '1')
			{
				return INVALID_INPUT;
			}
			*res = node->data == '1' ? true : false;
			return OK;
		default:
			break;
	}
	
	if (node->content == INVERSION)
	{
		if (node->left == NULL)
		{
			return INVALID_INPUT;
		}
		status_code err_code = expr_tree_node_calc(node->left, var_table, res);
		*res = err_code ? *res : !(*res);
		return err_code;
	}
	if (node->left == NULL || node->right == NULL)
	{
		return INVALID_INPUT;
	}
	bool res_left, res_right;
	status_code err_code = expr_tree_node_calc(node->left, var_table, &res_left);
	err_code = err_code ? err_code : expr_tree_node_calc(node->right, var_table, &res_right);
	if (err_code)
	{
		return err_code;
	}
	
	switch (node->content)
	{
		case CONJUCTION:
			*res = res_left && res_right;
			return OK;
		case DISJUCTION:
			*res = res_left || res_right;
			return OK;
		case IMPLICATION:
			*res = !res_left || res_right;
			return OK;
		case COIMPLICATION:
			*res = res_left && !res_right;
			return OK;
		case EXCLUSIVE:
			*res = res_left ^ res_right;
			return OK;
		case EQUIVALENCE:
			*res = res_left == res_right;
			return OK;
		case SHEFFER_STROKE:
			*res = !(res_left && res_right);
			return OK;
		case WEBB_FUNCTION:
			*res = !(res_left || res_right);
			return OK;
		default:
			return INVALID_INPUT;
	}
}

status_code expr_tree_calc(Expression_tree etree, const bool* var_values, bool* res)
{
	if (etree.root == NULL || var_values == NULL || res == NULL)
	{
		return INVALID_ARG;
	}
	
	bool var_table[26];
	for (ull i = 0, j = 0; i < 26; ++i)
	{
		if (etree.var_flag[i])
		{
			var_table[i] = var_values[j++];
		}
	}
	
	bool res_tmp;
	status_code err_code = expr_tree_node_calc(etree.root, var_table, &res_tmp);
	if (err_code)
	{
		return err_code;
	}
	*res = res_tmp;
	return OK;
}

status_code expr_tree_fprint_table(FILE* file, Expression_tree etree)
{
	if (etree.root == NULL)
	{
		return INVALID_ARG;
	}
	
	status_code err_code = OK;
	bool* var_values = (bool*) malloc(sizeof(bool) * etree.var_cnt);
	if (var_values == NULL)
	{
		return BAD_ALLOC;
	}
	
	if (etree.var_cnt == 0)
	{
		bool res;
		err_code = expr_tree_calc(etree, var_values, &res);
		free(var_values);
		if (err_code)
		{
			return err_code;
		}
		fprintf(file, "Value of the expression is %d\n", res);
		return OK;
	}
	
	for (ull i = 0; i < 26; ++i)
	{
		if (etree.var_flag[i])
		{
			fprintf(file, "| %c ", 'A' + (int) i);
		}
	}
	fprintf(file, "| Value |\n");
	
	ull mask_end = 2 << (etree.var_cnt - 1);
	for (ull var_mask = 0; var_mask < mask_end; ++var_mask)
	{
		ull var_mask2 = var_mask;
		for (ull i = etree.var_cnt; i > 0; --i)
		{
			var_values[i - 1] = var_mask2 & 1;
			var_mask2 >>= 1;
		}
		bool res;
		err_code = expr_tree_calc(etree, var_values, &res);
		if (err_code)
		{
			free(var_values);
			return err_code;
		}
		for (ull i = 0; i < etree.var_cnt; ++i)
		{
			fprintf(file, "| %d ", var_values[i]);
		}
		fprintf(file, "|   %d   |\n", res);
	}
	free(var_values);
	return OK;
}

status_code fread_line(FILE* file, char** line)
{
	if (file == NULL || line == NULL)
	{
		return INVALID_ARG;
	}
	ull iter = 0;
	ull size = 2;
	*line = (char*) malloc(sizeof(char) * size);
	if (*line == NULL)
	{
		return BAD_ALLOC;
	}
	char ch = getc(file);
	while (!feof(file) && ch != '\n')
	{
		if (iter > size - 2)
		{
			size *= 2;
			char* temp_line = realloc(*line, size);
			if (temp_line == NULL)
			{
				free(*line);
				return BAD_ALLOC;
			}
			*line = temp_line;
		}
		(*line)[iter++] = ch;
		ch = getc(file);
	}
	(*line)[iter] = '\0';
	return OK;
}

status_code generate_random_str(char** str)
{
	if (str == NULL)
	{
		return INVALID_ARG;
	}
	ull iter = 0;
	ull size = 2;
	*str = (char*) malloc(sizeof(char) * size);
	if (*str == NULL)
	{
		return BAD_ALLOC;
	}
	char symbols[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	
	char ch = symbols[rand() % 63];
	while ((ch != '\0' || iter == 0) && iter < 63)
	{
		while (iter == 0 && ch == '\0')
		{
			ch = symbols[rand() % 63];
		}
		if (iter > size - 2)
		{
			size *= 2;
			char* temp_line = realloc(*str, size);
			if (temp_line == NULL)
			{
				free(*str);
				return BAD_ALLOC;
			}
			*str = temp_line;
		}
		(*str)[iter++] = ch;
		ch = symbols[rand() % 63];
	}
	(*str)[iter] = '\0';
	return OK;
}

status_code construct_output_path(const char* input, const char* output_name, char** output)
{
	if (input == NULL || output_name == NULL || output == NULL)
	{
		return INVALID_ARG;
	}
	ull last_delim_pos = 0;
	for (ull i = 0; input[i]; ++i)
	{
		if (input[i] == '/' || input[i] == '\\')
		{
			last_delim_pos = i;
		}
	}
	*output = (char*) malloc(sizeof(char) * (last_delim_pos + strlen(output_name) + 2));
	if (*output == NULL)
	{
		return BAD_ALLOC;
	}
	memcpy(*output, input, sizeof(char) * (last_delim_pos + 1));
	strcpy(*output + last_delim_pos + 1, output_name);
	return OK;
}
