#include "code_generator.h"

/******************************************************************************
 *                                  TODO
 *****************************************************************************/
// TODO - check vars, params, if, while numbering - tests - redefinition


#define FUNC_DIV_ZERO   \
"\nlabel $div_zero"     \
"\n exit int@9"

#define FUNC_OP_NIL     \
"\nlabel $op_nil"     \
"\n exit int@8"

#define RETYPING_VAR1 \
"\nlabel $retyping_var1" \
"\n int2float GF@var1 GF@var1" \
"return"

#define RETYPING_VAR2 \
"\nlabel $retyping_var2" \
"\n int2float GF@var2 GF@var2" \
"return"


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





#define TYPE_CHECK() \
PRINT_INSTR(a, "pops GF@&var2" NON_VAR EOL, EMPTY_STR);                                             \
PRINT_INSTR(b, "pops GF@&var1" NON_VAR EOL , EMPTY_STR);                                            \
PRINT_INSTR(c, "type GF@&type1 GF@&var1" NON_VAR EOL, EMPTY_STR);                                   \
PRINT_INSTR(c, "type GF@&type2 GF@&var2" NON_VAR EOL EOL, EMPTY_STR);                               \
PRINT_INSTR(d, "jumpifeq $op_nil GF@&type1 string@nil" NON_VAR EOL, EMPTY_STR);                     \
PRINT_INSTR(e, "jumpifeq $op_nil GF@&type2 string@nil" NON_VAR EOL, EMPTY_STR);                     \
PRINT_INSTR(f, "jumpifeq $%s$%d$%d$continue_end GF@&type1 GF@&type2" EOL EOL, cnt.func_name.str,    \
            cnt.continue_cnt, cnt.deep);                                                            \
PRINT_INSTR(f, "jumpifeq $%s$%d$%d$continue_mid GF@&type1 string@float" EOL, cnt.func_name.str,     \
            cnt.continue_cnt, cnt.deep);                                                            \
PRINT_INSTR(g, "call $retyping_var1" NON_VAR EOL EOL, EMPTY_STR);                                   \
PRINT_INSTR(h, "label $%s$%d$%d$continue_mid" EOL, cnt.func_name.str, cnt.continue_cnt, cnt.deep);  \
PRINT_INSTR(g, "call $retyping_var2" NON_VAR EOL EOL, EMPTY_STR);                                   \
PRINT_INSTR(h, "label $%s$%d$%d$continue_end" EOL, cnt.func_name.str, cnt.continue_cnt, cnt.deep);  \
PRINT_INSTR(i, "pushs GF@&var1" NON_VAR EOL, EMPTY_STR);                                            \
PRINT_INSTR(j, "pushs GF@&var2" NON_VAR EOL EOL, EMPTY_STR);

#define IS_DIV_ZERO() \
PRINT_INSTR(aa, "\n\npops GF@$var2%s", "");              \
PRINT_INSTR(bb, "\njumpifeq $div_zero GF@$var2 int@0%s", ""); \
PRINT_INSTR(cc, "\npushs GF@$var2%s", "");
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
cnts_t cnt;


