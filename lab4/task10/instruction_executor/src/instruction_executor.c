#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "../include/instruction_executor.h"

status_code execute_instruction(const char* instruction, Trie* environment, config_data config)
{
    // Это ваще не лол...
    // Во-первых если это ввод-вывод, то принтим прямо здесь (в выводе всегда одна переменная)
    // Во-вторых, парсим с помощью модификации деградированной сорт станции Дейкстры )0
    // т.е. обычную логику скобок удаляем (ЕЁ НЕТ В ТЗ!) и добавляем логику различного порядка операций
    // *у каждой операции может быть свой порядок
    // *в инструкции не может быть двух пробелов подряд (это ответственность ридера)
    
    return OK;
}