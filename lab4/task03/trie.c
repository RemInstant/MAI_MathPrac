#include "trie.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

int char_to_int(char ch)
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

int char_to_ind(char ch)
{
	return char_to_int(ch);
}

char ind_to_char(int ind)
{
	if (ind >= 0 && ind <= 9)
	{
		return '0' + ind;
	}
	if (ind > 9 && ind < 36)
	{
		return 'a' + ind - 10;
	}
	return '\0';
}

ull calc_default_trie_hash(const char* str)
{
	if (str == NULL)
	{
		return 0;
	}
	ull res = 0;
	for (ull i = 0; str[i]; ++i)
	{
		res *= DEFAULT_HASH_PARAM;
		res += char_to_int(str[i]);
	}
	return res;
}


status_code trie_node_construct(trie_node** node)
{
	if (node == NULL)
	{
		return INVALID_ARG;
	}
	*node = (trie_node*) malloc(sizeof(trie_node));
	if (*node == NULL)
	{
		return BAD_ALLOC;
	}
	for (ull i = 0; i < ALPABET_LEN; ++i)
	{
		(*node)->children[i] = NULL;
	}
	(*node)->parent = NULL;
	(*node)->val = 0;
	return OK;
}

status_code trie_node_destruct(trie_node* node)
{
	if (node == NULL)
	{
		return INVALID_ARG;
	}
	for (ull i = 0; i < ALPABET_LEN; ++i)
	{
		if (node->children[i] != NULL)
		{
			trie_node_destruct(node->children[i]);
		}
		free(node->children[i]);
		node->children[i] = NULL;
	}
	node->parent = NULL;
	return OK;
}

status_code trie_set_null(Trie* trie)
{
	if (trie == NULL)
	{
		return INVALID_ARG;
	}
	trie->root = NULL;
	trie->hash = 0;
	return OK;
}

status_code trie_construct(Trie* trie, ull (*calc_hash)(const char*))
{
	if (trie == NULL)
	{
		return INVALID_ARG;
	}
	status_code err_code = trie_node_construct(&trie->root);
	if (err_code)
	{
		return err_code;
	}
	trie->calc_hash = calc_hash;
	trie->hash = 0;
	return OK;
}

status_code trie_destruct(Trie* trie)
{
	if (trie == NULL)
	{
		return INVALID_ARG;
	}
	status_code err_code = OK;
	if (trie->root != NULL)
	{
		err_code = trie_node_destruct(trie->root);
	}
	free(trie->root);
	trie->root = NULL;
	trie->calc_hash = NULL;
	trie->hash = 0;
	return err_code;
}


status_code trie_search_node(Trie trie, const char* str, trie_node** node)
{
	if (str == NULL || node == NULL)
	{
		return INVALID_ARG;
	}
	if (trie.root == NULL)
	{
		return INVALID_INPUT;
	}
	*node = NULL;
	trie_node* cur = trie.root;
	for (ull i = 0; str[i]; ++i)
	{
		if (cur == NULL)
		{
			*node = NULL;
			return OK;
		}
		int ind = char_to_ind(str[i]);
		if (ind == -1)
		{
			return INVALID_INPUT;
		}
		cur = cur->children[ind];
	}
	*node = cur;
	return OK;
}

status_code trie_create_node(Trie trie, const char* str, trie_node** node)
{
	if (str == NULL)
	{
		return INVALID_ARG;
	}
	if (trie.root == NULL)
	{
		return INVALID_INPUT;
	}
	status_code err_code = OK;
	trie_node* cur = trie.root;
	trie_node* first_new = NULL;
	for (ull i = 0; str[i] && !err_code; ++i)
	{
		int ind = char_to_ind(str[i]);
		if (ind == -1)
		{
			err_code = INVALID_INPUT;
		}
		if (cur->children[ind] == NULL)
		{
			err_code = trie_node_construct(&cur->children[ind]);
			first_new = first_new == NULL ? cur->children[ind] : first_new;
			if (!err_code)
			{
				cur->children[ind]->parent = cur;
			}
		}
		if (!err_code)
		{
			cur = cur->children[ind];
		}
	}
	if (err_code)
	{
		trie_node_destruct(first_new);
		free(first_new);
		cur = NULL;
	}
	if (node != NULL)
	{
		*node = cur;
	}
	return err_code;
}

