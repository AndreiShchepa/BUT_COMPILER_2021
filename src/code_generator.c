/**
 * Project: Compiler IFJ21
 *
 * @file code_generator.c
 *
 * @brief Implement generation code of IFJcode21
 *
 * @author  Andrej Binovsky     <xbinov00>
 *          Zdenek Lapes        <xlapes02>
 */

#include "code_generator.h"
#include "symstack.h"
#include <stdlib.h>

/******************************************************************************
  *									GLOBAL VARS
******************************************************************************/
string_t ifj_code[BLOCKS_NUM];
Queue *queue_if;
Queue *queue_while;
extern Queue* queue_id;
extern Queue* queue_expr;
extern int err;
extern arr_symtbs_t local_symtbs;
cnts_t cnt;

/******************************************************************************
  *									FUNCTIONS
******************************************************************************/
// print final ifjcode to stdout
bool gen_testing_helper() {
    PRINT_MAIN(1,   "exit int@0"       NON_VAR , EMPTY_STR);
    fprintf(stdout, "%s", ifj_code[FUNCTIONS].str);
    fprintf(stdout, "%s", ifj_code[MAIN].str);
    return true;
}

// allocate strings for final ifj_code
bool alloc_ifj_code() {
    if (!str_init(&ifj_code[MAIN]     , IFJ_CODE_START_LEN) ||
        !str_init(&ifj_code[FUNCTIONS], IFJ_CODE_START_LEN) ||
        !str_init(&ifj_code[WHILE]    , IFJ_CODE_START_LEN)) {
        CHECK_INTERNAL_ERR(true, false);
    }
    return true;
}

bool init_ifj_code() {
    str_clear(&ifj_code[MAIN]);
    str_clear(&ifj_code[FUNCTIONS]);
    str_clear(&ifj_code[WHILE]);
    return true;
}

// allocate helper structure
bool alloc_cnt() {
    if (!str_init(&cnt.func_name, IFJ_CODE_START_LEN) ||
        !str_init(&cnt.func_call, IFJ_CODE_START_LEN)) {
        CHECK_INTERNAL_ERR(true, false);
    }
    return true;
}

bool init_cnt() {
    str_clear(&cnt.func_name);
    str_clear(&cnt.func_call);

    cnt.param_cnt       = 0;
    cnt.if_cnt_max      = 0;
    cnt.while_cnt       = 0;
    cnt.while_cnt_max   = 0;
    cnt.deep            = 0;
    cnt.instr           = 0;
    cnt.in_return       = false;
    cnt.in_while        = false;
    return true;
}

bool gen_init() {
    // allocate all needed strings need for code generating
    // if error return false set err to INTERNAL ERROR
    if (!alloc_ifj_code()   ||
        !init_ifj_code()    ||
        !alloc_cnt()        ||
        !init_cnt()) {
        CHECK_INTERNAL_ERR(true, false);
    }
    queue_if = queue_init();
    queue_while = queue_init();
    if(!(queue_if) && !(queue_while)){
        CHECK_INTERNAL_ERR(true, false);
    }

    // generate header and declare the exclusive global vars in whole program
    PRINT_FUNC(1, ".IFJcode21" NON_VAR, EMPTY_STR);
    PRINT_FUNC(2,   "defvar GF@&type1"  NON_VAR , EMPTY_STR);
    PRINT_FUNC(3,   "defvar GF@&type2"  NON_VAR , EMPTY_STR);
    PRINT_FUNC(4,   "defvar GF@&var1"   NON_VAR , EMPTY_STR);
    PRINT_FUNC(5,   "defvar GF@&var2"   NON_VAR , EMPTY_STR);
    PRINT_FUNC(6, "jump $$main" NON_VAR, EMPTY_STR);
    DEBUG_PRINT_INSTR(20, MAIN, EOL DEVIDER NON_VAR , EMPTY_STR);
    DEBUG_PRINT_INSTR(21, MAIN, DEVIDER_2"MAIN LABEL" NON_VAR , EMPTY_STR);

    // generate main body of final code
    PRINT_MAIN(7,   "label $$main"       NON_VAR , EMPTY_STR);
    PRINT_MAIN(8,   "createframe"       NON_VAR , EMPTY_STR);
    PRINT_MAIN(9,   "pushframe"         NON_VAR , EMPTY_STR);
    PRINT_MAIN(10,  "createframe"       NON_VAR , EMPTY_STR);

    if (!gen_init_built_ins()) {
        CHECK_INTERNAL_ERR(true, false);
    }

    // only for better debug a structuring final code
    DEBUG_PRINT_INSTR(26, FUNCTIONS, NON_VAR , EMPTY_STR); // above will "#" from before call
    DEBUG_PRINT_INSTR(27, FUNCTIONS, NON_VAR , EMPTY_STR); // above will "#" from before call
    DEBUG_PRINT_INSTR(28, FUNCTIONS, NON_VAR , EMPTY_STR); // above will "#" from before call
    DEBUG_PRINT_INSTR(29, FUNCTIONS, NON_VAR , EMPTY_STR); // above will "#" from before call
    DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR); // above will "#" from before call
    DEBUG_PRINT_INSTR(31, FUNCTIONS, NON_VAR , EMPTY_STR); // above will "#" from before call
    DEBUG_PRINT_INSTR(32, FUNCTIONS, DEVIDER NON_VAR , EMPTY_STR); // above will "#" from before call
    DEBUG_PRINT_INSTR(33, FUNCTIONS, DEVIDER NON_VAR , EMPTY_STR); // above will "#" from before call
    DEBUG_PRINT_INSTR(34, FUNCTIONS, DEVIDER NON_VAR , EMPTY_STR); // above will "#" from before call
    DEBUG_PRINT_INSTR(35, FUNCTIONS, DEVIDER_2"FUNCTIONS" NON_VAR , EMPTY_STR); // above will "#" from before call
    //str_free(&cnt.func_name);
    return (err == NO_ERR);
}

