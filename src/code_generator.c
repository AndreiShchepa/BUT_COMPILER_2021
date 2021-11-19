#include <string.h>
#include "code_generator.h"
#include "str.h"
#include "queue.h"
#include "symtable.h"
#include "parser.h"

#define FUNC_TOINTEGER                                                        \
"\nlabel $tointeger # tointeger(f : number) : integer						"\
"\n	# start																	"\
"\n	createframe			# new TF											"\
"\n	pushframe			# TF => LF											"\
"\n																			"\
"\n	# logic																	"\
"\n	defvar		LF@ret1														"\
"\n	defvar		LF@$tointeger_var_type										"\
"\n																			"\
"\n	pops 		LF@ret1														"\
"\n	TYPE		LF@$tointeger_var_type LF@ret1								"\
"\n																			"\
"\n	jumpifneq 	$tointeger_end LF@$tointeger_var_type string@float			"\
"\n																			"\
"\n	label t_float															"\
"\n	float2int 	LF@ret1 	LF@ret1											"\
"\n	jump 		$tointeger_end												"\
"\n																			"\
"\n	## end																	"\
"\n	label $tointeger_end													"\
"\n	pushs LF@ret1															"\
"\n	popframe 			# LF => TF											"\
"\n	return																	"

#define FUNC_READI                                                        \
"\nlabel $readi # readi() : integer										"\
"\n	## start															"\
"\n	createframe															"\
"\n	pushframe															"\
"\n																		"\
"\n	## logic															"\
"\n	defvar		LF@readi_ret1											"\
"\n	read 		LF@readi_ret1 int										"\
"\n	pushs 		LF@readi_ret1											"\
"\n																		"\
"\n	## end																"\
"\n	popframe															"\
"\n	return																"\

#define FUNC_READN                                                        \
"\nlabel $readn # readn() : number										"\
"\n	# start																"\
"\n	createframe															"\
"\n	pushframe															"\
"\n																		"\
"\n	# logic																"\
"\n	defvar 		LF@readn_ret1											"\
"\n	read 		LF@readn_ret1 float										"\
"\n	pushs 		LF@readn_ret1											"\
"\n																		"\
"\n	# end																"\
"\n	popframe															"\
"\n	return																"\

#define FUNC_READS                                                        \
"\nlabel $reads # reads() : string										"\
"\n	# start																"\
"\n	createframe															"\
"\n	pushframe															"\
"\n																		"\
"\n	# logic																"\
"\n	defvar 		LF@reads_ret1											"\
"\n	read 		LF@reads_ret1 string									"\
"\n	pushs  		LF@reads_ret1											"\
"\n																		"\
"\n	# end																"\
"\n	popframe															"\
"\n	return																"\

#define FUNC_WRITE                                                                                                \
"\nlabel $write # write(... : string | integer | number | boolean)  -- podpora boolean pro bonusove rozsireni	"\
"\n	# start																										"\
"\n	createframe																									"\
"\n	pushframe																									"\
"\n																												"\
"\n	# logic																										"\
"\n	defvar 		LF@write_var_type																				"\
"\n	defvar 		LF@write_var_print																				"\
"\n																												"\
"\n	label $write_while1																							"\
"\n	pops 		LF@write_var_print																				"\
"\n	TYPE		LF@write_var_type 	LF@write_var_print 															"\
"\n	jumpifeq 	$write_label_t_nil 	LF@write_var_type string@nil												"\
"\n	jumpifeq 	$write_label_string	LF@write_var_type string@string												"\
"\n	write 		LF@write_var_print																				"\
"\n	jump 		$write_while1																					"\
"\n																												"\
"\n	label $write_label_string																					"\
"\n	jumpifeq 	$write_label_end	LF@write_var_print GF@bottom_of_stack										"\
"\n	write 		LF@write_var_print																				"\
"\n	jump 		$write_while1																					"\
"\n																												"\
"\n	label 		$write_label_t_nil																				"\
"\n	write 		string@nil																						"\
"\n	jump 		$write_while1																					"\
"\n																												"\
"\n	# end																										"\
"\n	label $write_label_end																						"\
"\n	popframe																									"\
"\n	return																										"\


