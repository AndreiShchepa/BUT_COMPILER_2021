
#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <string.h>
#include <stdbool.h>
#include "error.h"
#include "str.h"
#include "queue.h"
#include "symtable.h"

typedef struct cnts_s {
    string_t func_name;
    unsigned int param_cnt;
    unsigned int if_cnt;
    unsigned int while_cnt;
    unsigned int deep;
} cnts_t;

bool gen_init_built_int();

bool gen_while_start();
bool gen_while_end();

bool gen_param(htab_item_t *htab_item);

bool gen_if_start();
bool gen_if_else(/*TODO*/);
bool gen_if_end();

bool gen_func_start(char *id);
bool gen_func_end();

bool code_gen_print_ifj_code21();

bool gen_expression();

bool gen_init();
bool code_gen();

void gen_expression();
#endif // CODE_GENERATOR_H