bool gen_init_built_ins() {
#if DEBUG_BUILT_IN // code generating can be disabled
    // code for better debug
    DEBUG_PRINT_INSTR  (1 , FUNCTIONS, EOL EOL DEVIDER NON_VAR , EMPTY_STR);
    DEBUG_PRINT_INSTR  (2 , FUNCTIONS, DEVIDER NON_VAR , EMPTY_STR);
    DEBUG_PRINT_INSTR  (3 , FUNCTIONS, DEVIDER NON_VAR , EMPTY_STR);
    DEBUG_PRINT_INSTR  (4 , FUNCTIONS, DEVIDER_2"BUILT-IN FUNCTIONS" NON_VAR , EMPTY_STR);

    // generate built in functions
    PRINT_FUNC_BUILT_IN(5 , "%s", FUNC_CHECK_IS_NIL);  DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(6 , "%s", FUNC_TOINTEGER);     DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(7 , "%s", FUNC_READI);         DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(8 , "%s", FUNC_READN);         DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(9 , "%s", FUNC_READS);         DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(10, "%s", FUNC_WRITE);         DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(11, "%s", FUNC_SUBSTR);        DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(12, "%s", FUNC_ORD);           DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(13, "%s", FUNC_CHR);           DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);


    // generate functions for expression evaluation
    PRINT_FUNC_BUILT_IN(14, "%s", FUNC_OP_NIL);        DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(15, "%s", FUNC_RETYPING_VAR1); DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(16, "%s", FUNC_RETYPING_VAR2); DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(17, "%s", FUNC_CHECK_OP);      DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(18, "%s", FUNC_CHECK_DIV);     DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(19, "%s", FUNC_CHECK_IDIV);     DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(20, "%s", FUNC_CHECK_COMP);      DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);
    PRINT_FUNC_BUILT_IN(21, "%s", FUNC_CHECK_IS_NIL_CONCAT);  DEBUG_PRINT_INSTR(30, FUNCTIONS, NON_VAR , EMPTY_STR);

    // code for better debug
    DEBUG_PRINT_INSTR  (22, FUNCTIONS, DEVIDER_2"END BUILT-IN FUNCTIONS" NON_VAR , EMPTY_STR);
    DEBUG_PRINT_INSTR  (23, FUNCTIONS, DEVIDER NON_VAR , EMPTY_STR);
    DEBUG_PRINT_INSTR  (24, FUNCTIONS, DEVIDER NON_VAR , EMPTY_STR);
    DEBUG_PRINT_INSTR  (25, FUNCTIONS, DEVIDER NON_VAR , EMPTY_STR);