status_code trie_clean_branch(Trie trie, trie_node* node, trie_node* cleaned_child_node)
{
	if (node == NULL)
	{
		return INVALID_ARG;
	}
	int child_flag = 0;
	for (ull i = 0; i < ALPABET_LEN; ++i)
	{
		if (cleaned_child_node != NULL && node->children[i] == cleaned_child_node)
		{
			node->children[i] = NULL;
		}
		if (node->children[i] != NULL)
		{
			child_flag = 1;
		}
	}
	if (child_flag || trie.root == node || node->val != 0)
	{
		return OK;
	}
	trie_node* parent = node->parent;
	trie_node_destruct(node);
	free(node);
	return trie_clean_branch(trie, parent, node);
}


status_code trie_contains(Trie trie, const char* str, int* is_contained)
{
	if (str == NULL || is_contained == NULL)
	{
		return INVALID_ARG;
	}
	trie_node* node = NULL;
	status_code err_code = trie_search_node(trie, str, &node);
	if (err_code)
	{
		return err_code;
	}
	*is_contained = node != NULL && node->val > 0;
	return OK;
}

status_code trie_get(Trie trie, const char* str, ll* val)
{
	if (str == NULL || val == NULL)
	{
		return INVALID_ARG;
	}
	trie_node* node = NULL;
	status_code err_code = trie_search_node(trie, str, &node);
	if (err_code)
	{
		return err_code;
	}
	*val = node == NULL ? 0 : node->val;
	return OK;
}

status_code trie_set(Trie* trie, const char* str, ll val)
{
	if (trie == NULL || str == NULL)
	{
		return INVALID_ARG;
	}
	trie_node* node = NULL;
	status_code err_code = trie_create_node(*trie, str, &node);
	if (err_code)
	{
		return err_code;
	}
	ull str_hash = trie->calc_hash(str);
	trie->hash +=  val * str_hash - node->val * str_hash;
	node->val = val;
	if (node->val == 0)
	{
		return trie_clean_branch(*trie, node, NULL);
	}
	return OK;
}

status_code trie_add(Trie* trie, const char* str, ll val)
{
	if (trie == NULL || str == NULL)
	{
		return INVALID_ARG;
	}
	trie_node* node = NULL;
	status_code err_code = trie_create_node(*trie, str, &node);
	if (err_code)
	{
		return err_code;
	}
	ull str_hash = trie->calc_hash(str);
	trie->hash += val * str_hash;
	node->val += val;
	if (node->val == 0)
	{
		return trie_clean_branch(*trie, node, NULL);
	}
	return OK;
}


status_code trie_node_equal(trie_node* node_1, trie_node* node_2, int* is_equal)
{
	if (is_equal == NULL)
	{
		return INVALID_ARG;
	}
	if (node_1 == NULL && node_2 == NULL)
	{
		*is_equal = 1;
		return OK;
	}
	ll value_1 = node_1 == NULL ? 0 : node_1->val;
	ll value_2 = node_2 == NULL ? 0 : node_2->val;
	if (value_1 != value_2)
	{
		*is_equal = 0;
		return OK;
	}
	status_code err_code = OK;
	for (ull i = 0; i < ALPABET_LEN; ++i)
	{
		trie_node* child_1 = node_1 != NULL ? node_1->children[i] : NULL;
		trie_node* child_2 = node_2 != NULL ? node_2->children[i] : NULL;
		int tmp = 1;
		err_code = trie_node_equal(child_1, child_2, &tmp);
		if (err_code)
		{
			return err_code;
		}
		if (!tmp)
		{
			*is_equal = 0;
			return OK;
		}
	}
	*is_equal = 1;
	return OK;
}

