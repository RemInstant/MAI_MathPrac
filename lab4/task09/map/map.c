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

status_code map_construct(Map* map, map_base base, size_t (*calc_hash)(const char*))
{
    if (map == NULL || calc_hash == NULL)
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
            
            map->set_null           = (status_code (*)(void*))                                      trie_set_null;
            map->construct          = (status_code (*)(void*, size_t (*)(const char*)))             trie_construct;
            map->destruct           = (status_code (*)(void*))                                      trie_destruct;
            map->contains           = (status_code (*)(void*, const char*, int* is_contained))      trie_contains;
            map->get                = (status_code (*)(void*, const char*, Department** dep))       trie_get;
            map->insert             = (status_code (*)(void*, const char*, const Department* dep))  trie_set;
            map->erase              = (status_code (*)(void*, const char*))                         trie_erase;
            map->get_const_key_vals = (status_code (*)(void*, size_t*, pair_str_dep**))             trie_get_key_vals;
        }
    }
    
    map->set_null(map->ds);
    status_code code = map->construct(map->ds, calc_hash);
    
    if (code)
    {
        free(map->ds);
        map_set_null(map);
        return code;
    }
    
    return OK;
}

status_code map_destruct(Map* map)
{
    if (map == NULL)
    {
        return OK;
    }
    if (map->ds != NULL)
    {
        map->destruct(map->ds);
        free(map->ds);
    }
    map_set_null(map);
    return OK;
}


status_code map_contains(Map* map, const char* key, int* is_contained)
{
    if (map == NULL || key == NULL || is_contained == NULL)
    {
        return NULL_ARG;
    }
    return map->contains(map->ds, key, is_contained);
}

status_code map_get(Map* map, const char* key, Department** dep)
{
    if (map == NULL || key == NULL || dep == NULL)
    {
        return NULL_ARG;
    }
    return map->get(map->ds, key, dep);
}

status_code map_insert(Map* map, const char* key, Department* dep)
{
    if (map == NULL || key == NULL || dep == NULL)
    {
        return NULL_ARG;
    }
    return map->insert(map->ds, key, dep);
}

status_code map_erase(Map* map, const char* key)
{
    if (map == NULL || key == NULL)
    {
        return NULL_ARG;
    }
    return map->erase(map->ds, key);
}

status_code map_get_const_key_vals(Map* map, size_t* dict_size, pair_str_dep** dict)
{
    if (map == NULL || dict_size == NULL || dict == NULL)
    {
        return NULL_ARG;
    }
    return map->get_const_key_vals(map->ds, dict_size, dict);
}
