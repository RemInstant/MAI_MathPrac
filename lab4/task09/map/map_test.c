#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../utility.h"
#include "map.h"

#include "trie.h"

int main()
{
    for (map_base base = MB_HASHSET; base <= MB_TRIE; ++base)
    {
        if (base == MB_HASHSET) continue;
        if (base == MB_ARR) continue;
        if (base == MB_BST) continue;
        
        Map map;
        Department* dep;
        Department* dep1;
        char* key;
        int is_contained;
        size_t dict_size;
        pair_str_dep* dict;
        
        map_set_null(&map);
        
        assert(map_init(&map, base) == OK);
        
        assert(map.set_null(map.ds) == OK);
        
        // Empty map
        {
            assert(map.destruct(map.ds) == OK);
            assert(map.construct(map.ds, calc_default_str_hash) == OK);
            assert(map.destruct(map.ds) == OK);
            assert(map.destruct(map.ds) == OK);
        }
        // test 1
        {
            assert(map.construct(map.ds, calc_default_str_hash) == OK);
            
            key = (char*) malloc(sizeof(char) * 3);
            assert(key != NULL);
            key[2] = '\0';
            
            for (size_t i = 0; i < 10; ++i)
            {
                dep = (Department*) malloc(sizeof(Department));
                dep1 = NULL;
                assert(dep != NULL);
                assert(department_construct(dep, i, PQB_BINOM, 2.0, 1e-9, 1, 1, compare_request) == OK);
                
                key[0] = '0' + (i % 10);
                key[1] = '0' + ((i*5 + 3) % 10);
                
                assert(map.contains(map.ds, key, &is_contained) == OK);
                assert(is_contained == 0);
                assert(map.insert(map.ds, key, dep) == OK);
                assert(map.contains(map.ds, key, &is_contained) == OK);
                assert(is_contained == 1);
                assert(map.get(map.ds, key, &dep1) == OK);
                assert(dep == dep1);
            }
            
            assert(map.get_const_key_vals(map.ds, &dict_size, &dict) == OK);
            assert(dict != NULL);
            assert(dict_size == 10);
            
            for (size_t i = 0; i < dict_size; ++i)
            {
                int a = dict[i].str[0] - '0';
                assert(dict[i].str[1] == '0' + ((a*5 + 3) % 10));
                
                strcpy(key, dict[i].str);
                assert(map.contains(map.ds, key, &is_contained) == OK);
                assert(is_contained == 1);
                assert(map.get(map.ds, key, &dep) == OK);
                assert(dict[i].dep = dep);
            }
            
            for (size_t i = 0; i < dict_size; ++i)
            {
                free(dict[i].str);
            }
            free(dict);
            
            for (size_t i = 0; i < 10; ++i)
            {
                key[0] = '0' + (i % 10);
                key[1] = '0' + ((i*5 + 3) % 10);
                
                assert(map.contains(map.ds, key, &is_contained) == OK);
                assert(is_contained == 1);
                assert(map.get(map.ds, key, &dep) == OK);
                assert(dep->staff_size == i);
                
                assert(map.erase(map.ds, key) == OK);
                assert(map.contains(map.ds, key, &is_contained) == OK);
                assert(is_contained == 0);
                assert(department_destruct(dep) == OK);
            }
            
            free(key);
            assert(map.destruct(map.ds) == OK);
        }
        // test 2
        {
            assert(map.construct(map.ds, calc_default_str_hash) == OK);
            
            key = (char*) malloc(sizeof(char) * 9);
            assert(key != NULL);
            key[8] = '\0';
            
            for (size_t i = 0; i < 100; ++i)
            {
                dep = (Department*) malloc(sizeof(Department));
                dep1 = NULL;
                assert(dep != NULL);
                assert(department_construct(dep, i+5, PQB_BINOM, 2.0, 1e-9, 1, 1, compare_request) == OK);
                
                key[0] = '0' + ((3*i + 17) % 10);
                key[1] = '0' + ((5*i + 13) % 10);
                key[2] = '0' + ((i*(i+3) + 2) % 10);
                key[3] = '0' + ((i*(i+2)*(i+5) + 17) % 10);
                key[4] = 'A' + ((3*i + 3) % 26);
                key[5] = 'A' + ((17*i + 5) % 26);
                key[6] = 'A' + ((53*i + 7) % 26);
                key[7] = 'A' + ((97*i + 2) % 26);
                
                assert(map.contains(map.ds, key, &is_contained) == OK);
                assert(is_contained == 0);
                assert(map.insert(map.ds, key, dep) == OK);
                assert(map.contains(map.ds, key, &is_contained) == OK);
                assert(is_contained == 1);
                assert(map.get(map.ds, key, &dep1) == OK);
                assert(dep == dep1);
            }
            
            for (size_t i = 0; i < 100; ++i)
            {
                key[0] = '0' + ((3*i + 17) % 10);
                key[1] = '0' + ((5*i + 13) % 10);
                key[2] = '0' + ((i*(i+3) + 2) % 10);
                key[3] = '0' + ((i*(i+2)*(i+5) + 17) % 10);
                key[4] = 'A' + ((3*i + 3) % 26);
                key[5] = 'A' + ((17*i + 5) % 26);
                key[6] = 'A' + ((53*i + 7) % 26);
                key[7] = 'A' + ((97*i + 2) % 26);
                
                assert(map.contains(map.ds, key, &is_contained) == OK);
                assert(is_contained == 1);
                assert(map.get(map.ds, key, &dep) == OK);
                assert(dep->staff_size == i+5);
                
                assert(map.erase(map.ds, key) == OK);
                assert(map.contains(map.ds, key, &is_contained) == OK);
                assert(is_contained == 0);
                assert(department_destruct(dep) == OK);
            }
            
            free(key);
            assert(map.destruct(map.ds) == OK);
        }
        // test 3
        {
            assert(map.construct(map.ds, calc_default_str_hash) == OK);
            
            key = (char*) malloc(sizeof(char) * 2);
            assert(key != NULL);
            strcpy(key, "A");
            
            dep = (Department*) malloc(sizeof(Department));
            assert(dep != NULL);
            assert(department_construct(dep, 2, PQB_BINOM, 2.0, 1e-9, 1, 1, compare_request) == OK);
            assert(map.insert(map.ds, key, dep) == OK);
            
            dep = (Department*) malloc(sizeof(Department));
            assert(dep != NULL);
            assert(department_construct(dep, 2, PQB_BINOM, 2.0, 1e-9, 1, 1, compare_request) == OK);
            assert(map.insert(map.ds, key, dep) != OK);
            assert(department_destruct(dep) == OK);
            
            for (size_t i = 1; i < 20; ++i)
            {
                key[0] = 'A' + i;
                dep = (Department*) malloc(sizeof(Department));
                assert(dep != NULL);
                assert(department_construct(dep, 2, PQB_BINOM, 2.0, 1e-9, 1, 1, compare_request) == OK);
                assert(map.insert(map.ds, key, dep) == OK);
            }
            
            free(key);
            assert(map.destruct(map.ds) == OK);
        }
        
        assert(map_destruct(&map) == OK);
        
        if (base == MB_HASHSET) 	printf("Hash set passed the tests\n");
        if (base == MB_ARR) 		printf("Dynamic array passed the tests\n");
        if (base == MB_BST) 		printf("Binary search tree passed the tests\n");
        if (base == MB_TRIE) 		printf("Trie passed the tests\n");
    }
    printf("All tests have been passed\n");
}