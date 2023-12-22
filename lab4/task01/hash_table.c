#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

status_code htable_set_null(htable* ht)
{
	if (ht == NULL)
	{
		return NULL_ARG;
	}
	ht->chains = NULL;
	ht->chain_sizes = NULL;
	ht->cache = NULL;
	return OK;
}

status_code htable_construct(htable* ht, ull (*calc_hash)(const char*, ull))
{
	if (ht == NULL)
	{
		return NULL_ARG;
	}
	ht->chains = (ht_node**) calloc(MIN_HT_SIZE, sizeof(ht_node**));
	ht->chain_sizes = (ull*) calloc(MIN_HT_SIZE, sizeof(ull));
	ht->cache = (ht_cache*) malloc(sizeof(ht_cache) * 4);
	if (ht->chains == NULL || ht->chain_sizes == NULL || ht->cache == NULL)
	{
		free_all(3, ht->chains, ht->chain_sizes, ht->cache);
		ht->chains = NULL;
		ht->chain_sizes = NULL;
		ht->cache = NULL;
		return BAD_ALLOC;
	}
	ht->calc_hash = calc_hash;
	ht->ht_size = MIN_HT_SIZE;
	ht->max_chain_size = ht->min_chain_size = 0;
	ht->min_chain_cnt = MIN_HT_SIZE;
	ht->cache_cnt = 0;
	ht->cache_size = 4;
	return OK;
}

status_code htable_destruct(htable* ht)
{
	if (ht == NULL)
	{
		return NULL_ARG;
	}
	for (ull i = 0; i < ht->cache_cnt; ++i)
	{
		free_all(3, ht->cache[i].node->key, ht->cache[i].node->value, ht->cache[i].node);
	}
	free_all(3, ht->chains, ht->chain_sizes, ht->cache);
	ht->chains = NULL;
	ht->chain_sizes = NULL;
	ht->cache = NULL;
	return OK;
}

status_code htable_rebuild(htable* ht, ull param)
{
	if (ht == NULL)
	{
		return INVALID_ARG;
	}
	if (ht->ht_size > MAX_HT_SIZE / param)
	{
		return BAD_ALLOC;
	}
	ull new_size = ht->ht_size * param;
	ht_node** new_chains = (ht_node**) calloc(new_size, sizeof(ht_node*));
	ull* new_chain_sizes = (ull*) calloc(new_size, sizeof(ull));
	if (new_chains == NULL || new_chain_sizes == NULL)
	{
		free_all(2, new_chains, new_chain_sizes);
		return BAD_ALLOC;
	}
	for (ull i = 0; i < ht->cache_cnt; ++i)
	{
		ull ind = ht->cache[i].big_hash % new_size;
		ht_node* node = ht->cache[i].node;
		node->next = new_chains[ind];
		new_chains[ind] = node->next;
		new_chain_sizes[ind]++;
	}
	ull max_chain_size = 0;
	ull min_chain_size = ULLONG_MAX;
	ull min_chain_cnt = 0;
	for (ull i = 0; i < new_size; ++i)
	{
		if (new_chain_sizes[i] < min_chain_size)
		{
			min_chain_size = new_chain_sizes[i];
			min_chain_cnt = 1;
		}
		else if (new_chain_sizes[i] == min_chain_size)
		{
			min_chain_cnt++;
		}
		if (new_chain_sizes[i] > max_chain_size)
		{
			max_chain_size = new_chain_sizes[i];
		}
	}
	if (max_chain_size > 2 * (min_chain_size ? min_chain_size : 1))
	{
		free_all(2, new_chains, new_chain_sizes);
		return htable_rebuild(ht, param * 2);
	}
	free_all(2, ht->chains, ht->chain_sizes);
	ht->ht_size = new_size;
	ht->chains = new_chains;
	ht->chain_sizes = new_chain_sizes;
	ht->max_chain_size = max_chain_size;
	ht->min_chain_size = min_chain_size;
	ht->min_chain_cnt = min_chain_cnt;
	return OK;
}

