#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include "utility.h"

#define KEY_ALPHABET_SIZE 62
#define MIN_HT_SIZE 128
#define MAX_HT_SIZE (1ULL << 58)

typedef struct ht_node
{
	char* key;
	char* value;
	struct ht_node* next;
} ht_node;

typedef struct ht_cache
{
	ht_node* node;
	ull big_hash;
} ht_cache;

typedef struct hash_table
{
	ull (*calc_hash)(const char*, ull);
	ull ht_size;
	ht_node** chains;
	ull* chain_sizes;
	ull max_chain_size;
	ull min_chain_size;
	ull min_chain_cnt;
	ull cache_cnt;
	ull cache_size;
	ht_cache* cache;
} hash_table, htable;

status_code htable_set_null(htable* ht);
status_code htable_construct(htable* ht, ull (*calc_hash)(const char*, ull));
status_code htable_destruct(htable* ht);
status_code htable_contains(htable* ht, const char* key, int* is_contained);
status_code htable_set_value(htable* ht, const char* key, const char* value);
status_code htable_get_value(htable* ht, const char* key, char** value);

ull calc_default_hash(const char* str, ull mod);

#endif // _HASH_TABLE_H_