#endif
    return true; // if everything good return true
}

bool gen_def_var() {
    // declare variable
    PRINT_FUNC(1, "defvar " FORMAT_VAR , cnt.func_name.str, queue_id->front->id->deep, queue_id->front->id->key_id);
    // define variable, that's why to prevent uninitialised variable error
    PRINT_FUNC(2, "move   " FORMAT_VAR " nil@nil" , cnt.func_name.str, queue_id->front->id->deep, queue_id->front->id->key_id);
    return true;
}

bool gen_init_var() {
    // defining variable by taking them from stack
    PRINT_FUNC(1, "pops GF@&var1" NON_VAR , EMPTY_STR);
    PRINT_FUNC(2, "move " FORMAT_VAR " GF@&var1" , cnt.func_name.str, queue_id->rear->id->deep, queue_id->rear->id->key_id);
    queue_remove_rear(queue_id); // remove from queue, not needed anymore
    return true;
}

/**********************************************************
 *                  IF ELSE END
 **********************************************************/
bool gen_if_start() {
    PRINT_FUNC(1, "label " FORMAT_IF , cnt.func_name.str, queue_if->rear->cnt_if);
    return true;
}

bool gen_if_else() {
    // label for else statement
    PRINT_FUNC(1, "label " FORMAT_ELSE , cnt.func_name.str, queue_if->rear->cnt_if);
    return true;
}

bool gen_if_end() {
    // label for if statement
    PRINT_FUNC(3, "label " FORMAT_IF_END , cnt.func_name.str, queue_if->rear->cnt_if);
    queue_remove_rear(queue_if);
    return true;
}

bool gen_if_eval() {
    // generate condition for if statement
    queue_add_rear(queue_if);
    queue_if->rear->cnt_if = ++cnt.if_cnt_max;
    PRINT_FUNC(1, "pops GF@&var1" NON_VAR , EMPTY_STR);
    PRINT_FUNC(2, "type GF@&type1  GF@&var1" NON_VAR , EMPTY_STR);
    PRINT_FUNC(3, "jumpifeq $%s$%d$pre_else$ GF@&type1 string@bool" , cnt.func_name.str, queue_if->rear->cnt_if);
    PRINT_FUNC(4, "jumpifeq $%s$%d$pre_else_false$ GF@&type1 string@nil" , cnt.func_name.str, queue_if->rear->cnt_if);
    PRINT_FUNC(5, "move  GF@&var1 bool@true" NON_VAR , EMPTY_STR);
    PRINT_FUNC(6, "jump $%s$%d$pre_else$" , cnt.func_name.str, queue_if->rear->cnt_if);
    PRINT_FUNC(7, "label $%s$%d$pre_else_false$" , cnt.func_name.str, queue_if->rear->cnt_if);
    PRINT_FUNC(8, "move  GF@&var1 bool@false" NON_VAR , EMPTY_STR);
    PRINT_FUNC(9, "label $%s$%d$pre_else$" , cnt.func_name.str, queue_if->rear->cnt_if);
    PRINT_FUNC(10, "jumpifeq $%s$%d$else$ GF@&var1 bool@false" , cnt.func_name.str, queue_if->rear->cnt_if);
    return true;
}

bool gen_if_end_jump() {
//    end of if else end statement
    PRINT_FUNC(2, "jump $%s$%d$if_end$" , cnt.func_name.str, queue_if->rear->cnt_if);
    return true;
}

/**********************************************************
 *                  WHILE
 **********************************************************/
bool gen_while_label() {
    // while loop start label
    queue_add_rear(queue_while);
    queue_while->rear->cnt_while = ++cnt.while_cnt_max;
    DEBUG_PRINT_INSTR(1, FUNCTIONS, EOL DEVIDER_2"while" NON_VAR , EMPTY_STR);
    PRINT_FUNC(2, "label "FORMAT_WHILE , cnt.func_name.str, queue_while->rear->cnt_while);
    return true;
}

