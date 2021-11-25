#include "code_generator.h"
#include <stdlib.h>

/******************************************************************************
 *                                  TODO
 *****************************************************************************/
// TODO - check vars, params, if, while numbering - tests - redefinition
// TODO - change if in func PRINT_FUNC else PRINT_MAIN
// TODO - format params in func


/******************************************************************************
 *                                  CONVENTIONS
 *****************************************************************************/
// OUR:
// funcs    = label $funcname
// var      = $func$poradie_if$if$poradie_if_else_v_if$else_if$

// THEIR:
// funcs    = label &funcname


/******************************************************************************
  *									MACROS
*****************************************************************************/
#define IFJ_CODE_START_LEN 10000
#define MAX_LINE_LEN       300
#define EOL "\n"
#define EMPTY_STR ""
#define NON_VAR "%s"
#define DEBUG_INSTR   1
#define DEBUG_INSTR_2 1
#define DEVIDER "################################################################################# "
#define DEVIDER_2 "########## "
#define FORMAT_VAR " LF@$%s$%lu$%s$ "
#define FORMAT_PARAM " LF@%%%dp "


#define DEBUG_GEN(fmt, ...) \
    do { if (DEBUG_INSTR) fprintf(stderr, "%40s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)

#define INIT_CONCAT_STR(num, fmt, ...)                                                      \
    do {                                                                                    \
        if (!DEBUG_INSTR_2) {                                                               \
            sprintf(instr##num, (fmt EOL), __VA_ARGS__);                                    \
        } else {                                                                            \
            snprintf(instr##num, MAX_LINE_LEN, (fmt "%*s:%d:%s():" EOL), __VA_ARGS__,       \
                                    80-snprintf(NULL, 0, (fmt), __VA_ARGS__),           \
                                    "#", __LINE__, __func__);                               \
        }                                                                                   \
    } while(0)                                                                              \

#define PRINT_MAIN(num, fmt, ...)                                                           \
    do {                                                                                    \
        char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + MAX_LINE_LEN)];            \
        INIT_CONCAT_STR(num, fmt, __VA_ARGS__);                                             \
        if (!str_concat_str2(&ifj_code[MAIN], instr##num)) {                                \
            return false;                                                                   \
        }                                                                                   \
    } while(0)

#define PRINT_FUNC(num, fmt, ...)                                                           \
    do {                                                                                    \
        char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + MAX_LINE_LEN)];            \
        INIT_CONCAT_STR(num, fmt, __VA_ARGS__);                                                                                    \
        if (!str_concat_str2(&ifj_code[FUNCTIONS], instr##num)) {                           \
            return false;                                                                   \
        }                                                                                   \
    } while(0)

#define PRINT_FUNC_BUILT_IN(num, fmt, ...)                                                           \
    do {                                                                                    \
        char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + MAX_LINE_LEN)];            \
        sprintf(instr##num, (fmt EOL), __VA_ARGS__);                                    \
        if (!str_concat_str2(&ifj_code[FUNCTIONS], instr##num)) {                           \
            return false;                                                                   \
        }                                                                                   \
    } while(0)

#if DEBUG_INSTR
#define DEBUG_PRINT_INSTR(num, NUM_BLOCK ,fmt, ...)                                 \
		do {                                                                        \
			char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + MAX_LINE_LEN)];           \
			INIT_CONCAT_STR(num, fmt, __VA_ARGS__);                                         \
			if (!str_concat_str2(&ifj_code[NUM_BLOCK], instr##num)) {               \
				return false;                                                       \
			}                                                                       \
		} while(0)
#else
	#define DEBUG_PRINT_INSTR(num, fmt, ...)
#endif // DEBUG_INSTR

#define PRINT_WHERE(...)                        \
    do {                                        \
        if (strcmp(cnt.func_name.str, "") == 0) {   \
                PRINT_MAIN(__VA_ARGS__);            \
        } else {                                    \
                PRINT_FUNC(__VA_ARGS__);            \
        }                                           \
    } while(0)                                  \



/******************************************************************************
  *									GLOBAL VARS
******************************************************************************/
string_t ifj_code[BLOCKS_NUM];
extern Queue* queue_id;
extern Queue* queue_expr;
extern int err;
extern arr_symtbs_t local_symtbs;
cnts_t cnt = {.func_name.alloc_size = 0};