status_code trie_equal(Trie trie1, Trie trie2, int* is_equal)
{
	if (is_equal == NULL)
	{
		return INVALID_ARG;
	}
	if (trie1.root == NULL || trie2.root == NULL)
	{
		return INVALID_INPUT;
	}
	if (trie1.calc_hash == trie2.calc_hash && trie1.hash != trie2.hash)
	{
		*is_equal = 0;
		return OK;
	}
	return trie_node_equal(trie1.root, trie2.root, is_equal);
}

status_code trie_get_key_vals(Trie trie, ull* cnt, trie_key_val** key_vals)
{
	if (cnt == NULL || key_vals == NULL)
	{
		return INVALID_ARG;
	}
	if (trie.root == NULL)
	{
		return INVALID_INPUT;
	}
	
	ull kv_cnt = 0;
	ull kv_size = 2;
	trie_key_val* key_vals_tmp = (trie_key_val*) malloc(sizeof(trie_key_val) * 2);
	if (key_vals_tmp == NULL)
	{
		return BAD_ALLOC;
	}
	ull str_iter = 0;
	ull str_size = 2;
	char* str = malloc(sizeof(char) * 2);
	if (str == NULL)
	{
		free(key_vals_tmp);
		return BAD_ALLOC;
	}
	
	status_code err_code = OK;
	trie_node* cur_node = trie.root;
	int ind = 0;
	int run_flag = 1;
	while (!err_code && run_flag)
	{
		// Select node to process
		while (cur_node->children[ind] == NULL && ind < 36)
		{
			++ind;
		}
		// No node to process, back to parent
		if (ind >= 36)
		{
			if (cur_node != trie.root)
			{
				ind = char_to_ind(str[--str_iter]) + 1;
				str[str_iter] = '\0';
				cur_node = cur_node->parent;
			}
			else
			{
				run_flag = 0;
			}
		}
		// Go process node
		else
		{
			// Update buffer string, ind and cur_node
			if (str_iter + 1 == str_size)
			{
				str_size *= 2;
				char* tmp = (char*) realloc(str, sizeof(char) * str_size);
				err_code = tmp != NULL ? OK : BAD_ALLOC;
				str = tmp != NULL ? tmp : str;
			}
			if (!err_code)
			{
				str[str_iter++] = ind_to_char(ind);
				str[str_iter] = '\0';
			}
			cur_node = cur_node->children[ind];
			ind = 0;
			
			// Process node
			if (!err_code && cur_node->val != 0)
			{
				char* str_copy = (char*) malloc(sizeof(char) * (str_iter + 1));
				err_code = str_copy != NULL ? OK : BAD_ALLOC;
				if (str_copy != NULL)
				{
					strcpy(str_copy, str);
				}
				if (!err_code && kv_cnt == kv_size)
				{
					kv_size *= 2;
					trie_key_val* tmp = (trie_key_val*) realloc(key_vals_tmp, sizeof(trie_key_val) * kv_size);
					err_code = tmp != NULL ? OK : BAD_ALLOC;
					key_vals_tmp = tmp != NULL ? tmp : key_vals_tmp;
				}
				if (!err_code)
				{
					key_vals_tmp[kv_cnt].str = str_copy;
					key_vals_tmp[kv_cnt++].val = cur_node->val;
				}
				else
				{
					free(str_copy);
				}
			}
		}
	} // while end
	free(str);
	if (err_code)
	{
		for (ull i = 0; i < kv_cnt; ++i)
		{
			free(key_vals_tmp[i].str);
		}
		free(key_vals_tmp);
		return err_code;
	}
	*cnt = kv_cnt;
	*key_vals = key_vals_tmp;
	return OK;
}
