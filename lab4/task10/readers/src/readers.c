#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "../include/readers.h"

status_code read_config(FILE* file, config_data* config)
{
    if (file == NULL || config == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    
    while (!code)
    {
        char* line = NULL;
        char* first_word = NULL;
        char* second_word = NULL;
        
        code = code ? code : fread_line_with_comments(file, &line, 1, '#', '[', ']');
        
        const char* iter = line;
        
        code = code ? code : sread_until(iter, " ", 0, &iter, &first_word);
        code = code ? code : sread_while(iter, " ", &iter, NULL);
        
        if (!code && *iter != '\0')
        {
            code = code ? code : sread_until(iter, " ", 0, &iter, &second_word);
            code = code ? code : sread_while(iter, " ", &iter, NULL);
            
            if (!code && *iter != '\0')
            {
                code = FILE_INVALID_CONFIG;
            }
        }
        
        // TODO: set default config
        
        if (!code)
        {
            if (second_word == NULL)
            {
                // if (!strcmp(first_word, "left="))
                // {
                //     config->assign = ASSIGN_LEFT;
                // }
                // else if (!strcmp(first_word, "right="))
                // {
                //     config->assign = ASSIGN_RIGHT;
                // }
                // else if (!strcmp(first_word, "unary()"))
                // {
                //     config->unary = UNARY_LEFT;
                // }
                // else if (!strcmp(first_word, "()unary"))
                // {
                //     config->unary = UNARY_RIGHT;
                // }
                // else if (!strcmp(first_word, "binary()"))
                // {
                //     config->binary = BINARY_PREFIX;
                // }
                // else if (!strcmp(first_word, "(binary)"))
                // {
                //     config->binary = BINARY_INFIX;
                // }
                // else if (!strcmp(first_word, "()binary"))
                // {
                //     config->binary = BINARY_POSTFIX;
                // }
                // else
                // {
                //     code = FILE_INVALID_CONFIG;
                // }
            }
            else
            {
                
            }
        }
    }
}

status_code read_instruction(FILE* file, char** instruction)
{
    // TODO
}