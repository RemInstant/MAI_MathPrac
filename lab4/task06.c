#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <errno.h>

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
} status_codes;

void print_error(status_codes code)
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
	LEFT_BRACKET,
	RIGHT_BRACKET,
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

status_codes tree_node_dynamic_construct(tree_node** node, node_content content, char data);
status_codes tree_node_destruct(tree_node* node);

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

status_codes stack_set_null(Stack* stack);
status_codes stack_construct(Stack* stack);
status_codes stack_destruct(Stack* stack);
status_codes stack_empty(Stack* stack, int* empty);
status_codes stack_top(Stack stack, tree_node** node);
status_codes stack_push(Stack* stack, tree_node* node);
status_codes stack_pop(Stack* stack, tree_node** node);

typedef struct
{
	tree_node* root;
	int varFlag[26];
} Expression_tree;

status_codes get_token(const char* src, const char** end_ptr, node_content* content, char* data);
status_codes to_postfix_notaion(const char* infix, char** postfix);
status_codes count_vars(const char* expr);
status_codes expr_tree_set_null(Expression_tree* etree);
status_codes expr_tree_construct(Expression_tree* etree, const char* infix);
status_codes expr_tree_destruct(Expression_tree* etree);
status_codes expr_tree_destruct(Expression_tree* etree);
status_codes expr_tree_calc(Expression_tree etree, int values[26], int* res);
status_codes expr_tree_print_table(Expression_tree etree);

status_codes fread_line(FILE* file, char** str);

status_codes sread_until(const char* src, const char* delims, const char** end_ptr, char** str);
status_codes fread_expr(FILE* file, char** str);
//status_codes parse_expr(const char* src, operation* op, ull* arg_cnt, char** arg1, char** arg2, char** arg3);

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		//printf("Usage: cmd_path <input>\n");
		//return OK;
	}
	if (argc != 2)
	{
		//return INVALID_INPUT;
	}
	
	//FILE* input = fopen(argv[1], "r");
	FILE* input = fopen("lab4/t06_input", "r");
	if (input == NULL)
	{
		return FILE_OPENING_ERROR;
	}
	
	char* infix_expr;
	status_codes err_code = fread_line(input, &infix_expr);
	
	Expression_tree etree;
	expr_tree_set_null(&etree);
	err_code = err_code ? err_code : expr_tree_construct(&etree, infix_expr);
	
	free(infix_expr);
	expr_tree_destruct(&etree);
	if (err_code)
	{
		print_error(err_code);
		return err_code;
	}
}

status_codes tree_node_dynamic_construct(tree_node** node, node_content content, char data)
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

status_codes tree_node_destruct(tree_node* node)
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

status_codes stack_set_null(Stack* stack)
{
	if (stack == NULL)
	{
		return INVALID_ARG;
	}
	stack->top = stack->bottom = NULL;
	stack->size = 0;
	return OK;
}

status_codes stack_construct(Stack* stack)
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

status_codes stack_destruct(Stack* stack)
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

status_codes stack_empty(Stack* stack, int* empty)
{
	if (stack == NULL || empty == NULL)
	{
		return INVALID_ARG;
	}
	*empty = stack->top == stack->bottom;
	return OK;
}

status_codes stack_top(Stack stack, tree_node** node)
{
	if (node == NULL)
	{
		return INVALID_ARG;
	}
	*node = stack.top->node;
	return OK;
}

status_codes stack_push(Stack* stack, tree_node* tr_node)
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

status_codes stack_pop(Stack* stack, tree_node** node)
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

