/**
 * Project: Compiler IFJ21
 *
 * @file code_generator.h
 *
 * @brief Implement generation code of IFJcode21
 *
 * @author  Andrej Binovsky     <xbinov00>
 *          Zdenek Lapes        <xlapes02>
 */

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include "error.h"
#include "str.h"
#include "queue.h"
#include "symtable.h"
#include "symstack.h"
#include "parser.h"


/******************************************************************************
  *									MACROS
*****************************************************************************/
#define BLOCKS_NUM 3

#define DEBUG_ANDREJ 0
#define DEBUG_ZDENEK 0

#define DEBUG_INSTR                 1
#define DEBUG_BUILT_IN              1
#define DEBUG_INSTR_WITH_GEN_INFO   0

#define IFJ_CODE_START_LEN 10000
#define MAX_LINE_LEN       300
#define EOL                 "\n"
#define EMPTY_STR           ""
#define NON_VAR             "%s"
#define DEVIDER             "################################################################################# "
#define DEVIDER_2           "########## "
#define FORMAT_VAR          " LF@$%s$%lu$%s$ "
#define FORMAT_PARAM        " LF@%%%dp "
#define FORMAT_ARGS         " TF@%%%dp "
#define FORMAT_IF           " $%s$%d$if$ "
#define FORMAT_ELSE         " $%s$%d$else$ "
#define FORMAT_IF_END       " $%s$%d$if_end$ "
#define FORMAT_WHILE        " $%s$%d$while$ "
#define FORMAT_WHILE_END    " $%s$%d$while_end$ "


#define INIT_CONCAT_STR(num, fmt, ...)                                                  \
    do {                                                                                \
        cnt.instr++;                                                                    \
        if (!DEBUG_INSTR_WITH_GEN_INFO) {                                               \
            sprintf(instr##num, (fmt EOL), __VA_ARGS__);                                \
        } else {                                                                        \
            snprintf(instr##num, MAX_LINE_LEN, (fmt "%*s:%d:%s():%d" EOL), __VA_ARGS__, \
                                    80-snprintf(NULL, 0, (fmt), __VA_ARGS__),           \
                                    "#", __LINE__, __func__, cnt.instr);                \
        }                                                                               \
    } while(0)

#define PRINT_MAIN(num, fmt, ...)                                                \
    do {                                                                         \
        char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + MAX_LINE_LEN)]; \
        INIT_CONCAT_STR(num, fmt, __VA_ARGS__);                                  \
        if (!str_concat_str2(&ifj_code[MAIN], instr##num)) {                     \
            CHECK_INTERNAL_ERR(true, false);                                     \
        }                                                                        \
    } while(0)

#define PRINT_FUNC(num, fmt, ...)                                                           \
    do {                                                                                    \
        char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + MAX_LINE_LEN)];            \
        INIT_CONCAT_STR(num, fmt, __VA_ARGS__);                                             \
        if (!str_concat_str2(&ifj_code[(cnt.in_while) ? WHILE : FUNCTIONS], instr##num)) {  \
            CHECK_INTERNAL_ERR(true, false);                                                \
        }                                                                                   \
    } while(0)

#define PRINT_WHILE(num, fmt, ...)                                               \
    do {                                                                         \
        char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + MAX_LINE_LEN)]; \
        INIT_CONCAT_STR(num, fmt, __VA_ARGS__);                                  \
        if (!str_concat_str2(&ifj_code[WHILE], instr##num)) {                    \
            CHECK_INTERNAL_ERR(true, false);                                     \
        }                                                                        \
    } while(0)