#define FUNC_SUBSTR                                                        \
"\nlabel $substr # substr(str : string, i : integer, j : integer) : string 	"\
"\n	# start																	"\
"\n	createframe																"\
"\n	pushframe																"\
"\n																			"\
"\n	# logic																	"\
"\n	defvar 	LF@substr_param1												"\
"\n	defvar 	LF@substr_param2												"\
"\n	defvar 	LF@substr_param3												"\
"\n	defvar 	LF@substr_ret1													"\
"\n	defvar 	LF@substr_len													"\
"\n	defvar 	LF@substr_while_cnt												"\
"\n	defvar 	LF@substr_char													"\
"\n	defvar 	LF@substr_cmp													"\
"\n																			"\
"\n	pops 	LF@substr_param1												"\
"\n	pops 	LF@substr_param2												"\
"\n	pops 	LF@substr_param3												"\
"\n	move 	LF@substr_ret1 		string@										"\
"\n	strlen 	LF@substr_len 		LF@substr_param1							"\
"\n	move 	LF@substr_while_cnt	int@0										"\
"\n	move 	LF@substr_char 		string@										"\
"\n	move 	LF@substr_cmp 		bool@false									"\
"\n																			"\
"\n	# j > i																	"\
"\n	gt 			LF@substr_cmp 		LF@substr_param2 	LF@substr_param3	"\
"\n	JUMPIFEQ 	$substr_label_end2 	LF@substr_cmp 		bool@true			"\
"\n	# i < 0																	"\
"\n	lt 			LF@substr_cmp 		LF@substr_param2 	int@0				"\
"\n	JUMPIFEQ 	$substr_label_end2 	LF@substr_cmp 		bool@true			"\
"\n	# j < 0																	"\
"\n	lt 			LF@substr_cmp 		LF@substr_param3 	int@0				"\
"\n	JUMPIFEQ 	$substr_label_end2 	LF@substr_cmp 		bool@true			"\
"\n	# i > len-1																"\
"\n	lt 			LF@substr_cmp 		LF@substr_param2 	LF@substr_len		"\
"\n	JUMPIFNEQ 	$substr_label_end2 	LF@substr_cmp 		bool@true			"\
"\n	# j > len-1																"\
"\n	lt 			LF@substr_cmp 		LF@substr_param3 	LF@substr_len		"\
"\n	JUMPIFNEQ 	$substr_label_end2 	LF@substr_cmp 		bool@true			"\
"\n																			"\
"\n																			"\
"\n	label $substr_while1													"\
"\n	getchar  LF@substr_char 		LF@substr_param1 	LF@substr_param2	"\
"\n	concat   LF@substr_ret1 		LF@substr_ret1		LF@substr_char		"\
"\n	jumpifeq $substr_label_end 		LF@substr_param2 	LF@substr_param3	"\
"\n	add 	 LF@substr_while_cnt	LF@substr_while_cnt int@1				"\
"\n	add 	 LF@substr_param2 		LF@substr_param2 	int@1				"\
"\n	jump 	 $substr_while1													"\
"\n																			"\
"\n	# end																	"\
"\n	label $substr_label_end													"\
"\n	clears																	"\
"\n	pushs 	GF@bottom_of_stack												"\
"\n	pushs 	LF@substr_ret1													"\
"\n	jump 	$substr_label_end3												"\
"\n																			"\
"\n	# end 2																	"\
"\n	label $substr_label_end2												"\
"\n	clears																	"\
"\n	pushs 	GF@bottom_of_stack												"\
"\n	pushs 	nil@nil															"\
"\n																			"\
"\n	# end 3																	"\
"\n	label $substr_label_end3												"\
"\n	popframe																"\
"\n	return																	"\

#define FUNC_ORD                                                        \
"\nlabel $ord # ord(s : string, i : integer) : integer					"\
"\n	# start																"\
"\n	createframe															"\
"\n	pushframe															"\
"\n																		"\
"\n	# logic																"\
"\n	defvar 		LF@ord_param1											"\
"\n	defvar 		LF@ord_param2											"\
"\n	defvar 		LF@ord_cmp												"\
"\n	defvar 		LF@ord_ret1												"\
"\n	defvar 		LF@ord_len												"\
"\n																		"\
"\n	pops 		LF@ord_param1											"\
"\n	pops 		LF@ord_param2											"\
"\n	move 		LF@ord_ret1 	bool@false								"\
"\n	move 		LF@ord_ret1 	nil@nil									"\
"\n	strlen		LF@ord_len 		LF@ord_param1							"\
"\n																		"\
"\n	gt 			LF@ord_cmp 		LF@ord_param2 	int@255					"\
"\n	JUMPIFEQ 	$ord_label_end 	LF@ord_cmp 		bool@true				"\
"\n	lt 			LF@ord_cmp 		LF@ord_param2 	int@0					"\
"\n	JUMPIFEQ 	$ord_label_end 	LF@ord_cmp 		bool@true				"\
"\n	lt 			LF@ord_cmp 		LF@ord_param2	LF@ord_len				"\
"\n	JUMPIFNEQ 	$ord_label_end 	LF@ord_cmp 		bool@true				"\
"\n																		"\
"\n	stri2int 	LF@ord_ret1 LF@ord_param1 LF@ord_param2					"\
"\n																		"\
"\n	label $ord_label_end												"\
"\n	pushs 		LF@ord_ret1												"\
"\n																		"\
"\n	# end																"\
"\n	popframe															"\
"\n	return																"\