bool gen_while_eval() {
    // generate condition
    PRINT_FUNC(1, "pops GF@&var1" NON_VAR , EMPTY_STR);
    PRINT_FUNC(2, "type GF@&type1  GF@&var1" NON_VAR , EMPTY_STR);
    PRINT_FUNC(3, "jumpifeq $%s$%d$pre_while$ GF@&type1 string@bool" , cnt.func_name.str, queue_while->rear->cnt_while);

    PRINT_FUNC(4, "jumpifeq $%s$%d$pre_while_false$ GF@&type1 string@nil" , cnt.func_name.str, queue_while->rear->cnt_while);
    PRINT_FUNC(5, "move  GF@&var1 bool@true" NON_VAR , EMPTY_STR);
    PRINT_FUNC(6, "jump $%s$%d$pre_while$" , cnt.func_name.str, queue_while->rear->cnt_while);
    PRINT_FUNC(7, "label $%s$%d$pre_while_false$" , cnt.func_name.str, queue_while->rear->cnt_while);
    PRINT_FUNC(8, "move  GF@&var1 bool@false" NON_VAR , EMPTY_STR);
    PRINT_FUNC(9, "label $%s$%d$pre_while$" , cnt.func_name.str, queue_while->rear->cnt_while);
    PRINT_FUNC(10, "jumpifeq $%s$%d$while_end$ GF@&var1 bool@false" , cnt.func_name.str, queue_while->rear->cnt_while);


    return true;
}

bool gen_while_end() {
    // repeat loop and generate label where to jump if condition for repetition loop is false
    PRINT_FUNC(1, "jump  "FORMAT_WHILE      , cnt.func_name.str, queue_while->rear->cnt_while);
    PRINT_FUNC(2, "label "FORMAT_WHILE_END  , cnt.func_name.str, queue_while->rear->cnt_while);
    DEBUG_PRINT_INSTR(3, FUNCTIONS, NON_VAR , EMPTY_STR);
    queue_remove_rear(queue_while);
    return true;
}

bool gen_concat_while_functions() {
    // concatenate final string together with code for while cycle, when token signalize end of while loop
    cnt.in_while = false;
    PRINT_FUNC_BUILT_IN(1, "%s", ifj_code[WHILE].str);
    str_clear(&ifj_code[WHILE]);
    return true;
}

/**********************************************************
 *                  FUNCTIONS
 **********************************************************/
bool gen_func_start(char *id) {
    // create label for function name, and move TF => LF with function parameters
    DEBUG_PRINT_INSTR(1, FUNCTIONS,	EOL DEVIDER NON_VAR, EMPTY_STR);
    PRINT_FUNC(2, "label $%s"          , id);
    PRINT_FUNC(3, "pushframe"  NON_VAR , EMPTY_STR);
    PRINT_FUNC(4, "createframe"NON_VAR , EMPTY_STR);
    return true;
}

bool gen_func_end() {
//    delete local function frame and return
    DEBUG_PRINT_INSTR(1, FUNCTIONS, EOL DEVIDER_2"end" 	NON_VAR, EMPTY_STR);
    PRINT_FUNC(2,   "popframe" 	        NON_VAR,  EMPTY_STR);
    PRINT_FUNC(3,   "return"   	        NON_VAR,  EMPTY_STR);
    DEBUG_PRINT_INSTR(4, FUNCTIONS,	DEVIDER NON_VAR, EMPTY_STR);
    return true;
}

bool gen_params() {
    DEBUG_PRINT_INSTR(1, FUNCTIONS, EOL DEVIDER_2"params" NON_VAR , EMPTY_STR);

    // generate all arguments for function call
    QueueElementPtr *queue_elem = queue_id->front;
    for (int i = 0; queue_elem; queue_elem = queue_elem->previous_element, i++) {
        PRINT_FUNC(2, "defvar "FORMAT_VAR             , cnt.func_name.str, queue_id->rear->id->deep, queue_elem->id->key_id);
        PRINT_FUNC(3, "move   "FORMAT_VAR FORMAT_PARAM, cnt.func_name.str, queue_id->rear->id->deep, queue_elem->id->key_id, i);
    }
    DEBUG_PRINT_INSTR(4, FUNCTIONS, EOL DEVIDER_2"logic"NON_VAR , EMPTY_STR);
    queue_dispose(queue_id); // need to delete all elements, for later use
    return true;
}

bool gen_func_call_start() {
    // create new TF (clean previous)
    DEBUG_PRINT_INSTR(1, where_to_print(), EOL DEVIDER_2"call_func" NON_VAR , EMPTY_STR);
    PRINT_WHERE(2, "createframe" NON_VAR, EMPTY_STR);
    return true;
}

