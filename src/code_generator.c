#include "code_generator.h"
#include <stdlib.h>

/******************************************************************************
 *                                  TODO
 *****************************************************************************/
// TODO - check vars, params, if, while numbering - tests - redefinition


/******************************************************************************
  *									MACROS
*****************************************************************************/
#define IFJ_CODE_START_LEN 10000
#define MAX_FUNC_LEN       1000
#define EOL "\n"
#define EMPTY_STR ""
#define NON_VAR "%s"

#define PRINT_INSTR(num, fmt, ...)                                          \
    do {                                                                    \
        char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + 2)];       \
        sprintf(instr##num, (fmt), __VA_ARGS__);                            \
        if (!str_concat_str2(&ifj_code, instr##num)) {                      \
            return false;                                                   \
        }                                                                   \
    } while(0)

#ifdef DEBUG_INSTR
	#define DEBUG_PRINT_INSTR(num, fmt, ...)                                \
		do {                                                                \
			char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + 2)];   \
			sprintf(instr##num, (fmt), __VA_ARGS__);                        \
			if (!str_concat_str2(&ifj_code, instr##num)) {                  \
				return false;                                               \
			}                                                               \
		} while(0)
#else
	#define DEBUG_PRINT_INSTR(num, fmt, ...)
#endif // DEBUG_INSTR



/******************************************************************************
  *									GLOBAL VARS
******************************************************************************/
string_t ifj_code;
extern Queue* queue_id;
extern Queue* queue_expr;
extern int err;
extern arr_symtbs_t local_symtbs;
cnts_t cnt;


/******************************************************************************
  *									FUNCTIONS
******************************************************************************/
bool init_cnt() {
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
//    PRINT_INSTR(0, "INT2CHAR LF@%s LF@%s", queue->front->id->key_id, symb_1->attr.id, NULL);
    return true;
}

bool gen_func_label() {
    return true;
}

bool gen_header() {
    PRINT_INSTR(1, ".IFJcode21" NON_VAR EOL EOL, EMPTY_STR);
    return true;
}


bool gen_init_built_ins() {
    /*PRINT_INSTR(1, "%s", FUNC_TOINTEGER);
    PRINT_INSTR(2, "%s", FUNC_READI);
    PRINT_INSTR(3, "%s", FUNC_READN);
    PRINT_INSTR(4, "%s", FUNC_READS);
    PRINT_INSTR(5, "%s", FUNC_WRITE);
    PRINT_INSTR(6, "%s", FUNC_SUBSTR);
    PRINT_INSTR(7, "%s", FUNC_ORD);
    PRINT_INSTR(8, "%s", FUNC_CHR);
    PRINT_INSTR(10, "%s", FUNC_RETYPING_VAR1);
    PRINT_INSTR(11, "%s", FUNC_RETYPING_VAR2);
    PRINT_INSTR(12, "%s", FUNC_CHECK_OP);
    PRINT_INSTR(13, "%s", FUNC_CHECK_COMP);
    PRINT_INSTR(14, "%s", FUNC_OP_NIL);
    PRINT_INSTR(15, "%s", FUNC_CHECK_DIV);*/
    return true;
}

bool gen_label_item() {
    // TODO -
    // defvar
    // pop
    return true;
}

bool gen_while_label(htab_item_t *htab_item) {
    PRINT_INSTR(0, "label $%s_while_%d"EOL, htab_item->key_id,  cnt.while_cnt);
    cnt.while_cnt++;
	return true;
}

bool gen_while_cond(/*TODO*/) {
	// TODO
    // PRINT_INSTR(0, "label $%s_while_%d_end"EOL, htab_item->key_id,  cnt.while_cnt);
	// switch jumpifneq , jumpifeq etc.. label
	return true;
}

bool gen_while_start() {
    return true;
}

bool gen_while_end(htab_item_t *htab_item) {
	cnt.while_cnt--;
	PRINT_INSTR(0, "jump $%s_while_%d"EOL, htab_item->key_id, cnt.while_cnt);
    return true;
}

bool gen_params() {
    DEBUG_PRINT_INSTR(1, EOL"# params" NON_VAR EOL, EMPTY_STR);

    QueueElementPtr *queue_elem = queue_id->front;
    for (int i = 0; queue_elem; queue_elem = queue_elem->previous_element, i++) {
        PRINT_INSTR(2, "defvar LF@%s"        EOL, queue_elem->id->key_id);
        PRINT_INSTR(3, "move LF@%s  LF@%dp " EOL, queue_elem->id->key_id, i);
    }
    DEBUG_PRINT_INSTR(3, EOL"# logic"NON_VAR EOL, EMPTY_STR);
    queue_dispose(queue_id);
    return true;
}

bool gen_param() {
//    PRINT_INSTR(1, "# params" NON_VAR EMPTY_STR EOL, queue_id->front->id->key_id);
//    PRINT_INSTR(2, "defvar LF@%s"               EOL, queue_id->front->id->key_id);
//    PRINT_INSTR(3, "move LF@%s LF@!p1"          EOL, queue_id->front->id->key_id);
//    cnt.param_cnt++;
    return true;
}

bool gen_if_start() {
    cnt.if_cnt++;
    PRINT_INSTR(1, "label $%s$%d$if$" EOL, cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_if_else() {
    PRINT_INSTR(2, "label $%s$%d$$else$" EOL, cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_if_end(/*TODO*/) {
    PRINT_INSTR(3, "label $%s$%d$if_end$" EOL, cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_if_eval() {
    PRINT_INSTR(1, "pops GF@&var1" NON_VAR EOL, EMPTY_STR);
    PRINT_INSTR(2, "jumpifneq $%s$%d$$else$ GF@&type1 string@nil" EOL, cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_if_end_jump() {
    PRINT_INSTR(2, "jump $%s$%d$if_end$" EOL, cnt.func_name.str, cnt.if_cnt);
    return true;
}

bool gen_func_start(char *id) {
    cnt.if_cnt = 0;
    DEBUG_PRINT_INSTR(1, 	EOL"########################################################"   	NON_VAR, EOL);
    PRINT_INSTR(2, "label $%s"          EOL, id);
    PRINT_INSTR(3, "pushframe"  NON_VAR EOL, EMPTY_STR);
    PRINT_INSTR(4, "createframe"NON_VAR EOL, EMPTY_STR);
    return true;
}

bool gen_func_end() {
    DEBUG_PRINT_INSTR(1, EOL"# end" 	NON_VAR, EOL);
    PRINT_INSTR(2,  "popframe" 	NON_VAR, EOL);
    PRINT_INSTR(3, 	"return"   	NON_VAR, EOL);
    DEBUG_PRINT_INSTR(3, 	"########################################################"   	NON_VAR, EOL);
    return true;
}

bool gen_func_call_start() {
    DEBUG_PRINT_INSTR(1, EOL"# call_func" NON_VAR EOL, EMPTY_STR);
    PRINT_INSTR(2, "createframe"    NON_VAR EOL, EMPTY_STR);
    return true;
}

bool gen_func_call_args_var(htab_item_t *htab_item) {
	PRINT_INSTR(1, "defvar TF@%dp"       EOL, cnt.param_cnt);
    PRINT_INSTR(1, "move   TF@%dp LF@%s" EOL, cnt.param_cnt, htab_item->key_id);
    cnt.param_cnt++;
	return true;
}

bool gen_func_call_args_const(token_t *token) {
    PRINT_INSTR(1, "defvar TF@%dp" EOL, cnt.param_cnt);
	switch(token->type) {
		case (T_INT)	: PRINT_INSTR(2, "move   TF@%dp float@%s" EOL, cnt.param_cnt, token->attr.id.str); break;
		case (T_FLOAT)	: PRINT_INSTR(2, "move   TF@%dp float@%s" EOL, cnt.param_cnt, token->attr.id.str); break;
		case (T_STRING)	: PRINT_INSTR(2, "move   TF@%dp string@%s"EOL, cnt.param_cnt, token->attr.id.str); break;
		case (KW_NIL)	: PRINT_INSTR(2, "move   TF@%dp nil@nil"  EOL, cnt.param_cnt);                     break;
		default: break;
	}
    cnt.param_cnt++;
    return true;
}

bool gen_func_call_label() {
    PRINT_INSTR(1, "call $%s" EOL, queue_id->rear->id->key_id);
	queue_dispose(queue_id);
    init_cnt();
	return true;
}

bool code_gen_print_ifj_code21() {
    FILE *test_file = fopen("test_file.out", "w");
    if (!test_file) {
        return false;
    }
    fprintf(test_file, "%s", ifj_code.str);
    fclose(test_file);
    return true;
}

bool gen_init() {
    if (!str_init(&ifj_code, IFJ_CODE_START_LEN) ||
        !gen_header()                            ||
        !init_cnt()                              ||
        !gen_init_built_ins()
        ) {
        err = INTERNAL_ERR;
    }

    PRINT_INSTR(4, "defvar GF@&type1" NON_VAR EOL, EMPTY_STR);
    PRINT_INSTR(5, "defvar GF@&type2" NON_VAR EOL, EMPTY_STR);
    PRINT_INSTR(6, "defvar GF@&var1"  NON_VAR EOL, EMPTY_STR);
    PRINT_INSTR(7, "defvar GF@&var2"  NON_VAR EOL, EMPTY_STR);

    //str_free(&cnt.func_name);
    return (err == NO_ERR);
}

bool gen_testing_helper() {
    FILE *test_file = fopen("test_file.out", "w");
    if (!test_file)
        return false;
    fprintf(stdout, "%s", ifj_code.str);
    fclose(test_file);
    return true;
}

bool gen_expression() {
    htab_item_t *tmp;
    str_init(&cnt.func_name,100);
    cnt.func_name.str = "main\0";
    while (!queue_isEmpty(queue_expr)) {
        switch (queue_expr->front->token->type) {
            case T_ID:
                tmp = find_id_symtbs(&local_symtbs, queue_expr->front->token->attr.id.str);
                if(!tmp){
                    //todo error
                    tmp = calloc(1, sizeof(htab_item_t)); //todo vymazat
                    tmp->deep = 1; //todo vymazat
                }
                PRINT_INSTR(1, "pushs LF@$%s$%lu$%s$" EOL, cnt.func_name.str, tmp->deep, queue_expr->front->token->attr.id.str);
                break;
            case T_INT:
                PRINT_INSTR(2, "\npushs int@%llu" EOL, queue_expr->front->token->attr.num_i); //ubuntu chce lu
                break;
            case T_FLOAT:
                // todo format (asi je treba skontrolovat)
                PRINT_INSTR(3, "\npushs float@%a" EOL, queue_expr->front->token->attr.num_f);
                break;
            case T_STRING:
                PRINT_INSTR(4, "\npushs string@%s" EOL, queue_expr->front->token->attr.id.str);
                break;
            case T_KEYWORD:
                if(queue_expr->front->token->keyword == KW_NIL){
                    PRINT_INSTR(5, "pushs nil@nil" NON_VAR EOL, EMPTY_STR);
                }
                // todo else what??
                break;
            case T_PLUS:
                PRINT_INSTR(6, "call $check_op" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(7, "adds" NON_VAR EOL, EMPTY_STR);
                break;
            case T_MINUS:
                PRINT_INSTR(8, "call $check_op" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(9, "subs" NON_VAR EOL, EMPTY_STR);
                break;
            case T_MUL:
                PRINT_INSTR(10, "call $check_op" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(11, "muls" NON_VAR EOL, EMPTY_STR);
                break;
            case T_DIV:
                PRINT_INSTR(12, "call $check_div" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(13, "divs" NON_VAR EOL, EMPTY_STR);
                break;
            case T_DIV_INT:
                PRINT_INSTR(14, "call $check_div" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(15, "idivs" NON_VAR EOL, EMPTY_STR);
                break;
            case T_LT:
                PRINT_INSTR(16, "call $check_op" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(17, "lts" NON_VAR EOL, EMPTY_STR);
                break;
            case T_GT:
                PRINT_INSTR(18, "call $check_op" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(19, "gts" NON_VAR EOL, EMPTY_STR);
                break;
            case T_LE:
                PRINT_INSTR(20, "call $check_op" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(21, "pops GF@&var2" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(22, "pops GF@&var1" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(23, "pushs GF@&var1" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(24, "pushs GF@&var2" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(25, "lts" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(26, "pushs GF@&var1" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(27, "pushs GF@&var2" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(28, "eqs" NON_VAR EOL, EMPTY_STR);

                PRINT_INSTR(29, "ors" NON_VAR EOL, EMPTY_STR);
                break;
            case T_GE:
                PRINT_INSTR(30, "call $check_op" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(31, "pops GF@&var2" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(32, "pops GF@&var1" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(33, "pushs GF@&var1" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(34, "pushs GF@&var2" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(35, "gts" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(36, "pushs GF@&var1" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(37, "pushs GF@&var2" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(38, "neqs" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(39, "ors" NON_VAR EOL, EMPTY_STR);
                break;
            case T_EQ:
                PRINT_INSTR(40, "call $check_comp" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(41, "eqs" NON_VAR EOL, EMPTY_STR);
                break;
            case T_NEQ:
                PRINT_INSTR(42, "call $check_comp" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(43, "eqs" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(44, "nots" NON_VAR EOL, EMPTY_STR);
                break;
            case T_LENGTH:
                // todo nejake mozne errors? alebo pretypovania?
                PRINT_INSTR(45, "pops GF@&var1" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(46, "strlen GF@&var1 GF@&var1" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(47, "pushs GF@&var1" NON_VAR EOL, EMPTY_STR);
                break;
            case T_CONCAT:
                // todo nejake mozne errors? alebo pretypovania?
                PRINT_INSTR(48, "pops GF@&var2" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(49, "pops GF@&var1" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(50, "concat GF@&var1 GF@&var1 GF@&var2" NON_VAR EOL, EMPTY_STR);
                PRINT_INSTR(51, "pushs GF@&var1" NON_VAR EOL, EMPTY_STR);
                break;
            default:
                break;
        }
        queue_remove(queue_expr);
    }
    PRINT_INSTR(52, "\npops LF@%s", queue_id->front->id->key_id);
    PRINT_INSTR(53, "\nwrite LF@x"  NON_VAR EOL, EMPTY_STR);

    return true;
}