#define FUNC_CHR                                                        \
"\nlabel $chr # chr(i : integer) : string								"\
"\n	## start															"\
"\n	createframe															"\
"\n	pushframe															"\
"\n																		"\
"\n	# logic																"\
"\n	defvar 		LF@chr_param1											"\
"\n	defvar 		LF@chr_ret1												"\
"\n	defvar 		LF@chr_cmp												"\
"\n																		"\
"\n	pops 		LF@chr_param1											"\
"\n	move 		LF@chr_ret1 	nil@nil									"\
"\n																		"\
"\n	gt 			LF@chr_cmp 		LF@chr_param1 	int@255					"\
"\n	JUMPIFEQ 	$chr_label_end 	LF@chr_cmp 		bool@true				"\
"\n	lt 			LF@chr_cmp 		LF@chr_param1 	int@0					"\
"\n	JUMPIFEQ 	$chr_label_end 	LF@chr_cmp 		bool@true				"\
"\n																		"\
"\n	int2char 	LF@chr_ret1 	LF@chr_param1							"\
"\n																		"\
"\n	## end																"\
"\n	label $chr_label_end												"\
"\n	pushs 		LF@chr_ret1												"\
"\n	popframe															"\
"\n	return																"\

//todo SKONTROLOVAT
#define FUNC_DIV_ZERO   \
"\nlabel $div_zero"     \
"\n	## start"           \
"\n exit int@9"

#define FUNC_OP_NIL     \
"\nlabel $op_nil"     \
"\n	## start"           \
"\n exit int@8"


/******************************************************************************
  *									MACROS
*****************************************************************************/
#define IFJ_CODE_START_LEN 10000
#define INSTR_LEN 100
#define EOL "\n"