bool gen_func_call_args_var(htab_item_t *htab_item) {
    // argument based on variable => new variable in TF created for calling function
    PRINT_WHERE(1, "defvar TF@%%%dp"       , cnt.param_cnt);
    PRINT_WHERE(2, "move   TF@%%%dp LF@$%s$%lu$%s$" , cnt.param_cnt, cnt.func_name.str, htab_item->deep, htab_item->key_id);

    if (!is_write()) {
        cnt.param_cnt++;
    }
	return true;
}

bool gen_func_call_args_const(token_t *token_1) {
    // generating constants based on their type
    PRINT_WHERE(1, "defvar TF@%%%dp" , cnt.param_cnt);
    switch(token_1->type) {
        case (T_INT)	: PRINT_WHERE(2, "move "  FORMAT_ARGS " int@%llu" , cnt.param_cnt, (llu_t)token_1->attr.num_i);  break;
        case (T_FLOAT)	: PRINT_WHERE(2, "move "  FORMAT_ARGS " float@%a" , cnt.param_cnt, token_1->attr.num_f);         break;
        case (T_STRING)	: convert_str_to_ascii(&token_1->attr.id);
                          PRINT_WHERE(2, "move "  FORMAT_ARGS " string@%s", cnt.param_cnt, token_1->attr.id.str);
                          break;
        default       	: PRINT_WHERE(2, "move "  FORMAT_ARGS " nil@nil"  , cnt.param_cnt);                              break;
    }

    if (strcmp(cnt.func_call.str, "write") != 0)
        cnt.param_cnt++;
    return true;
}

bool gen_func_call_label() {
    // write function is calling once for every argument (handling infinite number of arguments)
    if (strcmp(cnt.func_call.str, "write") == 0) {
        PRINT_WHERE(1, "call $write" NON_VAR, EMPTY_STR);
        return true;
    }

    // else normal function calling
    PRINT_WHERE(2, "call $%s" , queue_id->rear->id->key_id);
    queue_remove_rear(queue_id);
    return true;
}

int where_to_print() {
    // decide if inside whiel loop and return value where to print final code
    if (cnt.in_while) {
        return WHILE;
    }
    // if is not inside while decide if print into MAIN body or FUNCTION body
    return (strcmp(cnt.func_name.str, "") == 0) ? MAIN : FUNCTIONS;
}

bool is_write() {
    return (strcmp(cnt.func_call.str, "write") == 0) ? true : false;
}