/******************************************************************************
  *									FUNCTIONS
******************************************************************************/
bool init_cnt() {
    if (cnt.func_name.alloc_size == 0) str_init(&cnt.func_name, IFJ_CODE_START_LEN);
    else                               str_clear(&cnt.func_name);

    if (cnt.func_call.alloc_size == 0) str_init(&cnt.func_call, IFJ_CODE_START_LEN);
    else                               str_clear(&cnt.func_call);

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
    PRINT_FUNC(2, "%s", FUNC_READI);
    PRINT_FUNC(3, "%s", FUNC_READN);
    PRINT_FUNC(4, "%s", FUNC_READS);
    PRINT_FUNC(6, "%s", FUNC_SUBSTR);
    PRINT_FUNC(7, "%s", FUNC_ORD);
    PRINT_FUNC(8, "%s", FUNC_CHR);
    PRINT_FUNC(10, "%s", FUNC_RETYPING_VAR1);
    PRINT_FUNC(11, "%s", FUNC_RETYPING_VAR2);
    PRINT_FUNC(12, "%s", FUNC_CHECK_OP);
    PRINT_FUNC(13, "%s", FUNC_CHECK_COMP);
    PRINT_FUNC(14, "%s", FUNC_OP_NIL);
#endif
    PRINT_FUNC_BUILT_IN(15, "%s", FUNC_RETYPING_VAR1);
    PRINT_FUNC_BUILT_IN(16, "%s", FUNC_RETYPING_VAR2);
    PRINT_FUNC_BUILT_IN(16, "%s", FUNC_CHECK_OP);
    PRINT_FUNC_BUILT_IN(16, "%s", FUNC_CHECK_COMP);
    PRINT_FUNC_BUILT_IN(16, "%s", FUNC_OP_NIL);

    PRINT_FUNC(5, "%s", FUNC_WRITE);
    return true;
}

bool gen_label_item() {
    // TODO -
    // defvar
    // pop
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
//    PRINT_FUNC(1, "# params" NON_VAR EMPTY_STR , queue_id->front->id->key_id);
//    PRINT_FUNC(2, "defvar LF@%s"               , queue_id->front->id->key_id);
//    PRINT_FUNC(3, "move LF@%s LF@!p1"          , queue_id->front->id->key_id);
//    cnt.param_cnt++;
    return true;
}

bool gen_def_var() {
    PRINT_FUNC(1, "defvar LF@$%s$%lu$%s$" , cnt.func_name.str, queue_id->front->id->deep, queue_id->front->id->key_id);
    PRINT_FUNC(1, "move LF@$%s$%lu$%s$ nil@nil" , cnt.func_name.str, queue_id->front->id->deep, queue_id->front->id->key_id);
    return true;
}

bool gen_init_var() {
    PRINT_FUNC(1, "pops GF@&var1" NON_VAR , EMPTY_STR);
    PRINT_FUNC(1, "move LF@$%s$%lu$%s$ GF@&var1" , cnt.func_name.str, queue_id->rear->id->deep, queue_id->rear->id->key_id);
    queue_remove_rear(queue_id);
    return true;
}

