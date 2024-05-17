#include <stdio.h>
#include <stdlib.h>

#include <utility.h>
#include <trie.h>

int main()
{
    print_error(FILE_INVALID_CONTENT, 1);
    
    Trie trie;
    trie_set_null(&trie);
    trie_construct(&trie);
    trie_insert(&trie, "123", 5);
    trie_insert(&trie, "12345", 6);
    trie_insert(&trie, "100", 7);
    trie_insert(&trie, "1001", 8);
    trie_insert(&trie, "", 9);
    
    ull sz;
    trie_key_val* kv = NULL;
    
    int cont = 0;
    trie_contains(&trie, "", &cont);
    printf("%d\n", cont);
    
    trie_get_key_vals(&trie, &sz, &kv);
    
    for (ull i = 0; i < sz; ++i)
    {
        printf("%s: %d\n", kv[i].key, kv[i].value);
    }
    
    for (ull i = 0; i < sz; ++i)
    {
        free(kv[i].key);
    }
    free(kv);
    
    trie_destruct(&trie);
    
    printf("Cool\n");
}