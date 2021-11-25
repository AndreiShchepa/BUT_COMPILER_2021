#include "code_generator.h"
#include "symstack.h"
#include <stdlib.h>

/******************************************************************************
 *                                  TODO
 *****************************************************************************/
// TODO - check vars, params, if, while numbering - tests - redefinition
// TODO - change if in func PRINT_FUNC else PRINT_MAIN
// TODO - format params in func
// TODO - not printing write("\n") - print only string@_  (empty string)
// TODO - ascii spaces in strings (32)
// TODO - clear stack
// TODO - tointeger => return nil or exit 8 if arg nil?
// TODO - string convert to ascii
// TODO - muze byt v substr pouzita funkce clear
// TODO - substr, ord, chr - i <= 0 is right???


/******************************************************************************
 *                                  CONVENTIONS
 *****************************************************************************/
// OUR:
// funcs    = label $funcname
// var      = $func$poradie_if$if$poradie_if_else_v_if$else_if$

// THEIR:
// funcs    = label &funcname



/******************************************************************************
  *									GLOBAL VARS
******************************************************************************/
string_t ifj_code[BLOCKS_NUM];
extern Queue* queue_id;
extern Queue* queue_expr;
extern int err;
extern arr_symtbs_t local_symtbs;
cnts_t cnt;

/******************************************************************************
  *									FUNCTIONS
******************************************************************************/
bool alloc_ifj_code() {
    if (!str_init(&ifj_code[MAIN]     , IFJ_CODE_START_LEN) ||
        !str_init(&ifj_code[FUNCTIONS], IFJ_CODE_START_LEN)) {
        return false;
    }
    return true;
}

bool init_ifj_code() {
    str_clear(&ifj_code[MAIN]);
    str_clear(&ifj_code[FUNCTIONS]);
    return true;
}

bool alloc_cnt() {
    if (!str_init(&cnt.func_name, IFJ_CODE_START_LEN) ||
        !str_init(&cnt.func_call, IFJ_CODE_START_LEN)) {
        return false;
    }
    return true;
}

bool init_cnt() {
    str_clear(&cnt.func_name);
    str_clear(&cnt.func_call);

    cnt.param_cnt = 0;
    cnt.if_cnt    = 0;
    cnt.while_cnt = 0;
    cnt.deep      = 0;
    return true;
}

bool gen_file_start() {
    return true;
}

bool gen_int2char(Queue *queue, token_t *symb_1) {
    (void )queue;
    (void )symb_1;
//    PRINT_FUNC(0, "INT2CHAR LF@%s LF@%s", queue->front->id->key_id, symb_1->attr.id, NULL);
    return true;
}

bool gen_func_label() {
    return true;
}