bool gen_if_start() {
    PRINT_FUNC(1, "label $%s$%d$if$" , cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_if_else() {
    PRINT_FUNC(2, "label $%s$%d$else$" , cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_if_end(/*TODO*/) {
    PRINT_FUNC(3, "label $%s$%d$if_end$" , cnt.func_name.str, cnt.if_cnt);
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
    init_cnt();
    strcpy(cnt.func_name.str, id);
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
//    if (strcmp(cnt.func_name.str, "") == 0) PRINT_MAIN(2, "createframe"    NON_VAR , EMPTY_STR);
//    else                                    PRINT_FUNC(2, "createframe"    NON_VAR , EMPTY_STR);
    return true;
}

bool gen_func_call_write() {
    return true;
}

bool gen_func_call_args_var(htab_item_t *htab_item) {
    PRINT_WHERE(1, "defvar TF@%%%dp"       , cnt.param_cnt);
    PRINT_WHERE(2, "move   TF@%%%dp LF@$%s$%lu$%s$" , cnt.param_cnt, cnt.func_name.str, htab_item->deep, htab_item->key_id );
//    if (strcmp(cnt.func_name.str, "") == 0) PRINT_MAIN(1, "defvar TF@%dp"       , cnt.param_cnt);
//    else                                    PRINT_FUNC(1, "defvar TF@%dp"       , cnt.param_cnt);;
//
//    if (strcmp(cnt.func_name.str, "") == 0) PRINT_MAIN(2, "move   TF@%dp LF@%s" , cnt.param_cnt, htab_item->key_id);
//    else                                    PRINT_FUNC(2, "move   TF@%dp LF@%s" , cnt.param_cnt, htab_item->key_id);

    if (!is_write()) {
        cnt.param_cnt++;
    }
	return true;
}

bool gen_func_call_args_const(token_t *token) {
//    if (strcmp(cnt.func_name.str, "") == 0) {
    PRINT_WHERE(1, "defvar TF@%%%dp" , cnt.param_cnt);
    switch(token->type) {
        case (T_INT)	: PRINT_WHERE(2, "move   TF@%%%dp int@%llu" , cnt.param_cnt, (llu_t)token->attr.num_i); break;
        case (T_FLOAT)	: PRINT_WHERE(2, "move   TF@%%%dp float@%a" , cnt.param_cnt, token->attr.num_f); break;
        case (T_STRING)	: PRINT_WHERE(2, "move   TF@%%%dp string@%s", cnt.param_cnt, token->attr.id.str); break;
        default       	: PRINT_WHERE(2, "move   TF@%%%dp nil@nil"  , cnt.param_cnt);                     break;
    }
//    } else {
//        PRINT_FUNC(1, "defvar TF@%dp" , cnt.param_cnt);
//        switch(token->type) {
//            case (T_INT)	: PRINT_FUNC(2, "move   TF@%%%dp int@%s" , cnt.param_cnt, token->attr.id.str); break;
//            case (T_FLOAT)	: PRINT_FUNC(2, "move   TF@%%%dp float@%s" , cnt.param_cnt, token->attr.id.str); break;
//            case (T_STRING)	: PRINT_FUNC(2, "move   TF@%%%dp string@%s", cnt.param_cnt, token->attr.id.str); break;
//            default       	: PRINT_FUNC(2, "move   TF@%%%dp nil@nil"  , cnt.param_cnt);                     break;
//        }
//    }

    if (strcmp(cnt.func_call.str, "write") != 0)
        cnt.param_cnt++;
    return true;
}

bool gen_func_call_label() {
    if (strcmp(cnt.func_call.str, "write") == 0) {
        PRINT_WHERE(1, "call &write" NON_VAR, EMPTY_STR);
        return true;
//        if (strcmp(cnt.func_name.str, "") == 0) PRINT_MAIN(1, "call &%s" , queue_id->rear->id->key_id);
//        else                                    PRINT_FUNC(1, "call &%s" , queue_id->rear->id->key_id);
    }

    PRINT_WHERE(1, "call &%s" , queue_id->rear->id->key_id);
//    if (strcmp(cnt.func_name.str, "") == 0) PRINT_MAIN(1, "call &%s" , queue_id->rear->id->key_id);
//    else                                    PRINT_FUNC(1, "call &%s" , queue_id->rear->id->key_id);

    queue_remove_rear(queue_id);
    init_cnt();
	return true;
}

bool gen_init() {
    if (!str_init(&ifj_code[FUNCTIONS], IFJ_CODE_START_LEN) ||
        !str_init(&ifj_code[MAIN], IFJ_CODE_START_LEN)      ||
        !init_cnt()
        ) {
        err = INTERNAL_ERR;
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
        err = INTERNAL_ERR;
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

#if 0
bool PRINT_FUNC(const char* instr, ...) {
    PRINT_FUNC(1, "pushs LF@$%s$%llu$%s$" , cnt.func_name.str, tmp->deep, queue_expr->front->token->attr.id.str);
    va_list pArgs;
    va_start(pArgs, instr);
    va_arg(pArgs, char *);

    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, instr, pArgs);

    va_end(pArgs);
    return true;
}
#endif

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