int get_priority(node_content op)
{
	switch (op)
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

status_codes get_token(const char* src, const char** end_ptr, node_content* content, char* data)
{
	if (src == NULL || end_ptr == NULL || content == NULL || data == NULL)
	{
		return INVALID_ARG;
	}
	status_codes err_code = OK;
	if (isalpha(*src))
	{
		*content = VAR;
		*data = *src;
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
		*content = LEFT_BRACKET;
	}
	else if (*src == ')')
	{
		*content = RIGHT_BRACKET;
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

status_codes validate_token_combination(node_content prev, node_content cur)
{
	if (is_operation(prev) && is_operation(cur))
	{
		return INVALID_INPUT;
	}
	if ((prev == CONST || prev == VAR) && (cur == CONST || cur == VAR))
	{
		return INVALID_INPUT;
	}
	if ((prev == LEFT_BRACKET && cur == RIGHT_BRACKET) || (prev == RIGHT_BRACKET && cur == RIGHT_BRACKET))
	{
		return INVALID_INPUT;
	}
	if ((is_operation(prev) && cur == RIGHT_BRACKET) || (prev == LEFT_BRACKET && is_operation(cur)))
	{
		return INVALID_INPUT;
	}
	if (((prev == CONST || prev == VAR) && cur == LEFT_BRACKET)
			|| (prev == RIGHT_BRACKET && (cur == CONST || cur == VAR)))
	{
		return INVALID_INPUT;
	}
	return OK;
}

status_codes expr_tree_set_null(Expression_tree* etree)
{
	etree->root = NULL;
	return OK;
}

status_codes expr_tree_construct(Expression_tree* etree, const char* infix)
{
	if (etree == NULL || infix == NULL)
	{
		return INVALID_ARG;
	}
	
	status_codes err_code = OK;
	Expression_tree etree_tmp;
	Stack args;
	Stack opers;
	expr_tree_set_null(&etree_tmp);
	stack_set_null(&args);
	stack_set_null(&opers);
	err_code = err_code ? err_code : stack_construct(&args);
	err_code = err_code ? err_code : stack_construct(&opers);
	
	node_content prev_content = NONE;
	const char* ptr = infix;
	while(!err_code && *ptr)
	{
		node_content content = NONE;
		char data = 0;
		err_code = get_token(ptr, &ptr, &content, &data);
		err_code = err_code ? err_code : validate_token_combination(prev_content, content);
		// --- HANDLE VARIABLE AND CONSTANT ---
		if (!err_code && (content == VAR || content == CONST))
		{
			tree_node* tnode = NULL;
			err_code = tree_node_dynamic_construct(&tnode, content, data);
			err_code = err_code ? err_code : stack_push(&args, tnode);
		}
		// --- HANDLE OPERATION ---
		else if (!err_code && is_operation(content))
		{
			tree_node* tnode = NULL;
			stack_top(opers, &tnode);
			int cur_prior = get_priority(content);
			int other_prior = tnode != NULL ? get_priority(tnode->content) : -1;
			while (!err_code && other_prior >= cur_prior)
			{
				err_code = args.size >= 2 ? OK : INVALID_INPUT;
				// combine operation with 2 args
				err_code = err_code ? err_code : stack_pop(&args, &tnode->right);
				err_code = err_code ? err_code : stack_pop(&args, &tnode->left);
				err_code = err_code ? err_code : stack_push(&args, tnode);
				// prepare to check next operation
				err_code = err_code ? err_code : stack_pop(&opers, NULL);
				stack_top(opers, &tnode);
				other_prior = tnode != NULL ? get_priority(tnode->content) : -1;
			}
			err_code = err_code ? err_code : tree_node_dynamic_construct(&tnode, content, data);
			err_code = err_code ? err_code : stack_push(&opers, tnode);
		}
		// --- HANDLE LEFT BRACKET ---
		else if (!err_code && content == LEFT_BRACKET)
		{
			tree_node* tnode = NULL;
			err_code = tree_node_dynamic_construct(&tnode, content, data);
			err_code = err_code ? err_code : stack_push(&opers, tnode);
		}
		// --- HANDLE RIGHT BRACKET ---
		else if (!err_code && content == RIGHT_BRACKET)
		{
			tree_node* tnode = NULL;
			stack_top(opers, &tnode);
			err_code = tnode != NULL ? OK : INVALID_INPUT;
			while (!err_code && tnode->content != LEFT_BRACKET)
			{
				err_code = args.size >= 2 ? OK : INVALID_INPUT;
				// combine operation with 2 args
				err_code = err_code ? err_code : stack_pop(&args, &tnode->right);
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
		tree_node* tnode = NULL;
		err_code = stack_pop(&opers, &tnode);
		err_code = err_code ? err_code : (tnode->content != LEFT_BRACKET ? OK : INVALID_INPUT);
		err_code = err_code ? err_code : (args.size >= 2 ? OK : INVALID_INPUT);
		// combine operation with 2 args
		err_code = err_code ? err_code : stack_pop(&args, &tnode->right);
		err_code = err_code ? err_code : stack_pop(&args, &tnode->left);
		err_code = err_code ? err_code : stack_push(&args, tnode);
	}
	if (!err_code)
	{
		err_code = args.size == 1 ? OK : INVALID_INPUT;
		tree_node* tnode = NULL;
		err_code = err_code ? err_code : stack_pop(&args, &tnode);
		etree_tmp.root = tnode;
	}
	if (err_code)
	{
		expr_tree_destruct(&etree_tmp);
		stack_destruct(&args);
		stack_destruct(&opers);
		return err_code;
	}
	*etree = etree_tmp;
	return OK;
}

status_codes expr_tree_destruct(Expression_tree* etree)
{
	if (etree == NULL)
	{
		return INVALID_ARG;
	}
	tree_node_destruct(etree->root);
	free(etree->root);
	return OK;
}

status_codes fread_line(FILE* file, char** line)
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
	while (!feof(file))
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










