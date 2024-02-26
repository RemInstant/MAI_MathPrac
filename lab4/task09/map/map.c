#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "trie.h"

status_code map_set_null(Map* map)
{
    if (map == NULL)
    {
        return NULL_ARG;
    }
    map->ds = NULL;
    map->set_null = NULL;
    map->construct = NULL;
    map->destruct = NULL;
    map->contains = NULL;
    map->get = NULL;
    map->insert = NULL;
    map->erase = NULL;
    map->get_const_key_vals = NULL;
    return OK;
}

status_code map_init(Map* map, map_base base)
{
    if (map == NULL)
    {
        return NULL_ARG;
    }
    
    map->ds = NULL;
    
    switch (base)
    {
        case MB_HASHSET:
        case MB_ARR:
        case MB_BST:
        case MB_TRIE:
        {
            map->ds = malloc(sizeof(Trie));
            map->set_null = trie_set_null;
            map->construct = trie_construct;
            map->destruct = trie_destruct;
            map->contains = trie_contains;
            map->get = trie_get;
            map->insert = trie_set;
            map->erase = trie_erase;
            map->get_const_key_vals = trie_get_key_vals;
        }
    }
    if (map->ds == NULL)
    {
        map_set_null(map);
        return BAD_ALLOC;
    }
    return OK;
}

status_code map_destruct(Map* map)
{
    if (map == NULL)
    {
        return NULL_ARG;
    }
    free(map->ds);
    map_set_null(map);
    return OK;
}