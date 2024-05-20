#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "../include/debugger.h"

status_code handle_value(Trie* environment);
status_code handle_print(Trie* environment);
status_code handle_change(Trie* environment);
status_code handle_insert(Trie* environment);
status_code handle_delete(Trie* environment);;

status_code run_debug_session(Trie* environment)
{
    int running = 1;
    status_code st = OK;
    while (running && !st)
    {
        printf("Enter the command\n");
        char* command = NULL;
        st = read_line(&command, 0);
        if (!st && !strcmp(command, "Stop"))
        {
            running = 0;
            printf("Debug session ended!\n");
        }
        else if (!st && !strcmp(command, "Value"))
        {
            st = handle_value(environment);
        }
        else if (!st && !strcmp(command, "Print all"))
        {
            st = handle_print(environment);
        }
        else if (!st && !strcmp(command, "Change value"))
        {
            st = handle_change(environment);
        }
        else if (!st && !strcmp(command, "Insert"))
        {
            st = handle_insert(environment);
        }
        else if (!st && !strcmp(command, "Delete"))
        {
            st = handle_delete(environment);
        }

        else if (!st && !strcmp(command, "Shut down"))
        {
            st = SHUT_DOWN;
            printf("Shutting down\n");
        }
        else
        {
            printf("Unknown command, try again\n");
        }
        free(command);
    }
    return st;
}


status_code handle_value(Trie* environment)
{
    status_code st = OK;
    printf("Enter name of variable you want to check (0 to exit)\n");
    char* name = NULL;
    int flag = 1;
    while (!st && flag)
    {
        st = read_line(&name, 0);
        if (!strcmp(name, "0"))
        {
            st = USER_INPUT_END;
        }
        if (!st)
        {
            uint32_t value = 0;
            st = trie_get(environment, name, &value);
            if (!st)
            {
                printf("%X\n", value);
                for (int i = sizeof(value) * 8 - 1; i >= 0; i--) {
                    printf("%u ", (value >> i) & 1);
                }
                printf("\n");
                flag = 0;
            }
            if (st == BAD_ACCESS)
            {
                printf("This variable does not exist. Try again\n");
                st = OK;
            }
        }
        if (st == USER_INPUT_END)
        {
            st = OK;
            flag = 0;
        }
        free(name);
    }

    return st;
}

status_code handle_print(Trie* environment)
{
    status_code st = OK;
    ull trie_size = 0;
    trie_key_val* trie_info = NULL;
    st = trie_get_key_vals(environment, &trie_size, &trie_info);
    if (!st)
    {
        if (trie_size == 0)
        {
            printf("No variables are initialized\n");
        }
        for (int i = 0; i < trie_size; ++i)
        {
            printf("%s %u\n", trie_info[i].key, trie_info[i].value);
            free(trie_info[i].key);
        }
        free(trie_info);
    }
    return st;
}

status_code handle_change(Trie* environment)
{
    status_code st = OK;
    printf("Enter name of variable you want to change value of (0 to exit)\n");
    char* name = NULL;
    char* new_value = NULL;
    int flag = 1;
    while (!st && flag)
    {
        st = read_line(&name, 0);
        if (!strcmp(name, "0"))
        {
            st = USER_INPUT_END;
        }
        if (!st)
        {
            uint32_t value = 0;
            st = trie_get(environment, name, &value);
            if (!st)
            {
                printf("You want to change '%s' which value is %u\n", name, value);
                printf("Enter new value in base 16: ");
                while (!st && flag)
                {
                    st = read_line(&new_value, 0);
                    printf("\n");
                    st = st ? st : parse_uint32(new_value, 16, &value);
                    st = st ? st : trie_set(environment, name, value);
                    if (!st)
                    {
                        printf("Success!\n");
                        flag = 0;
                    }
                    if (st == INVALID_INPUT)
                    {
                        printf("Invalid input. Try again\n");
                        st = OK;
                    }
                    free(new_value);
                }
            }
            if (st == BAD_ACCESS)
            {
                printf("Unknown variable! Try again (0 to exit)\n");
                st = OK;
            }
        }
        if (st == USER_INPUT_END)
        {
            st = OK;
            flag = 0;
        }
        free(name);
    }
    return st;
}

status_code handle_insert(Trie* environment)
{
    status_code st = OK;
    printf("Enter name of new variable (0 to exit)\n");
    char* name = NULL;
    char* new_value = NULL;
    int flag = 1;
    while (!st && flag)
    {
        uint32_t value = 0;
        int contains = 0;
        
        st = read_line(&name, 0);
        if (!strcmp(name, "0"))
        {
            st = USER_INPUT_END;
        }
        
        // TODO varname check
        st = st ? st : trie_contains(environment, name, &contains);
        if (contains)
        {
            printf("This variable already exists! Try again (0 to exit)\n");
        }
        else if (!st)
        {
            printf("How you want to initialize variable?\n");
            flag = 1;
            char* type = NULL;
            while (!st && flag)
            {
                printf("For Zeckendorf representation enter: 1\n");
                printf("For Roman representation enter: 2\n");
                st = read_line(&type, 0);
                if (!st && (!strcmp(type, "1") || !strcmp(type, "2")))
                {
                    status_code (*parse)(const char*, uint32_t*) = type[0] == '1'
                            ? parse_zeckendorf_uint32
                            : parse_roman_uint32;
                    
                    printf("Enter %s representation: ", type[0] == '1' ? "Zeckendorf" : "Roman");
                    st = read_line(&new_value, 0);
                    st = st ? st : parse(new_value, &value);
                    if (!st)
                    {
                        printf("You entered %u\n", value);
                        st = trie_insert(environment, name, value);
                        if (!st)
                        {
                            printf("Success!\n");
                            flag = 0;
                        }
                    }
                    free(new_value);
                }
                else if (!st)
                {
                    printf("Wrong type! Try again\n");
                }
                if (st == INVALID_INPUT)
                {
                    printf("Invalid input. Try again\n");
                    st = OK;
                }
                free(type);
            }
        }
        if (st == INVALID_INPUT)
        {
            printf("Invalid variable name. Try again\n");
            st = OK;
        }
        if (st == USER_INPUT_END)
        {
            st = OK;
            flag = 0;
        }
        free(name);
    }
    return st;
}

status_code handle_delete(Trie* environment)
{
    status_code st = OK;
    printf("Enter name of variable you want to delete (0 to exit)\n");
    char* name = NULL;
    int flag = 1;
    while (!st && flag)
    {
        st = read_line(&name, 0);
        if (!strcmp(name, "0"))
        {
            st = USER_INPUT_END;
        }
        if (!st)
        {
            st = trie_erase(environment, name);
            if (!st)
            {
                printf("Successfully deleted!\n");
                flag = 0;
            }
            if (st == BAD_ACCESS)
            {
                printf("Unknown variable. Try again (0 to exit)\n");
                st = OK;
            }
        }
        if (st == USER_INPUT_END)
        {
            st = OK;
            flag = 0;
        }
        free(name);
    }
    return st;
}