#define PRINT_INSTR(num, fmt, ...)                                      \
    do {                                                                \
        char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + 2)];   \
        sprintf(instr##num, (fmt), __VA_ARGS__);                        \
        str_concat_str2(&ifj_code, instr##num);                         \
    } while(0)

#define IS_NIL(num) \
PRINT_INSTR(a, "\n\npops GF@$var2%s", "");                          \
PRINT_INSTR(b, "\n\npops GF@$var1%s", "");                          \
PRINT_INSTR(c, "\ntype GF@$type1 GF@$var2%s", "");                  \
PRINT_INSTR(d, "\njumpifeq $op_nil GF@$type1 string@nil%s", "");    \
PRINT_INSTR(e, "\njumpifeq $op_nil GF@$type2 string@nil%s", "");    \
PRINT_INSTR(f, "\n\njumpifeq $continue%d GF@$type1 GF@$type2%s", (num), "");    \
PRINT_INSTR(g, "\ncall$retyping%s", "");                                \
PRINT_INSTR(h, "\n\nlabel $continue%s", "");                         \
PRINT_INSTR(i, "\n\npushs GF@$var1%s", "");                             \
PRINT_INSTR(j, "\npushs GF@$var2%s", "");

#define IS_DIV_ZERO() \
PRINT_INSTR(aa, "\n\npops GF@$var2%s", "");              \
PRINT_INSTR(bb, "\njumpifeq $div_zero GF@$var2 int@0%s", ""); \
PRINT_INSTR(cc, "\npushs GF@$var2%s", "");

/******************************************************************************
  *									GLOBAL VARS
******************************************************************************/
string_t ifj_code;
extern Queue* queue_id;
extern Queue* queue_expr;
/******************************************************************************
  *									FUNCTIONS
******************************************************************************/
void gen_file_start() {
    return;
}

void gen_int2char(Queue *queue, token_t *symb_1) {
    (void )queue;
    (void )symb_1;
//    PRINT_INSTR(0, "INT2CHAR LF@%s LF@%s", queue->front->id->key_id, symb_1->attr.id, NULL);
}

void gen_func_label() {

}

void gen_init_built_ins() {
//    PRINT_INSTR(FUNC_TOINTEGER);
//    PRINT_INSTR(FUNC_READI);
//    PRINT_INSTR(FUNC_READN);
//    PRINT_INSTR(FUNC_READS);
//    PRINT_INSTR(FUNC_WRITE);
//    PRINT_INSTR(FUNC_SUBSTR);
//    PRINT_INSTR(FUNC_ORD);
//    PRINT_INSTR(FUNC_CHR);
}

void gen_label_item() {
    // TODO -
    // defvar
    // pop
}

void gen_while_start() {

}

void gen_while_end() {

}

void gen_param(/*TODO*/) {
//    PRINT_INSTR(0, "defvar LF@%s", ""/*TODO - func_name + id_param*/);
}

void gen_if_start(/*TODO*/) {
//    PRINT_INSTR(1, "label if%s", ""/*TODO - func_name + id_of_if*/);
}

void gen_if_else(/*TODO*/) {
//    PRINT_INSTR(2, "label else%s", ""/*TODO - func_name + id_of_if*/);
}

void gen_if_end(/*TODO*/) {
//    PRINT_INSTR(3, "label end%s", ""/*TODO - func_name + id_of_if*/);
}

void gen_func_start(/*TODO*/) {
//    PRINT_INSTR(1, "label@$%s%s%s", "a", "a", "a");
//    PRINT_INSTR(2, "label@$%s%s%s", "a", "a", "a");
//    PRINT_INSTR(3, "pushframe%s", "\n" /*TODO id*/);
}

void gen_func_end() {
//    PRINT_INSTR(0, "return%s", "\n");
}

void code_gen() {
    str_init(&ifj_code, IFJ_CODE_START_LEN);
    PRINT_INSTR(1, EOL"%s %d", "ahoj", 1);
    PRINT_INSTR(2, EOL"%s %d", "ahoj", 1);
    PRINT_INSTR(3, EOL"%s", ""); // new_line
    PRINT_INSTR(4, EOL"%s", ""); // new_line
    fprintf(stdout, "%d", snprintf(NULL, 0, "%s", "sacqs\0"));

    FILE *test_file = fopen("test_file.out", "w");
    if (!test_file)
        return;
    fprintf(test_file, "%s", ifj_code.str);
    fclose(test_file);

    // todo - header
    //    gen_init_built_ins();
    return;
}

void gen_expression() {
    str_init(&ifj_code, IFJ_CODE_START_LEN);

    while (!queue_isEmpty(queue_expr)) {
        switch (queue_expr->front->token->type) {
            case T_ID:
                PRINT_INSTR(1, "\npushs LF@%s", queue_expr->front->token->attr.id.str);
                break;
            case T_INT:
                PRINT_INSTR(2, "\npushs int@%llu", queue_expr->front->token->attr.num_i);
                break;
            case T_FLOAT:
                PRINT_INSTR(1, "\npushs float@%f", queue_expr->front->token->attr.num_f);
                break;
            case T_STRING:
                PRINT_INSTR(1, "\npushs string@%s", queue_expr->front->token->attr.id.str);
                break;
            case T_NONE: //todo nil???
                PRINT_INSTR(1, "\npushs nil@nil%s", "");
                break;
            case T_PLUS:
                IS_NIL(1);
                PRINT_INSTR(2, "\nadds%s", "");
                break;
            case T_MINUS:
                IS_NIL(2);
                PRINT_INSTR(3, "\nsubs%s", "");
                break;
            case T_MUL:
                IS_NIL(3);
                PRINT_INSTR(4, "\nmuls%s", "");
                break;
            case T_DIV:
                IS_NIL(4);
                IS_DIV_ZERO();
                PRINT_INSTR(5, "\ndivs%s", "");
                break;
            case T_DIV_INT:
                IS_NIL(5);
                IS_DIV_ZERO();
                PRINT_INSTR(6, "\nidivs%s", "");
                break;
            case T_LT:
                IS_NIL(6);
                PRINT_INSTR(7, "\nlts%s", "");
                break;
            case T_GT:
                IS_NIL(7);
                PRINT_INSTR(8, "\ngts%s", "");
                break;
            case T_LE:
                IS_NIL(8);
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
               // IS_NIL();
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
}
