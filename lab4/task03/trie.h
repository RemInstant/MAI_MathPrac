#ifndef _TRIE_H_
#define _TRIE_H_

#include "utility.h"

#define ALPABET_LEN 36
#define DEFAULT_HASH_PARAM 37

typedef long long ll;
typedef unsigned long long ull;

typedef struct trie_node
{
	struct trie_node* parent;
	struct trie_node* children[ALPABET_LEN];
	ll val;
} trie_node;

typedef struct Trie
{
	trie_node* root;
	ull (*calc_hash)(const char*);
	ull hash;
} Trie;

typedef struct trie_key_val
{
	char* str;
	ll val;
} trie_key_val;

status_code trie_set_null(Trie* trie);
status_code trie_construct(Trie* trie, ull (*calc_hash)(const char*));
status_code trie_destruct(Trie* trie);
status_code trie_contains(Trie trie, const char* str, int* is_contained);
status_code trie_get(Trie trie, const char* str, ll* val);
status_code trie_set(Trie* trie, const char* str, ll val);
status_code trie_add(Trie* trie, const char* str, ll val);
status_code trie_equal(Trie trie1, Trie trie2, int* is_equal);
status_code trie_get_key_vals(Trie trie, ull* cnt, trie_key_val** key_vals);

ull calc_default_trie_hash(const char* str);

#endif // _TRIE_H_