bool gen_init_built_ins() {
#if 0
    PRINT_FUNC(1, "%s", FUNC_TOINTEGER);
    PRINT_FUNC(6, "%s", FUNC_SUBSTR);
    PRINT_FUNC(7, "%s", FUNC_ORD);
    PRINT_FUNC(8, "%s", FUNC_CHR);
    PRINT_FUNC(10, "%s", FUNC_RETYPING_VAR1);
    PRINT_FUNC(11, "%s", FUNC_RETYPING_VAR2);
    PRINT_FUNC(12, "%s", FUNC_CHECK_OP);
    PRINT_FUNC(13, "%s", FUNC_CHECK_COMP);
    PRINT_FUNC(14, "%s", FUNC_OP_NIL);
#endif
    DEBUG_PRINT_INSTR(1, FUNCTIONS, EOL EOL DEVIDER"BUILT-IN FUNCTIONS" NON_VAR , EMPTY_STR);
    DEBUG_PRINT_INSTR(1, FUNCTIONS, DEVIDER_2"BUILT-IN FUNCTIONS" NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(2, "%s", FUNC_READI); DEBUG_PRINT_INSTR(3, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(3, "%s", FUNC_READN); DEBUG_PRINT_INSTR(3, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(4, "%s", FUNC_READS); DEBUG_PRINT_INSTR(3, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(5, "%s", FUNC_WRITE); DEBUG_PRINT_INSTR(3, FUNCTIONS, NON_VAR , EMPTY_STR);

    PRINT_FUNC_BUILT_IN(15, "%s", FUNC_RETYPING_VAR1);
    PRINT_FUNC_BUILT_IN(17, "%s", FUNC_RETYPING_VAR2);
    PRINT_FUNC_BUILT_IN(18, "%s", FUNC_CHECK_OP);
    PRINT_FUNC_BUILT_IN(19, "%s", FUNC_CHECK_COMP);
    PRINT_FUNC_BUILT_IN(20, "%s", FUNC_OP_NIL);
    return true;
}

bool gen_label_item() {
    return true;
}

bool gen_while_label(char *key_id) {
    cnt.while_cnt++;
    DEBUG_PRINT_INSTR(1, FUNCTIONS, EOL DEVIDER_2"while" NON_VAR , EMPTY_STR);
    PRINT_FUNC(2, "label $%s$%d$while$" , key_id, cnt.while_cnt);
	return true;
}

bool gen_while_eval() {
    PRINT_FUNC(1, "pops GF@&var1" NON_VAR , EMPTY_STR);
    PRINT_FUNC(2, "jumpifeq $%s$%d$while_end$ GF@&var1 bool@false" , cnt.func_name.str, cnt.while_cnt);
	return true;
}

bool gen_while_end() {
    PRINT_FUNC(1, "jump $%s$%d$while$" , cnt.func_name.str, cnt.while_cnt);
    PRINT_FUNC(2, "label $%s$%d$while_end$" , cnt.func_name.str, cnt.while_cnt);
    DEBUG_PRINT_INSTR(3, FUNCTIONS, NON_VAR , EMPTY_STR);
    //cnt.while_cnt--; //todo this is wrong, -- never should be anywhere, only cnt.while == 0 after new func.
    return true;
}

bool gen_params() {
    DEBUG_PRINT_INSTR(1, FUNCTIONS, EOL DEVIDER_2"params" NON_VAR , EMPTY_STR);

    QueueElementPtr *queue_elem = queue_id->front;
    for (int i = 0; queue_elem; queue_elem = queue_elem->previous_element, i++) {
        PRINT_FUNC(2, "defvar "FORMAT_VAR             , cnt.func_name.str, queue_id->rear->id->deep, queue_elem->id->key_id);
        PRINT_FUNC(3, "move   "FORMAT_VAR FORMAT_PARAM, cnt.func_name.str, queue_id->rear->id->deep, queue_elem->id->key_id, i);
    }
    DEBUG_PRINT_INSTR(3, FUNCTIONS, EOL DEVIDER_2"logic"NON_VAR , EMPTY_STR);
    // TODO - remove one by one
    queue_dispose(queue_id);
    return true;
}

bool gen_param() {
    return true;
}

bool gen_def_var() {
    PRINT_FUNC(1, "defvar " FORMAT_VAR , cnt.func_name.str, queue_id->front->id->deep, queue_id->front->id->key_id);
    PRINT_FUNC(1, "move   " FORMAT_VAR " nil@nil" , cnt.func_name.str, queue_id->front->id->deep, queue_id->front->id->key_id);

    return true;
}

bool gen_init_var() {
    PRINT_FUNC(1, "pops GF@&var1" NON_VAR , EMPTY_STR);
    PRINT_FUNC(1, "move " FORMAT_VAR " GF@&var1" , cnt.func_name.str, queue_id->rear->id->deep, queue_id->rear->id->key_id);
    queue_remove_rear(queue_id);
    return true;
}

bool gen_if_start() {
    PRINT_FUNC(1, "label " FORMAT_IF , cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_if_else() {
    PRINT_FUNC(2, "label " FORMAT_ELSE , cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_if_end(/*TODO*/) {
    PRINT_FUNC(3, "label " FORMAT_IF_END , cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_if_eval() {
    cnt.if_cnt++; // because this instruction is printed first
    PRINT_FUNC(1, "pops GF@&var1" NON_VAR , EMPTY_STR);
    PRINT_FUNC(2, "jumpifeq $%s$%d$else$ GF@&var1 bool@false" , cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_if_end_jump() {
    PRINT_FUNC(2, "jump $%s$%d$if_end$" , cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_func_start(char *id) {
    DEBUG_PRINT_INSTR(1, FUNCTIONS,	EOL DEVIDER NON_VAR, EMPTY_STR);
    PRINT_FUNC(2, "label &%s"          , id);
    PRINT_FUNC(3, "pushframe"  NON_VAR , EMPTY_STR);
    PRINT_FUNC(4, "createframe"NON_VAR , EMPTY_STR);
    return true;
}

bool gen_func_end() {
    DEBUG_PRINT_INSTR(1, FUNCTIONS, EOL DEVIDER_2"end" 	NON_VAR, EMPTY_STR);
    PRINT_FUNC(2,   "popframe" 	        NON_VAR,  EMPTY_STR);
    PRINT_FUNC(3,   "return"   	        NON_VAR,  EMPTY_STR);
    DEBUG_PRINT_INSTR(3, FUNCTIONS,	DEVIDER NON_VAR, EMPTY_STR);
    return true;
}

int where_to_print() {
    return (strcmp(cnt.func_name.str, "") == 0) ? MAIN : FUNCTIONS;
}

bool is_write() {
    return (strcmp(cnt.func_call.str, "write") == 0) ? true : false;
}

bool gen_func_call_start() {
    DEBUG_PRINT_INSTR(1, where_to_print(), EOL DEVIDER_2"call_func" NON_VAR , EMPTY_STR);
    PRINT_WHERE(1, "createframe" NON_VAR, EMPTY_STR);
    return true;
}

bool gen_func_call_write() {
    return true;
}

bool gen_func_call_args_var(htab_item_t *htab_item) {
    PRINT_WHERE(1, "defvar TF@%%%dp"       , cnt.param_cnt);
    PRINT_WHERE(2, "move   TF@%%%dp LF@$%s$%lu$%s$" , cnt.param_cnt, cnt.func_name.str, htab_item->deep, htab_item->key_id);

    if (!is_write()) {
        cnt.param_cnt++;
    }
	return true;
}


bool convert_str_to_ascii(string_t *str_in) {
    string_t str_out;
    CODE_GEN(str_init, &str_out, strlen(str_in->str)*5);

    int idx = 0;
    char input_char = str_in->str[idx];
    char tmp_str[3];

    while(input_char != '\0') {
        switch (input_char) {
            SWITCH_CASE(0);
            SWITCH_CASE(1);
            SWITCH_CASE(2);
            SWITCH_CASE(3);
            SWITCH_CASE(4);
            SWITCH_CASE(5);
            SWITCH_CASE(6);
            SWITCH_CASE(7);
            SWITCH_CASE(8);
            SWITCH_CASE(9);
            SWITCH_CASE(10);
            SWITCH_CASE(11);
            SWITCH_CASE(12);
            SWITCH_CASE(13);
            SWITCH_CASE(14);
            SWITCH_CASE(15);
            SWITCH_CASE(16);
            SWITCH_CASE(17);
            SWITCH_CASE(18);
            SWITCH_CASE(19);
            SWITCH_CASE(20);
            SWITCH_CASE(21);
            SWITCH_CASE(22);
            SWITCH_CASE(23);
            SWITCH_CASE(24);
            SWITCH_CASE(25);
            SWITCH_CASE(26);
            SWITCH_CASE(27);
            SWITCH_CASE(28);
            SWITCH_CASE(29);
            SWITCH_CASE(30);
            SWITCH_CASE(31);
            SWITCH_CASE(32);

            SWITCH_CASE(35);
            SWITCH_CASE(96);

            default:
                CODE_GEN(str_add_char, &str_out, input_char);
                break;
        }
        idx++;
        input_char = str_in->str[idx];
    }
    free(str_in->str);
    str_in->str = str_out.str;
    str_in->length = str_out.length;
    str_in->alloc_size = str_out.alloc_size;
    return true;
}


bool gen_func_call_args_const(token_t *token) {
    PRINT_WHERE(1, "defvar TF@%%%dp" , cnt.param_cnt);
    switch(token->type) {
        case (T_INT)	: PRINT_WHERE(2, "move "  FORMAT_PARAM " int@%llu" , cnt.param_cnt, (llu_t)token->attr.num_i);  break;
        case (T_FLOAT)	: PRINT_WHERE(2, "move "  FORMAT_PARAM " float@%a" , cnt.param_cnt, token->attr.num_f);         break;
        case (T_STRING)	: convert_str_to_ascii(&token->attr.id);
                          PRINT_WHERE(2, "move "  FORMAT_PARAM " string@%s", cnt.param_cnt, token->attr.id.str);        break;
        default       	: PRINT_WHERE(2, "move "  FORMAT_PARAM " nil@nil"  , cnt.param_cnt);                            break;
    }

    if (strcmp(cnt.func_call.str, "write") != 0)
        cnt.param_cnt++;
    return true;
}

bool gen_func_call_label() {
    if (strcmp(cnt.func_call.str, "write") == 0) {
        PRINT_WHERE(1, "call &write" NON_VAR, EMPTY_STR);
        return true;
    }

    PRINT_WHERE(1, "call &%s" , queue_id->rear->id->key_id);
    queue_remove_rear(queue_id);
	return true;
}

bool gen_init() {
    if (!alloc_ifj_code()   ||
        !init_ifj_code()    ||
        !alloc_cnt()        ||
        !init_cnt()) {
        return ((err = INTERNAL_ERR) == NO_ERR);
    }
    PRINT_FUNC(1, ".IFJcode21" NON_VAR, EMPTY_STR);
    PRINT_FUNC(2,   "defvar GF@&type1"  NON_VAR , EMPTY_STR);
    PRINT_FUNC(3,   "defvar GF@&type2"  NON_VAR , EMPTY_STR);
    PRINT_FUNC(4,   "defvar GF@&var1"   NON_VAR , EMPTY_STR);
    PRINT_FUNC(5,   "defvar GF@&var2"   NON_VAR , EMPTY_STR);
    PRINT_FUNC(6, "jump $main" NON_VAR, EMPTY_STR);
    DEBUG_PRINT_INSTR(20, MAIN, EOL DEVIDER NON_VAR , EMPTY_STR);
    DEBUG_PRINT_INSTR(21, MAIN, DEVIDER_2"MAIN LABEL" NON_VAR , EMPTY_STR);
    PRINT_MAIN(7,   "label $main"       NON_VAR , EMPTY_STR);
    PRINT_MAIN(8,   "createframe"       NON_VAR , EMPTY_STR);
    PRINT_MAIN(9,   "pushframe"         NON_VAR , EMPTY_STR);
    PRINT_MAIN(10,  "createframe"       NON_VAR , EMPTY_STR);

    if (!gen_init_built_ins()) {
        return ((err = INTERNAL_ERR) == NO_ERR);
    }

    DEBUG_PRINT_INSTR(25, FUNCTIONS, EOL DEVIDER NON_VAR , EMPTY_STR); // above will "#" from before call
    DEBUG_PRINT_INSTR(20, FUNCTIONS, NON_VAR , EMPTY_STR);

    //str_free(&cnt.func_name);
    return (err == NO_ERR);
}

bool gen_retval_nil() {
    PRINT_FUNC(1, "pushs nil@nil " NON_VAR, EMPTY_STR);
    return true;
}

bool gen_testing_helper() {
    fprintf(stdout, "%s", ifj_code[FUNCTIONS].str);
    fprintf(stdout, "%s", ifj_code[MAIN].str);
    return true;
}

bool gen_expression() {
    htab_item_t *tmp;
    while (!queue_isEmpty(queue_expr)) {
        switch (queue_expr->front->token->type) {
            case T_ID:
                tmp = find_id_symtbs(&local_symtbs, queue_expr->front->token->attr.id.str);
                if(!tmp){
                    err = INTERNAL_ERR;
                    return false;
                }
                PRINT_FUNC(1, "pushs LF@$%s$%llu$%s$" , cnt.func_name.str, (llu_t)tmp->deep, queue_expr->front->token->attr.id.str);
                str_free(&queue_expr->front->token->attr.id);
                break;
            case T_INT:
                PRINT_FUNC(2, "pushs int@%llu" , (llu_t)queue_expr->front->token->attr.num_i); //ubuntu chce lu
                break;
            case T_FLOAT:
                // todo format (asi je treba skontrolovat)
                PRINT_FUNC(3, "pushs float@%a" , queue_expr->front->token->attr.num_f);
                break;
            case T_STRING:
                convert_str_to_ascii(&queue_expr->front->token->attr.id);
                PRINT_FUNC(4, "pushs string@%s" , queue_expr->front->token->attr.id.str);
                break;
            case T_KEYWORD:
                if(queue_expr->front->token->keyword == KW_NIL){
                    PRINT_FUNC(5, "pushs nil@nil" NON_VAR , EMPTY_STR);
                } else {
                    err = PARSER_ERR;
                    return false;
                }
                break;
            case T_PLUS:
                PRINT_FUNC(6, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(7, "adds" NON_VAR , EMPTY_STR);
                break;
            case T_MINUS:
                PRINT_FUNC(8, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(9, "subs" NON_VAR , EMPTY_STR);
                break;
            case T_MUL:
                PRINT_FUNC(10, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(11, "muls" NON_VAR , EMPTY_STR);
                break;
            case T_DIV:
                PRINT_FUNC(12, "call $check_div" NON_VAR , EMPTY_STR);
                PRINT_FUNC(13, "divs" NON_VAR , EMPTY_STR);
                break;
            case T_DIV_INT:
                PRINT_FUNC(14, "call $check_div" NON_VAR , EMPTY_STR);
                PRINT_FUNC(15, "idivs" NON_VAR , EMPTY_STR);
                break;
            case T_LT:
                PRINT_FUNC(16, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(17, "lts" NON_VAR , EMPTY_STR);
                break;
            case T_GT:
                PRINT_FUNC(18, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(19, "gts" NON_VAR , EMPTY_STR);
                break;
            case T_LE:
                PRINT_FUNC(20, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(21, "pops GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(22, "pops GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(23, "pushs GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(24, "pushs GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(25, "lts" NON_VAR , EMPTY_STR);
                PRINT_FUNC(26, "pushs GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(27, "pushs GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(28, "eqs" NON_VAR , EMPTY_STR);

                PRINT_FUNC(29, "ors" NON_VAR , EMPTY_STR);
                break;
            case T_GE:
                PRINT_FUNC(30, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(31, "pops GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(32, "pops GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(33, "pushs GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(34, "pushs GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(35, "gts" NON_VAR , EMPTY_STR);
                PRINT_FUNC(36, "pushs GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(37, "pushs GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(38, "neqs" NON_VAR , EMPTY_STR);
                PRINT_FUNC(39, "ors" NON_VAR , EMPTY_STR);
                break;
            case T_EQ:
                PRINT_FUNC(40, "call $check_comp" NON_VAR , EMPTY_STR);
                PRINT_FUNC(41, "eqs" NON_VAR , EMPTY_STR);
                break;
            case T_NEQ:
                PRINT_FUNC(42, "call $check_comp" NON_VAR , EMPTY_STR);
                PRINT_FUNC(43, "eqs" NON_VAR , EMPTY_STR);
                PRINT_FUNC(44, "nots" NON_VAR , EMPTY_STR);
                break;
            case T_LENGTH:
                PRINT_FUNC(45, "pops GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(46, "strlen GF@&var1 GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(47, "pushs GF@&var1" NON_VAR , EMPTY_STR);
                break;
            case T_CONCAT:
                PRINT_FUNC(48, "pops GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(49, "pops GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(50, "concat GF@&var1 GF@&var1 GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(51, "pushs GF@&var1" NON_VAR , EMPTY_STR);
                break;
            default:
                break;
        }
        free(queue_expr->front->token);
        queue_remove_front(queue_expr);
    }
    return true;
}

bool dealloc_gen_var() {
    str_free(&cnt.func_name);
    str_free(&cnt.func_call);
    str_free(&ifj_code[MAIN]);
    str_free(&ifj_code[FUNCTIONS]);
    return true;
}

