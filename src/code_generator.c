#include "code_generator.h"

//todo SKONTROLOVAT
#define FUNC_DIV_ZERO   \
"\nlabel $div_zero"     \
"\n	## start"           \
"\n exit int@9"

#define FUNC_OP_NIL     \
"\nlabel $div_zero"     \
"\n	## start"           \
"\n exit int@8"


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

#define IS_NIL() \
PRINT_INSTR(a, "\npops GF@$var2%s", "");\
PRINT_INSTR(b, "\ntype GF@$type GF@$var2%s", "");\
PRINT_INSTR(c, "\njumpifeq GF@$type nil@$nil%s", "");\
PRINT_INSTR(d, "\npops GF@$var1%s", "");\
PRINT_INSTR(e, "\ntype GF@$type GF@$var2%s", "");\
PRINT_INSTR(f, "\njumpifeq GF@$type nil@$nil%s", "");\
PRINT_INSTR(h, "\npushs GF@$var1%s", "");\
PRINT_INSTR(i, "\npushs GF@$var2%s", "");

/******************************************************************************
  *									GLOBAL VARS
******************************************************************************/
string_t ifj_code;
extern Queue* queue_id;
extern Queue* queue_expr;
extern int err;
cnts_t cnt = {.func_name.str = NULL};

/******************************************************************************
  *									FUNCTIONS
******************************************************************************/
bool init_cnt() {
    if (cnt.func_name.str) {
        if (!str_init(&cnt.func_name, MAX_FUNC_LEN))
            return false;
    } else {
        str_free(&cnt.func_name);
        if (!str_init(&cnt.func_name, MAX_FUNC_LEN))
            return false;
    }
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

bool gen_while_start() {
    return true;
}

bool gen_while_end() {
    return true;
}

bool gen_params() {
    QueueElementPtr *queue_elem = queue_id->front;
    for (int i = 1; queue_elem; queue_elem = queue_elem->previous_element, i++) {
        PRINT_INSTR(1, "defvar LF@%s"        EOL    , queue_elem->id->key_id);
        PRINT_INSTR(1, "move LF@%s  LF@%dp " EOL EOL, queue_elem->id->key_id, i);
    }
    return true;
}

bool gen_param() {
    PRINT_INSTR(1, "defvar LF@%s"       EOL, queue_id->front->id->key_id);
    PRINT_INSTR(2, "move LF@%s LF@!p1"  EOL, queue_id->front->id->key_id);
    cnt.param_cnt++;
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
    PRINT_INSTR(1, "label $%s"                  EOL, id);
    PRINT_INSTR(2, "pushframe"          NON_VAR EOL, EMPTY_STR);
    PRINT_INSTR(3, "createframe"        NON_VAR EOL, EMPTY_STR);
    return true;
}

bool gen_func_end() {
    PRINT_INSTR(1, "popframe" NON_VAR, EOL);
    PRINT_INSTR(2, "return"   NON_VAR, EOL);
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
    fprintf(test_file, "%s", ifj_code.str);
    fclose(test_file);
    return true;
}

#if 0
bool gen_expression() {
    str_init(&ifj_code, IFJ_CODE_START_LEN);

    while (!queue_isEmpty(queue_expr)) {
        switch (queue_expr->front->token->type) {
            case T_ID:
                PRINT_INSTR(1, "\npushs LF@%s", queue_expr->front->token->attr.id.str);
                break;
            case T_INT:
                PRINT_INSTR(1, "\npushs int@%llu", queue_expr->front->token->attr.num_i);
                break;
            case T_FLOAT:
                PRINT_INSTR(1, "\npushs float@%f", queue_expr->front->token->attr.num_f);
                break;
            case T_STRING:
                PRINT_INSTR(1, "\npushs string@%s", queue_expr->front->token->attr.id.str);
                break;
            case T_NONE: //todo nil???  AND WHAT BOOL
                PRINT_INSTR(1, "\npushs nil@nil%s", "");
                break;
            case T_PLUS:
                IS_NIL();
                PRINT_INSTR(2, "\nadds%s", "");
                break;
            case T_MINUS:
                IS_NIL();
                PRINT_INSTR(3, "\nsubs%s", "");
                break;
            case T_MUL:
                IS_NIL();
                PRINT_INSTR(4, "\nmuls%s", "");
                break;
            case T_DIV:
                IS_NIL();
                PRINT_INSTR(5, "\ndivs%s", "");
                break;
            case T_DIV_INT:
                IS_NIL();
                PRINT_INSTR(6, "\nidivs%s", "");
                break;
            case T_LT:
                IS_NIL();
                PRINT_INSTR(7, "\nlts%s", "");
                break;
            case T_GT:
                IS_NIL();
                PRINT_INSTR(8, "\ngts%s", "");
                break;
            case T_LE:
                IS_NIL();
                PRINT_INSTR(9, "\npops GF@$var2%s", "");
                PRINT_INSTR(9, "\npops GF@$var1%s", "");

                PRINT_INSTR(9, "\npushs GF@$var1%s", "");
                PRINT_INSTR(9, "\npushs GF@$var2%s", "");
                PRINT_INSTR(9, "\nlts%s", "");

                PRINT_INSTR(9, "\npushs GF@$var1%s", "");
                PRINT_INSTR(9, "\npushs GF@$var2%s", "");
                PRINT_INSTR(10, "\neqs%s", "");

                PRINT_INSTR(11, "\nors%s", "");
                break;
            case T_GE:
                IS_NIL();
                PRINT_INSTR(9, "\npops GF@$var2%s", "");
                PRINT_INSTR(9, "\npops GF@$var1%s", "");

                PRINT_INSTR(9, "\npushs GF@$var1%s", "");
                PRINT_INSTR(9, "\npushs GF@$var2%s", "");
                PRINT_INSTR(9, "\ngts%s", "");

                PRINT_INSTR(9, "\npushs GF@$var1%s", "");
                PRINT_INSTR(9, "\npushs GF@$var2%s", "");
                PRINT_INSTR(10, "\neqs%s", "");

                PRINT_INSTR(11, "\nors%s", "");
                break;
            case T_EQ:
                PRINT_INSTR(15, "\neqs%s", "");
                break;
            case T_NEQ:
                PRINT_INSTR(16, "\neqs%s", "");
                PRINT_INSTR(17, "\nnots%s", "");
                break;
            case T_LENGTH:

                break;
            case T_CONCAT:
                break;
            default:
                break;
        }
        queue_remove(queue_expr);
    }
    PRINT_INSTR(18, "\nPOPS LF@%s", queue_id->front->id->key_id);

    FILE *testik = fopen("testik.out", "w");
    fprintf(testik, "toto je ifj21: \n%s", ifj_code.str);
    fclose(testik);
}
#endif