status_code htable_search(htable* ht, const char* key, int create_flag, ht_node** node)
{
	if (ht == NULL || key == NULL || node == NULL)
	{
		return NULL_ARG;
	}
	ull hash = ht->calc_hash(key, (ull) 1 << 58);
	ull ind = hash % ht->ht_size;
	ht_node* cur = ht->chains[ind];
	while (cur != NULL && strcmp(cur->key, key))
	{
		cur = cur->next;
	}
	if (cur != NULL || !create_flag)
	{
		*node = cur;
		return OK;
	}
	
	if (ht->cache_cnt == ht->cache_size)
	{
		ht_cache* tmp = (ht_cache*) realloc(ht->cache, sizeof(ht_cache) * ht->cache_size * 2);
		if (tmp == NULL)
		{
			return BAD_ALLOC;
		}
		ht->cache_size *= 2;
		ht->cache = tmp;
	}
	
	ht_node* new_node = (ht_node*) malloc(sizeof(ht_node));
	char* key_copy = (char*) malloc(sizeof(char) * (strlen(key) + 1));
	if (new_node == NULL || key_copy == NULL)
	{
		free_all(2, new_node, key_copy);
		return BAD_ALLOC;
	}
	strcpy(key_copy, key);
	new_node->key = key_copy;
	new_node->value = NULL;
	new_node->next = ht->chains[ind]; // insert in head
	ht->chains[ind] = new_node;
	ht->cache[ht->cache_cnt++] = (ht_cache) { .node = new_node, .big_hash = hash };
	*node = new_node;
	
	ull cur_size = ht->chain_sizes[ind];
	ht->chain_sizes[ind]++;
	// update data abount max chain
	if (ht->max_chain_size == cur_size)
	{
		ht->max_chain_size++;
	}
	// update data about min chain
	if (ht->min_chain_size == cur_size)
	{
		ht->min_chain_cnt--;
		if (ht->min_chain_cnt == 0)
		{
			ht->min_chain_size = cur_size + 1;
			for (ull i = 0; i < ht->ht_size; ++i)
			{
				if (ht->chain_sizes[i] == ht->min_chain_size)
				{
					ht->min_chain_cnt++;
				}
			}
		}
	}
	ull mn = ht->min_chain_size ? ht->min_chain_size : 1;
	if (ht->max_chain_size > 2 * mn)
	{
		status_code code = htable_rebuild(ht, 2);
		return code;
	}
	return OK;
}

status_code htable_contains(htable* ht, const char* key, int* is_contained)
{
	if (ht == NULL || key == NULL || is_contained == NULL)
	{
		return NULL_ARG;
	}
	ht_node* node = NULL;
	status_code err_code = htable_search(ht, key, 0, &node);
	*is_contained = node ? 1 : 0;
	return err_code;
}

status_code htable_set_value(htable* ht, const char* key, const char* value)
{
	if (ht == NULL || key == NULL || value == NULL)
	{
		return NULL_ARG;
	}
	ht_node* node = NULL;
	status_code err_code = htable_search(ht, key, 1, &node);
	if (err_code)
	{
		return err_code;
	}
	free(node->value);
	node->value = (char*) malloc(sizeof(char) * (strlen(value) + 1));
	if (node->value == NULL)
	{
		return BAD_ALLOC;
	}
	strcpy(node->value, value);
	return OK;
}

status_code htable_get_value(htable* ht, const char* key, char** value)
{
	if (ht == NULL || key == NULL || value == NULL)
	{
		return NULL_ARG;
	}
	ht_node* node = NULL;
	status_code err_code = htable_search(ht, key, 0, &node);
	if (err_code)
	{
		return err_code;
	}
	if (node == NULL)
	{
		return BAD_ACCESS;
	}
	*value = (char*) malloc(sizeof(char) * (strlen(node->value) + 1));
	if (*value == NULL)
	{
		return BAD_ALLOC;
	}
	strcpy(*value, node->value);
	return OK;
}

ull calc_default_hash(const char* str, ull mod)
{
	if (str == NULL)
	{
		return 0;
	}
	ull res = 0;
	for (int i = 0; str[i]; ++i)
	{
		res = res * KEY_ALPHABET_SIZE % mod;
		res = (res + ctoi(str[i], 1)) % mod;
	}
	return res;
}