bool convert_str_to_ascii(string_t *str_in) {
    if(strlen(str_in->str) == 0){
        return true;
    }
    string_t str_out;
    CODE_GEN(str_init, &str_out, strlen(str_in->str)*5);

    int idx = 0;
    char input_char = str_in->str[idx];
    char tmp_str[3];

    // convert escaped chars into its ascii value => create new string without escaped chars
    // number for macro "SWITCH_CASE" represents value of escaped char
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
            SWITCH_CASE(92);

            default:                    // if non escaped char just print them as it is
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

bool gen_retval_nil() {
    // generate code if count of returning values is fewer that in function declaration
    PRINT_FUNC(1, "pushs nil@nil " NON_VAR, EMPTY_STR);
    return true;
}

bool gen_expression() {
    htab_item_t *tmp;
    while (!queue_isEmpty(queue_expr)) {
        switch (queue_expr->front->token->type) {
            case T_ID:                   // push to stack variable
                tmp = find_id_symtbs(&local_symtbs, queue_expr->front->token->attr.id.str);
                if(!tmp){
                    CHECK_INTERNAL_ERR(true, false);
                }
                PRINT_FUNC(1, "pushs LF@$%s$%llu$%s$" , cnt.func_name.str, (llu_t)tmp->deep, queue_expr->front->token->attr.id.str);
                str_free(&queue_expr->front->token->attr.id);
                break;
            case T_INT:                 // push to stack integer
                PRINT_FUNC(2, "pushs int@%llu" , (llu_t)queue_expr->front->token->attr.num_i); //ubuntu chce lu
                break;
            case T_FLOAT:                   // push to stack flaot
                PRINT_FUNC(3, "pushs float@%a" , queue_expr->front->token->attr.num_f);
                break;
            case T_STRING:              // push to stack string after converting escaped character to decimal representation
                convert_str_to_ascii(&queue_expr->front->token->attr.id);
                PRINT_FUNC(4, "pushs string@%s" , queue_expr->front->token->attr.id.str);
                str_free(&queue_expr->front->token->attr.id);
                break;
            case T_KEYWORD:                 // push to stack nil
                if(queue_expr->front->token->keyword == KW_NIL){
                    PRINT_FUNC(5, "pushs nil@nil" NON_VAR , EMPTY_STR);
                } else {
                    CHECK_INTERNAL_ERR(true, false);
                }
                break;
            case T_PLUS:                    // check operands and make add operation
                PRINT_FUNC(6, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(7, "adds" NON_VAR , EMPTY_STR);
                break;
            case T_MINUS:                    // check operands and make minus operation
                PRINT_FUNC(8, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(9, "subs" NON_VAR , EMPTY_STR);
                break;
            case T_MUL:                    // check operands and make multiply operation
                PRINT_FUNC(10, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(11, "muls" NON_VAR , EMPTY_STR);
                break;
            case T_DIV:                     // check operands and make division operation
                PRINT_FUNC(12, "call $check_div" NON_VAR , EMPTY_STR);
                PRINT_FUNC(13, "divs" NON_VAR , EMPTY_STR);
                break;
            case T_DIV_INT:                     // check operands and make divide operation
                PRINT_FUNC(14, "call $check_idiv" NON_VAR , EMPTY_STR);
                PRINT_FUNC(15, "idivs" NON_VAR , EMPTY_STR);
                break;
            case T_LT:                     // check operands and compare operands for lower_than
                PRINT_FUNC(16, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(17, "lts" NON_VAR , EMPTY_STR);
                break;
            case T_GT:                    // check operands and compare operands for greater_than
                PRINT_FUNC(18, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(19, "gts" NON_VAR , EMPTY_STR);
                break;
            case T_LE:                    // check operands and compare operands for lower_equal
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
            case T_GE:                    // check operands and compare operands for greater_equal
                PRINT_FUNC(31, "call $check_op" NON_VAR , EMPTY_STR);
                PRINT_FUNC(32, "pops GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(33, "pops GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(34, "pushs GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(35, "pushs GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(36, "gts" NON_VAR , EMPTY_STR);
                PRINT_FUNC(37, "pushs GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(38, "pushs GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(39,  "eqs" NON_VAR , EMPTY_STR);
                PRINT_FUNC(40, "ors" NON_VAR , EMPTY_STR);
                break;
            case T_EQ:                    // check operands and compare operands for equality
                PRINT_FUNC(51, "call $check_comp" NON_VAR , EMPTY_STR);
                PRINT_FUNC(52, "eqs" NON_VAR , EMPTY_STR);
                break;
            case T_NEQ:                    // check operands and compare operands for non equality
                PRINT_FUNC(53, "call $check_comp" NON_VAR , EMPTY_STR);
                PRINT_FUNC(54, "eqs" NON_VAR , EMPTY_STR);
                PRINT_FUNC(55, "nots" NON_VAR , EMPTY_STR);
                break;
            case T_LENGTH:                    // check operand and get length of string
                PRINT_FUNC(56,  "call $check_is_nil" NON_VAR , EMPTY_STR);
                PRINT_FUNC(57, "pops GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(58, "strlen GF@&var1 GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(59, "pushs GF@&var1" NON_VAR , EMPTY_STR);
                break;
            case T_CONCAT:                  // check operands for nil and concatenate them together
                PRINT_FUNC(60, "pops GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(61, "pops GF@&var1" NON_VAR , EMPTY_STR);
                PRINT_FUNC(62, "call $check_is_nil_concat" NON_VAR , EMPTY_STR);
                PRINT_FUNC(63, "concat GF@&var1 GF@&var1 GF@&var2" NON_VAR , EMPTY_STR);
                PRINT_FUNC(64, "pushs GF@&var1" NON_VAR , EMPTY_STR);
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
    // free all allocated sources
    str_free(&cnt.func_name);
    str_free(&cnt.func_call);
    str_free(&ifj_code[MAIN]);
    str_free(&ifj_code[FUNCTIONS]);
    str_free(&ifj_code[WHILE]);
    queue_free(queue_if);
    queue_free(queue_while);
    return true;
}
