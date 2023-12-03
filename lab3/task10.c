#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define EPS 1e-12

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
	BAD_ALLOC
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
		case BAD_ALLOC:
			printf("Memory lack error occurred\n");
			return;
		default:
			printf("Unexpected error occurred\n");
			return;
	}
}

status_codes fread_line(FILE* file, char** str)
{
	if (file == NULL || str == NULL)
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
	char ch = getc(file);
	while (ch != '\n' && ch != EOF)
	{
		if (iter > size - 2)
		{
			size *= 2;
			char* temp_word = realloc(*str, sizeof(char) * size);
			if (temp_word == NULL)
			{
				free(*str);
				return BAD_ALLOC;
			}
			*str = temp_word;
		}
		(*str)[iter++] = ch;
		ch = getc(file);
	}
	(*str)[iter] = '\0';
	return OK;
}

typedef struct tree_node
{
	struct tree_node* parent;
	struct tree_node* brother;
	struct tree_node* child;
	char key;
} tree_node;

typedef struct
{
	tree_node* root;
} Tree;

status_codes create_node(tree_node** node, const char key)
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
	(*node)->parent = NULL;
	(*node)->brother = NULL;
	(*node)->child = NULL;
	(*node)->key = key;
	return OK;
}

status_codes add_child(tree_node* node, const char key, tree_node** created_node)
{
	if (node == NULL)
	{
		return INVALID_ARG;
	}
	
	tree_node* new_node;
	status_codes create_code = create_node(&new_node, key);
	if (create_code != OK)
	{
		return create_code;
	}
	
	new_node->parent = node;
	if (node->child == NULL)
	{
		node->child = new_node;
	}
	else
	{
		tree_node* youngest_child = node->child;
		while (youngest_child->brother != NULL)
		{
			youngest_child = youngest_child->brother;
		}
		youngest_child->brother = new_node;
	}
	if (created_node != NULL)
	{
		*created_node = new_node;
	}
	return OK;
}

status_codes destruct_tree_node(tree_node* tree)
{
	if (tree == NULL)
	{
		return INVALID_ARG;
	}
	if (tree->child != NULL)
	{
		destruct_tree_node(tree->child);
	}
	if (tree->brother != NULL)
	{
		destruct_tree_node(tree->brother);
	}
	free(tree->child);
	free(tree->brother);
	return OK;
}

status_codes destruct_tree(Tree* tree)
{
	if (tree == NULL)
	{
		return INVALID_ARG;
	}
	status_codes code = destruct_tree_node(tree->root);
	if (code != OK)
	{
		return code;
	}
	free(tree->root);
	tree->root = NULL;
	return OK;
}

status_codes construct_tree(const char* str, Tree* tree)
{
	if (str == NULL || tree == NULL)
	{
		return INVALID_ARG;
	}
	
	const char* ptr = str;
	tree->root = NULL;
	if (*ptr == '\0')
	{
		return OK;
	}
	
	status_codes err_code = OK;
	tree_node* cur_node = NULL;
	tree_node* last_added_node = NULL;
	int key_waiting_flag = 1;
	while (*ptr != '\0' && !err_code)
	{
		while (*ptr == ' ')
		{
			++ptr;
		}
		
		if (key_waiting_flag)
		{
			key_waiting_flag = 0;
			if (*ptr == ')')
			{
				cur_node = cur_node->parent;
			}
			else if (cur_node == NULL)
			{
				err_code = create_node(&(tree->root), *ptr);
				last_added_node = tree->root;
			}
			else
			{
				err_code = add_child(cur_node, *ptr, &last_added_node);
			}
		}
		else
		{
			key_waiting_flag = 1;
			if (*ptr == '(')
			{
				if (last_added_node == NULL)
				{
					err_code = INVALID_INPUT;
				}
				else
				{
					cur_node = last_added_node;
				}
			}
			else if (*ptr == ')')
			{
				if (cur_node == NULL)
				{
					err_code = INVALID_INPUT;
				}
				else
				{
					cur_node = cur_node->parent;
				}
				key_waiting_flag = 0;
			}
			else if (*ptr != ',')
			{
				err_code = INVALID_INPUT;
			}
		}
		++ptr;
	}
	
	if (cur_node != NULL || key_waiting_flag)
	{
		err_code = INVALID_INPUT;
	}
	if (err_code)
	{
		destruct_tree(tree);
		return err_code;
	}
	return OK;
}

void fprint_tree_node(FILE* file, const tree_node* node, const ull offset)
{
	if (node == NULL || file == NULL)
	{
		return;
	}
	for (ull i = 0; i < offset; ++i)
	{
		fprintf(file, "  ");
	}
	fprintf(file, "%c\n", node->key);
	fprint_tree_node(file, node->child, offset + 1);
	fprint_tree_node(file, node->brother, offset);
}

void fprint_tree(FILE* file, const Tree tree)
{
	if (file == NULL)
	{
		return;
	}
	fprint_tree_node(file, tree.root, 0);
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: cmd_path <input> <output>\n");
		return OK;
	}
	
	if (argc != 3)
	{
		print_error(INVALID_INPUT);
		return INVALID_INPUT;
	}
	
	FILE* input = fopen(argv[1], "r");
	if (input == NULL)
	{
		print_error(FILE_OPENING_ERROR);
		return FILE_OPENING_ERROR;
	}
	
	FILE* output = fopen(argv[2], "w");
	if (output == NULL)
	{
		fclose(input);
		print_error(FILE_OPENING_ERROR);
		return FILE_OPENING_ERROR;
	}
	
	ull iter = 1;
	status_codes err_code = OK;
	while (!feof(input))
	{
		Tree tree;
		char* str = NULL;
		err_code = fread_line(input, &str);
		
		if (!err_code)
		{
			err_code = construct_tree(str, &tree);
			if (err_code == INVALID_INPUT)
			{
				err_code = FILE_CONTENT_ERROR;
			}
		}
		if (!err_code)
		{
			fprintf(output, "Tree N%llu\n", iter);
			fprint_tree(output, tree);
			if (!feof(input))
			{
				fprintf(output, "\n");
			}
			err_code = destruct_tree(&tree);
			err_code = err_code == INVALID_ARG ? OK : err_code;
		}
		free(str);
		++iter;
	}
	if (err_code)
	{
		print_error(err_code);
		return err_code;
	}
	
	fclose(input);
	fclose(output);
}