#define PRINT_FUNC_BUILT_IN(num, fmt, ...)                                       \
    do {                                                                         \
        char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + MAX_LINE_LEN)]; \
        sprintf(instr##num, (fmt EOL), __VA_ARGS__);                             \
        if (!str_concat_str2(&ifj_code[FUNCTIONS], instr##num)) {                \
            CHECK_INTERNAL_ERR(true, false);                                     \
        }                                                                        \
    } while(0)


#ifdef DEBUG_INSTR
#define DEBUG_PRINT_INSTR(num, NUM_BLOCK ,fmt, ...)                                  \
		do {                                                                         \
			char instr##num[(snprintf(NULL, 0, (fmt), __VA_ARGS__) + MAX_LINE_LEN)]; \
			INIT_CONCAT_STR(num, fmt, __VA_ARGS__);                                  \
			if (!str_concat_str2(&ifj_code[NUM_BLOCK], instr##num)) {                \
                CHECK_INTERNAL_ERR(true, false);                                     \
			}                                                                        \
		} while(0)
#else
#define DEBUG_PRINT_INSTR(num, fmt, ...)
#endif // DEBUG_INSTR

#define PRINT_WHERE(...)                          \
    do {                                          \
        if (strcmp(cnt.func_name.str, "") == 0) { \
                PRINT_MAIN(__VA_ARGS__);          \
        } else {                                  \
                PRINT_FUNC(__VA_ARGS__);          \
        }                                         \
    } while(0)


#define SWITCH_CASE(number)                                     \
        case number:                                            \
            sprintf(tmp_str, "%d", number);                     \
            CODE_GEN(str_add_char, &str_out, '\\');             \
            CODE_GEN(str_add_char, &str_out, '0');              \
            if (tmp_str[1] == '\0'){                            \
                 CODE_GEN(str_add_char, &str_out, '0');         \
                 CODE_GEN(str_add_char, &str_out, tmp_str[0]);  \
            }                                                   \
            else{                                               \
                CODE_GEN(str_add_char, &str_out, tmp_str[0]);   \
                CODE_GEN(str_add_char, &str_out, tmp_str[1]);   \
            }                                                   \
            break;

#define CODE_GEN(callback, ...)              \
    do {                                     \
        if (!(callback)(__VA_ARGS__)) {      \
            CHECK_INTERNAL_ERR(true, false); \
        }                                    \
    } while(0)

#define QUEUE_ADD_ID(where_is_id_key)                                 \
    do {                                                              \
        if (strcmp(cnt.func_call.str, "write") == 0) {                \
               break;                                                 \
        } else if (!queue_add_id_rear(queue_id, (where_is_id_key))) { \
            CHECK_INTERNAL_ERR(true, false);                          \
        }                                                             \
    } while(0)

#define QUEUE_ADD_ARGS(where_is_id_key)                          \
    do {                                                         \
        if (!queue_add_id_rear(queue_args, (where_is_id_key))) { \
            CHECK_INTERNAL_ERR(true, false);                     \
        }                                                        \
    } while(0)


/******************************************************************************
  *									STRUCTS
*****************************************************************************/

typedef struct cnts_s {
    string_t func_name;
    string_t func_call;
    unsigned int param_cnt;
    unsigned int if_cnt_max;
    unsigned int while_cnt;
    unsigned int while_cnt_max;
    unsigned int while_cnt_deep;
    unsigned int deep;
    unsigned int ret_vals;
    unsigned int instr;
    bool 		 in_return;
    bool         in_while;
} cnts_t;

extern cnts_t cnt;
extern string_t ifj_code[BLOCKS_NUM];

enum block_e {FUNCTIONS, MAIN, WHILE};

typedef long long unsigned int llu_t;

/******************************************************************************
  *									FUNCTIONS
*****************************************************************************/
/**
 * @brief print generated .ifjcode on stdout
 * @return On success True, otherwise False
 */
bool gen_testing_helper();

/**
 * @brief allocates all strings for generating .ifjcode
 * @return On success True, otherwise False
 */
bool alloc_ifj_code();

/**
 * @brief clears all dynamic strings
 * @return On success True, otherwise False
 */
bool init_ifj_code();

/**
 * @brief allocates strings in cnt structure
 * @return On success True, otherwise False
 */
bool alloc_cnt();

/**
 * @brief initialize cnt structure
 * @return On success True, otherwise False
 */
bool init_cnt();

/**
 * @brief
 * @return On success True, otherwise False
 */
bool gen_init();

/**
 * @brief generate all built-in functions into string string fo functions
 * @return On success True, otherwise False
 */
bool gen_init_built_ins();

/**
 * @brief Generate declaration of variable and initialize it with nil
 * @return On success True, otherwise False
 */
bool gen_def_var();

/**
 * @brief Generate initialization of variable with value from stack
 * @return On success True, otherwise False
 */
bool gen_init_var();

/**
 * @brief Generate label if
 * @return On success True, otherwise False
 */
bool gen_if_start();

/**
 * @brief Generate label else
 * @return On success True, otherwise False
 */
bool gen_if_else();

/**
 * @brief Generate label end of if else
 * @return On success True, otherwise False
 */
bool gen_if_end();

/**
 * @brief Generate evaluation of if expression
 * @return On success True, otherwise False
 */
bool gen_if_eval();

/**
 * @brief Generate if jump based on result of evaluation
 * @return On success True, otherwise False
 */
bool gen_if_end_jump();

/**
 * @brief generate while label
 * @param key_id
 * @return On success True, otherwise False
 */
bool gen_while_label();

/*  *
 * @brief Generate evaluating for condition of while
 * @param key_id
 * @return On success True, otherwise False
 */
bool gen_while_eval();

/**
 * @brief generate label where to jump if condition for repeting while cycle is false
 * @return On success True, otherwise False
 */
bool gen_while_end();

/**
 * @brief concatenate code for variables outside while with while logic
 * @return On success True, otherwise False
 */
bool gen_concat_while_functions();

/**
 * @brief generate label for function with creating of LF
 * @param id
 * @return On success True, otherwise False
 */
bool gen_func_start(char *id);

/**
 * @brief destroy LF and return from function
 * @return On success True, otherwise False
 */
bool gen_func_end();

/**
 * @brief handle taking parameters for function
 * @return On success True, otherwise False
 */
bool gen_params();

/**
 * @brief generaet new TF for calling function
 * @return On success True, otherwise False
 */
bool gen_func_call_start();

/**
 * @brief create arguments for calling function on TF from variable
 * @param htab_item
 * @return On success True, otherwise False
 */
bool gen_func_call_args_var(htab_item_t *htab_item);

/**
 * @brief create arguments for calling function on TF from const
 * @param token
 * @return On success True, otherwise False
 */
bool gen_func_call_args_const(token_t *token);

/**
 * @brief generate call command for  write function or any other function
 * @return On success True, otherwise False
 */
bool gen_func_call_label();

/**
 * @brief decide where to print ifjcode (WHILE, MAIN, FUNCTIONS)
 * @return
 */
int where_to_print();

/**
 * @brief check if is calling funtion write
 * @return if is write return true, otherwise false
 */
bool is_write();

/**
 * @brief
 * @param str_in
 * @return On success True, otherwise False
 */
bool convert_str_to_ascii(string_t *str_in);

/**
 * @brief Generate push value nil on stack
 * @return On success True, otherwise False
 */
bool gen_retval_nil();

/**
 * @brief Generate expression and result push on stack
 * @return On success True, otherwise False
 */
bool gen_expression();

/**
 * @brief deallocate all string need for code_generator
 * @return On success True, otherwise False
 */
bool dealloc_gen_var();


/******************************************************************************
  *									BUILT-IN FUNCS
*****************************************************************************/
#define FUNC_CHECK_IS_NIL                 \
"\nlabel $check_is_nil"                   \
"\npops GF@&var1"                         \
"\ntype GF@&type1 GF@&var1"               \
"\njumpifeq $op_nil GF@&type1 string@nil" \
"\npushs GF@&var1"                        \
"\nreturn"

#define FUNC_TOINTEGER                                          \
"\nlabel $tointeger # tointeger(f : number) : integer"          \
"\npushframe # TF => LF"                                        \
"\ncreateframe	# new TF"                                       \
"\ndefvar LF@$tointeger$0p_type"                                \
"\npushs LF@%0p"                                                \
"\npops GF@&var1"                                               \
"\ntype GF@&type1 LF@%0p"                                       \
"\njumpifneq $tointeger$continue GF@&type1 string@nil"          \
"\npushs GF@&var1"                                              \
"\nreturn"                                                      \
"\nlabel $tointeger$continue"                                   \
"\npushs GF@&var1"                                              \
"\nTYPE	LF@$tointeger$0p_type LF@%0p"                           \
"\njumpifneq $tointeger$end LF@$tointeger$0p_type string@float" \
"\nfloat2ints"                                                  \
"\nlabel $tointeger$end"                                        \
"\npopframe 			# LF => TF"                             \
"\nreturn"

#define FUNC_READI                   \
"\nlabel $readi # readi() : integer" \
"\n## start"                         \
"\ncreateframe"                      \
"\npushframe	"                    \
"\n"                                 \
"\n## logic"                         \
"\ndefvar		LF@readi$ret1"       \
"\nread 		LF@readi$ret1 int"   \
"\npushs 		LF@readi$ret1"       \
"\n"                                 \
"\n## end"                           \
"\npopframe"                         \
"\nreturn		"

#define FUNC_READN                \
"label $readn # readn() : number" \
"\npushframe"                     \
"\ncreateframe"                   \
"\ndefvar LF@readn_ret1"          \
"\nread   LF@readn_ret1 float"    \
"\npushs  LF@readn_ret1"          \
"\npopframe"                      \
"\nreturn"

#define FUNC_READS                    \
"label $reads # reads() : string	" \
"\npushframe"                         \
"\ncreateframe"                       \
"\ndefvar 	LF@reads_ret1"            \
"\nread    LF@reads_ret1 string"      \
"\npushs   LF@reads_ret1"             \
"\npopframe"                          \
"\nreturn"

#define FUNC_WRITE                                                 \
"label $write"                                                     \
"\npushframe"                                                      \
"\ncreateframe"                                                    \
"\ndefvar 		LF@&write_var_type"                                \
"\ntype		    LF@&write_var_type 	LF@%0p"                        \
"\njumpifeq 	$write$type$nil 	LF@&write_var_type string@nil" \
"\nwrite 		LF@%0p"                                            \
"\njump 		$write$end"                                        \
"\nlabel 		$write$type$nil"                                   \
"\nwrite 		string@nil"                                        \
"\nlabel $write$end"                                               \
"\npopframe"                                                       \
"\nreturn"

#define FUNC_SUBSTR                                                          \
"\nlabel $substr # substr(str : string, i : integer, j : integer) : string " \
"\n# start"                                                                  \
"\npushframe	"                                                            \
"\ncreateframe"                                                              \
"\n"                                                                         \
"\n# logic"                                                                  \
"\ndefvar 	LF@substr$str"                                                   \
"\ndefvar 	LF@substr$i"                                                     \
"\ndefvar 	LF@substr$j"                                                     \
"\ndefvar 	LF@substr$ret1"                                                  \
"\ndefvar 	LF@substr$len"                                                   \
"\ndefvar 	LF@substr$while$cnt"                                             \
"\ndefvar 	LF@substr$char"                                                  \
"\ndefvar 	LF@substr$cmp"                                                   \
"\n"                                                                         \
"\npushs LF@%0p"                                                             \
"\ncall  $check_is_nil"                                                      \
"\npops  LF@substr$str"                                                      \
"\npushs LF@%1p"                                                             \
"\ncall  $check_is_nil"                                                      \
"\npops  LF@substr$i"                                                        \
"\npushs LF@%2p"                                                             \
"\ncall  $check_is_nil"                                                      \
"\npops  LF@substr$j"                                                        \
"\n"                                                                         \
"\nmove 	LF@substr$ret1 		string@"                                     \
"\nstrlen 	LF@substr$len 		LF@substr$str"                               \
"\nmove 	LF@substr$while$cnt	int@0"                                       \
"\nmove 	LF@substr$char 		string@"                                     \
"\nmove 	LF@substr$cmp 		bool@false"                                  \
"\n"                                                                         \
"\nsub LF@substr$i LF@substr$i int@1"                                        \
"\nsub LF@substr$j LF@substr$j int@1"                                        \
"\n# i > j"                                                                  \
"\ngt 			LF@substr$cmp 		LF@substr$i	 	LF@substr$j"             \
"\nJUMPIFEQ 	$substr_label_end2 	LF@substr$cmp 	bool@true"               \
"\n# i < 0"                                                                  \
"\nlt 			LF@substr$cmp 		LF@substr$i 	int@0"                   \
"\nJUMPIFEQ 	$substr_label_end2 	LF@substr$cmp 	bool@true"               \
"\n# j < 0"                                                                  \
"\nlt 			LF@substr$cmp 		LF@substr$j 	int@0"                   \
"\nJUMPIFEQ 	$substr_label_end2 	LF@substr$cmp 	bool@true"               \
"\n# i > len-1"                                                              \
"\nlt 			LF@substr$cmp 		LF@substr$i 	LF@substr$len"           \
"\nJUMPIFNEQ 	$substr_label_end2 	LF@substr$cmp 	bool@true"               \
"\n# j > len-1"                                                              \
"\nlt 			LF@substr$cmp 		LF@substr$j 	LF@substr$len"           \
"\nJUMPIFNEQ 	$substr_label_end2 	LF@substr$cmp 	bool@true"               \
"\n"                                                                         \
"\n"                                                                         \
"\nlabel $substr_while1"                                                     \
"\ngetchar  LF@substr$char 		LF@substr$str 		LF@substr$i"             \
"\nconcat   LF@substr$ret1 		LF@substr$ret1		LF@substr$char"          \
"\njumpifeq $substr_label_end 		LF@substr$i 	    LF@substr$j"         \
"\nadd 	 LF@substr$while$cnt	LF@substr$while$cnt int@1"                   \
"\nadd 	 LF@substr$i   			LF@substr$i 		int@1"                   \
"\njump 	 $substr_while1"                                                 \
"\n"                                                                         \
"\n# end"                                                                    \
"\nlabel $substr_label_end"                                                  \
"\npushs 	LF@substr$ret1"                                                  \
"\npopframe"                                                                 \
"\nreturn"                                                                   \
"\n"                                                                         \
"\n# end 2"                                                                  \
"\nlabel $substr_label_end2"                                                 \
"\npushs 	string@"                                                         \
"\npopframe"                                                                 \
"\nreturn"

#define FUNC_ORD                                        \
"\nlabel $ord # ord(s : string, i : integer) : integer" \
"\n# start"                                             \
"\npushframe"                                           \
"\ncreateframe"                                         \
"\n"                                                    \
"\n# logic"                                             \
"\ndefvar 		LF@ord$s"                               \
"\ndefvar 		LF@ord$i"                               \
"\ndefvar 		LF@ord$cmp"                             \
"\ndefvar 		LF@ord$ret1"                            \
"\ndefvar 		LF@ord$len"                             \
"\n"                                                    \
"\nmove 		LF@ord$s		nil@nil"                \
"\nmove 		LF@ord$i		nil@nil"                \
"\nmove 		LF@ord$cmp		nil@nil"                \
"\nmove 		LF@ord$ret1		nil@nil"                \
"\nmove 		LF@ord$len		nil@nil"                \
"\n"                                                    \
"\n"                                                    \
"\npushs LF@%0p"                                        \
"\ncall $check_is_nil"                                  \
"\npops LF@ord$s"                                       \
"\npushs LF@%1p"                                        \
"\ncall $check_is_nil"                                  \
"\npops LF@ord$i"                                       \
"\n"                                                    \
"\nstrlen		LF@ord$len 		LF@ord$s"               \
"\n"                                                    \
"\n# if i <= 0 or i > #s then"                          \
"\nsub 		LF@ord$i LF@ord$i int@1"                    \
"\ngt 			LF@ord$cmp 		LF@ord$i 	LF@ord$len" \
"\nJUMPIFEQ 	$ord_label_end 	LF@ord$cmp 	bool@true"  \
"\nlt 			LF@ord$cmp 		LF@ord$i 	int@0"      \
"\nJUMPIFEQ 	$ord_label_end2	LF@ord$cmp 	bool@true"  \
"\nlt 			LF@ord$cmp 		LF@ord$i	LF@ord$len" \
"\nJUMPIFNEQ 	$ord_label_end2	LF@ord$cmp	bool@true"  \
"\n"                                                    \
"\nstri2int 	LF@ord$ret1 LF@ord$s LF@ord$i"          \
"\n"                                                    \
"\nlabel $ord_label_end"                                \
"\npushs LF@ord$ret1"                                   \
"\npopframe"                                            \
"\nreturn"                                              \
"\n"                                                    \
"\nlabel $ord_label_end2"                               \
"\npushs nil@nil"                                       \
"\npopframe"                                            \
"\nreturn"


#define FUNC_CHR                                       \
"\nlabel $chr # chr(i : integer) : string"             \
"\n## start"                                           \
"\npushframe	"                                      \
"\ncreateframe"                                        \
"\n"                                                   \
"\n# logic"                                            \
"\ndefvar 		LF@chr$i"                              \
"\ndefvar 		LF@chr$ret1"                           \
"\ndefvar 		LF@chr$cmp"                            \
"\n"                                                   \
"\npushs LF@%0p"                                       \
"\ncall $check_is_nil"                                 \
"\npops 		LF@chr$i"                              \
"\n"                                                   \
"\nmove 		LF@chr$ret1 nil@nil"                   \
"\nmove 		LF@chr$cmp 	bool@false"                \
"\n"                                                   \
"\ngt 			LF@chr$cmp 		LF@chr$i 	int@255"   \
"\nJUMPIFEQ 	$chr_label_end 	LF@chr$cmp 	bool@true" \
"\nlt 			LF@chr$cmp 		LF@chr$i 	int@0"     \
"\nJUMPIFEQ 	$chr_label_end 	LF@chr$cmp 	bool@true" \
"\n"                                                   \
"\nint2char 	LF@chr$ret1 	LF@chr$i"              \
"\npushs 		LF@chr$ret1"                           \
"\npopframe"                                           \
"\nreturn"                                             \
"\n"                                                   \
"\n## end"                                             \
"\nlabel $chr_label_end"                               \
"\npushs nil@nil"                                      \
"\npopframe"                                           \
"\nreturn"


/******************************************************************************
  *									AUXILIARY FUNCS
*****************************************************************************/
#define FUNC_OP_NIL \
"\nlabel $op_nil"   \
"\nexit int@8"

#if 0
"\ncreateframe"\
"\ndefvar TF@%0p"\
"\nmove TF@%0p string@ERROR\\0328:\\032Unexpected\\032nil\\032value\\032in\\032the\\032parameter.\\010"\
"\ncall $write"
#endif

#define FUNC_RETYPING_VAR1      \
"\nlabel $retyping_var1"        \
"\nint2float GF@&var1 GF@&var1" \
"\nreturn\n"

#define FUNC_RETYPING_VAR2      \
"\nlabel $retyping_var2"        \
"\nint2float GF@&var2 GF@&var2" \
"\nreturn\n"

#define FUNC_CHECK_OP                                \
"\nlabel $check_op"                                  \
"\npops GF@&var2"                                    \
"\npops GF@&var1"                                    \
"\ntype GF@&type1 GF@&var1"                          \
"\ntype GF@&type2 GF@&var2"                          \
"\njumpifeq $op_nil GF@&type1 string@nil"            \
"\njumpifeq $op_nil GF@&type2 string@nil"            \
"\njumpifeq $continue_end_op GF@&type1 GF@&type2"    \
"\njumpifeq $continue_mid_op GF@&type1 string@float" \
"\ncall $retyping_var1"                              \
"\njump $continue_end_op"                            \
"\nlabel $continue_mid_op"                           \
"\ncall $retyping_var2"                              \
"\nlabel $continue_end_op"                           \
"\npushs GF@&var1"                                   \
"\npushs GF@&var2"                                   \
"\nreturn\n"

#define FUNC_CHECK_DIV                                \
"\nlabel $check_div"                                  \
"\npops GF@&var2"                                     \
"\npops GF@&var1"                                     \
"\ntype GF@&type1 GF@&var1"                           \
"\ntype GF@&type2 GF@&var2"                           \
"\njumpifeq $op_nil GF@&type1 string@nil"             \
"\njumpifeq $op_nil GF@&type2 string@nil"             \
"\njumpifeq $continue_mid_div GF@&type1 string@int"   \
"\njumpifeq $continue_end_div GF@&type2 string@float" \
"\ncall $retyping_var2"                               \
"\njump $continue_end_div"                            \
"\nlabel $continue_mid_div"                           \
"\ncall $retyping_var1"                               \
"\njumpifeq $continue_end_div GF@&type2 string@float" \
"\ncall $retyping_var2"                               \
"\nlabel $continue_end_div"                           \
"\njumpifeq $div_zero_error GF@&var2 float@0x0.0p+0"  \
"\npushs GF@&var1"                                    \
"\npushs GF@&var2"                                    \
"\nreturn"                                            \
"\nlabel $div_zero_error"                             \
"\nexit int@9\n"


#if 0
"\ncreateframe"\
"\ndefvar TF@%0p"\
"\nmove TF@%0p string@ERROR\\0329:\\032Unexpected\\032zero\\032division.\\010"\
"\ncall $write"
#endif

#define FUNC_CHECK_IDIV                      \
"\nlabel $check_idiv"                        \
"\npops GF@&var2"                            \
"\npops GF@&var1"                            \
"\ntype GF@&type1 GF@&var1"                  \
"\ntype GF@&type2 GF@&var2"                  \
"\njumpifeq $op_nil GF@&type1 string@nil"    \
"\njumpifeq $op_nil GF@&type2 string@nil"    \
"\njumpifeq $idiv_zero_error GF@&var2 int@0" \
"\npushs GF@&var1"                           \
"\npushs GF@&var2"                           \
"\nreturn"                                   \
"\nlabel $idiv_zero_error"                   \
"\nexit int@9\n"

#if 0
"\ncreateframe"\
"\ndefvar TF@%0p"\
"\nmove TF@%0p string@ERROR\\0329:\\032Unexpected\\032zero\\032division.\\010"\
"\ncall $write"
#endif

#define FUNC_CHECK_IS_NIL_CONCAT           \
"\nlabel $check_is_nil_concat"             \
"\ntype GF@&type1 GF@&var1"                \
"\ntype GF@&type2 GF@&var2"                \
"\njumpifeq $op_nil GF@&type1 string@nil"  \
"\njumpifeq $op_nil GF@&type2 string@nil"  \
"\nreturn"


#define FUNC_CHECK_COMP                                 \
"\nlabel $check_comp"                                   \
"\npops GF@&var2"                                       \
"\npops GF@&var1"                                       \
"\ntype GF@&type1 GF@&var1"                             \
"\ntype GF@&type2 GF@&var2"                             \
"\njumpifeq $continue_end_comp GF@&type1 string@nil"    \
"\njumpifeq $continue_end_comp GF@&type2 string@nil"    \
"\njumpifeq $continue_end_comp GF@&type1 string@string" \
"\njumpifeq $continue_end_comp GF@&type2 string@string" \
"\njumpifeq $continue_mid_comp GF@&type1 string@int"    \
"\njumpifeq $continue_end_comp GF@&type2 string@float"  \
"\ncall $retyping_var2"                                 \
"\njump $continue_end_comp"                             \
"\nlabel $continue_mid_comp"                            \
"\ncall $retyping_var1"                                 \
"\njumpifeq $continue_end_comp GF@&type2 string@float"  \
"\ncall $retyping_var2"                                 \
"\nlabel $continue_end_comp"                            \
"\npushs GF@&var1"                                      \
"\npushs GF@&var2"                                      \
"\nreturn\n"

#endif // CODE_GENERATOR_H