/******************************************************************************
  *									FUNCTIONS
******************************************************************************/
bool init_cnt() {
    cnt.param_cnt = 0;
    cnt.if_cnt    = 0;
    cnt.while_cnt = 0;
    cnt.deep      = 0;
    cnt.continue_cnt = 0;
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
    PRINT_INSTR(1, "%s", FUNC_TOINTEGER);
    PRINT_INSTR(2, "%s", FUNC_READI);
    PRINT_INSTR(3, "%s", FUNC_READN);
    PRINT_INSTR(4, "%s", FUNC_READS);
    PRINT_INSTR(5, "%s", FUNC_WRITE);
    PRINT_INSTR(6, "%s", FUNC_SUBSTR);
    PRINT_INSTR(7, "%s", FUNC_ORD);
    PRINT_INSTR(8, "%s", FUNC_CHR);
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

bool gen_if_start(/*TODO*/) {
//    PRINT_INSTR(1, "label %s_if_%s", ""/*TODO - func_name + id_of_if*/);
    return true;
}

bool gen_if_else(/*TODO*/) {
//    PRINT_INSTR(2, "label else%s", ""/*TODO - func_name + id_of_if*/);
    return true;
}

bool gen_if_end(/*TODO*/) {
//    PRINT_INSTR(3, "label end%s", ""/*TODO - func_name + id_of_if*/);
    return true;
}

bool gen_func_start(char *id) {
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
        !init_cnt()
//      !gen_init_built_ins()
        ) {
        err = INTERNAL_ERR;
    }

    PRINT_INSTR(4, "defvar GF@&type1" NON_VAR EOL, EMPTY_STR);
    PRINT_INSTR(5, "defvar GF@&type2" NON_VAR EOL, EMPTY_STR);
    PRINT_INSTR(6, "defvar GF@&var1"  NON_VAR EOL, EMPTY_STR);
    PRINT_INSTR(7, "defvar GF@&var2"  NON_VAR EOL, EMPTY_STR);

    str_free(&cnt.func_name);
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
    while (!queue_isEmpty(queue_expr)) {
        switch (queue_expr->front->token->type) {
            case T_ID:
                PRINT_INSTR(1, "pushs LF@&%s" EOL, queue_expr->front->token->attr.id.str);
                break;
            case T_INT:
                PRINT_INSTR(2, "\npushs int@%llu" EOL, queue_expr->front->token->attr.num_i);
                break;
            case T_FLOAT:
                PRINT_INSTR(3, "\npushs float@%f" EOL, queue_expr->front->token->attr.num_f);
                break;
            case T_STRING:
                PRINT_INSTR(4, "\npushs string@%s" EOL, queue_expr->front->token->attr.id.str);
                break;
            case T_NONE: //todo nil???
                PRINT_INSTR(5, "pushs nil@nil" NON_VAR EOL, EMPTY_STR);
                break;
            case T_PLUS:
                TYPE_CHECK();
                PRINT_INSTR(6, "adds" NON_VAR EOL, EMPTY_STR);
                break;

            case T_MINUS:
                TYPE_CHECK();
                PRINT_INSTR(7, "subs" NON_VAR EOL, EMPTY_STR);
                break;
            case T_MUL:
                TYPE_CHECK();
                PRINT_INSTR(8, "muls" NON_VAR EOL, EMPTY_STR);
                break;
            case T_DIV:
                TYPE_CHECK();
                IS_DIV_ZERO();
                PRINT_INSTR(9, "divs" NON_VAR EOL, EMPTY_STR);
                break;
            case T_DIV_INT:
                TYPE_CHECK();
                IS_DIV_ZERO();
                PRINT_INSTR(10, "idivs" NON_VAR EOL, EMPTY_STR);
                break;
            case T_LT:
                //IS_NIL();
                PRINT_INSTR(11, "lts" NON_VAR EOL, EMPTY_STR);
                break;
            case T_GT:
                //IS_NIL();
                PRINT_INSTR(12, "gts" NON_VAR EOL, EMPTY_STR);
                break;
            case T_LE:
                //IS_NIL();
                PRINT_INSTR(13, "\npops GF@$var2%s", "");
                PRINT_INSTR(14, "\npops GF@$var1%s", "");

                PRINT_INSTR(15, "\npushs GF@$var1%s", "");
                PRINT_INSTR(16, "\npushs GF@$var2%s", "");
                PRINT_INSTR(17, "\nlts%s", "");

                PRINT_INSTR(18, "\npushs GF@$var1%s", "");
                PRINT_INSTR(19, "\npushs GF@$var2%s", "");
                PRINT_INSTR(20, "\neqs%s", "");

                PRINT_INSTR(21, "\nors%s", "");
                break;
            case T_GE:
                // IS_NIL();
                PRINT_INSTR(22, "\npops GF@$var2%s", "");
                PRINT_INSTR(23, "\npops GF@$var1%s", "");

                PRINT_INSTR(24, "\npushs GF@$var1%s", "");
                PRINT_INSTR(25, "\npushs GF@$var2%s", "");
                PRINT_INSTR(26, "\ngts%s", "");

                PRINT_INSTR(27, "\npushs GF@$var1%s", "");
                PRINT_INSTR(28, "\npushs GF@$var2%s", "");
                PRINT_INSTR(29, "\neqs%s", "");

                PRINT_INSTR(30, "\nors%s", "");
                break;
            case T_EQ:
                PRINT_INSTR(31, "\neqs%s", "");
                break;
            case T_NEQ:
                PRINT_INSTR(32, "\neqs%s", "");
                PRINT_INSTR(33, "\nnots%s", "");
                break;
            case T_LENGTH:
                //IS_NIL(); // todo??
                break;
            case T_CONCAT:
                //IS_NIL(); // todo??
                break;
            default:
                break;
        }
        queue_remove(queue_expr);
    }
    PRINT_INSTR(18, "\npops LF@%s", queue_id->front->id->key_id);

    FILE *testik = fopen("testik.out", "w");
    fprintf(testik, "toto je ifj21: \n%s", ifj_code.str);
    fclose(testik);
    return true;
}



