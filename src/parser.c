/**
 * Project: Compiler IFJ21
 *
 * @file parser.c
 *
 * @brief Implement functions processing rules for syntax analysis
 *
 * @author  Andrei Shchapaniak  <xshcha00>
 *          Andrej Binovsky     <xbinov00>
 *          Zdenek Lapes        <xlapes02>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expressions.h"
#include "parser.h"
#include "error.h"
#include "str.h"
#include "queue.h"
#include "code_generator.h"
#include "symstack.h"

// token from file
token_t token;
// err code after each function
int err;
// number of scope where we are
unsigned long deep = 0;
// helping variable
static bool ret;
// type of variable in case of its declaration
char left_new_var_type;
// name of varaible in case of its declaration
string_t left_new_var;
// array of local tables of symbols for variables
arr_symtbs_t local_symtbs;
// global table of symbols for functions
htable_t global_symtab;
// tmp_var for name of functions where we are
htab_item_t *item;
// tmp_var for currently working variable
htab_item_t *tmp_var;
// tmp_var for currently working function
htab_item_t *tmp_func;
// string of types on the left side
string_t tps_left;
// string of types on the right side
string_t tps_right;
// what structure we can use
// 0 - decl_fun, 1 - def_func
bool working_func;

Queue* queue_id;
Queue* queue_expr;


#define FILL_TYPE(IDX)                            \
        do {                                      \
            switch (token.keyword) {              \
                case KW_INTEGER:                  \
                    ret = str_add_char(IDX, 'I'); \
                    break;                        \
                case KW_STRING:                   \
                    ret = str_add_char(IDX, 'S'); \
                    break;                        \
                case KW_NUMBER:                   \
                    ret = str_add_char(IDX, 'F'); \
                    break;                        \
                case KW_NIL:                      \
                    ret = str_add_char(IDX, 'N'); \
                    break;                        \
                default:                          \
                    ret = str_add_char(IDX, 'U'); \
                    break;                        \
            }                                     \
            CHECK_INTERNAL_ERR(!ret, false);      \
        } while(0)

#define ADD_FUNC_TO_SYMTAB(SUSPECT_REDECLARATION, LABEL)                   \
        do {                                                               \
            if (!symtab_add(&global_symtab, &token.attr.id)) {             \
                if (err == INTERNAL_ERR) {                                 \
                    return false;                                          \
                }                                                          \
                else if (err == SEM_DEF_ERR) {                             \
                    item = symtab_find(&global_symtab, token.attr.id.str); \
                    if (!item && err == INTERNAL_ERR) {                    \
                        return false;                                      \
                    }                                                      \
                    else if (SUSPECT_REDECLARATION) {                      \
                        return false;                                      \
                    }                                                      \
                    err = NO_ERR;                                          \
                    goto LABEL;                                            \
                }                                                          \
            }                                                              \
            item = symtab_find(&global_symtab, token.attr.id.str);         \
            if (!item) {                                                   \
                return false;                                              \
            }                                                              \
            item->data.func = calloc(1, sizeof(func_t));                   \
            CHECK_INTERNAL_ERR(!item->data.func, false);                   \
            item->data.func->decl = false;                                 \
            item->data.func->def = false;                                  \
            item->type = FUNC;                                             \
        } while(0)

#define CHECK_TPS_DEF_DECL_FUNCS()                                                                     \
        do {                                                                                           \
            if (item->data.func->decl && item->data.func->def) {                                       \
                if (strcmp(item->data.func->def_attr.argv.str, item->data.func->decl_attr.argv.str) || \
                    strcmp(item->data.func->def_attr.rets.str, item->data.func->decl_attr.rets.str))   \
                {                                                                                      \
                    err = SEM_DEF_ERR;                                                                 \
                    return false;                                                                      \
                }                                                                                      \
            }                                                                                          \
        } while(0)

#define ALLOC_VAR_IN_SYMTAB(KEY)                                                    \
        do {                                                                        \
            tmp_var = symtab_add(&local_symtbs.htab[local_symtbs.size - 1], (KEY)); \
            if (!tmp_var) {                                                         \
                return false;                                                       \
            }                                                                       \
            tmp_var->data.var = calloc(1, sizeof(var_t));                           \
            CHECK_INTERNAL_ERR(!tmp_var->data.var, false);                          \
            tmp_var->deep = deep;                                                   \
            tmp_var->type = VAR;                                                    \
            str_init(&tmp_var->data.var->type, 2);                                  \
        } while(0)

#define CHECK_COMPATIBILITY(ERROR)       \
        do {                             \
            if (!type_compatibility()) { \
                err = (ERROR);           \
                return false;            \
            }                            \
            else {                       \
                str_clear(&tps_left);    \
                str_clear(&tps_right);   \
            }                            \
        } while(0)

#define STR_COPY_STR(DST, COND, SRC_1, SRC_2)          \
        ret = str_copy_str(DST, COND ? SRC_1 : SRC_2); \
        CHECK_INTERNAL_ERR(!ret, false)

#define CHECK_COUNT_OF_ARGS()                                        \
        do {                                                         \
            if (str_get_len(&tps_left) != str_get_len(&tps_right) && \
                !tmp_func->data.func->func_write)                    \
            {                                                        \
                err = SEM_FUNC_ERR;                                  \
                return false;                                        \
            }                                                        \
        } while(0)


bool type_compatibility() {
    if (tmp_func != NULL && tmp_func->data.func->func_write) {
        for (int i = 0; i < str_get_len(&tps_right); i++) {
            if (tps_right.str[i] == 'U') {
                err = SEM_FUNC_ERR;
                return false;
            }
        }

        return true;
    }

    if (str_get_len(&tps_left) > str_get_len(&tps_right)) {
        err = SEM_FUNC_ERR;
        return false;
    }
    else {
        for (int i = 0; i < str_get_len(&tps_left); i++) {
            if ((tps_left.str[i] == tps_right.str[i]) ||
                (tps_left.str[i] == 'F' && tps_right.str[i] == 'I') ||
                (tps_right.str[i] == 'N'))
            {
                continue;
            }
            else {
                err = SEM_FUNC_ERR;
                return false;
            }
        }
    }

    return true;
}

bool prolog() {
    if (token.keyword == KW_REQUIRE) {
        print_rule("1.  <prolog> -> require t_string <prog>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_STRING);
        if (str_cmp_const_str(&token.attr.id, "ifj21")) {
            err = SEM_OTHER_ERR;
            return false;
        }

        NEXT_TOKEN();

        CODE_GEN(gen_init);
        return prog();
    }

    return false;
}

bool prog() {
    if (token.keyword == KW_GLOBAL) {
        print_rule("2.  <prog> -> global id : function ( <arg_T> ) <ret_T> <prog>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        // Allocate structure for decl_function in symtable //
        ADD_FUNC_TO_SYMTAB(item->data.func->decl == true, add_func_decl);
add_func_decl:
        item->deep = deep;
        item->data.func->decl = true;
        working_func = 0; // declaration of function
        ret = str_init(&item->data.func->decl_attr.rets, 5);
        CHECK_INTERNAL_ERR(!ret, false);
        ret = str_init(&item->data.func->decl_attr.argv, 5);
        CHECK_INTERNAL_ERR(!ret, false);
        //////////////////////////////////////////////////////

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();
        EXPECTED_TOKEN(token.keyword == KW_FUNCTION);
        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
        NEXT_TOKEN();

        // fill decl_func.argv
        NEXT_NONTERM(arg_T());

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();

        // fill decl_func.rets
        NEXT_NONTERM(ret_T());

        // check if decl a def of function have the same types of args and rets
        CHECK_TPS_DEF_DECL_FUNCS();

        return prog();
    }
    else if (token.keyword == KW_FUNCTION) {
        print_rule("3.  <prog> -> function id ( <arg> ) <ret_T> <stmt>"
                " end <prog>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        // GEN_CODE //
        strcpy(cnt.func_name.str, token.attr.id.str);
        CODE_GEN(gen_func_start, token.attr.id.str);
        //////////////

        // Allocate structure for def_function in symtable //
        ADD_FUNC_TO_SYMTAB(item->data.func->def == true, add_func_def);
add_func_def:
        item->deep = deep;
        item->data.func->def = true;
        working_func = 1;
        ret = str_init(&item->data.func->def_attr.rets, 5);
        CHECK_INTERNAL_ERR(!ret, false);
        ret = str_init(&item->data.func->def_attr.argv, 5);
        CHECK_INTERNAL_ERR(!ret, false);
        /////////////////////////////////////////////////////

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        // create local table of symbols
        ADD_SYMTAB();
        deep++;

        NEXT_TOKEN();
        NEXT_NONTERM(arg());

        // GEN_CODE //
        CODE_GEN(gen_params);
        //////////////

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(ret_T());

        // check if decl a def of function have the same tpes of args and rets
        CHECK_TPS_DEF_DECL_FUNCS();

        NEXT_NONTERM(stmt());

        // GEN_CODE //
        while (cnt.ret_vals > 0 && !cnt.in_return) {
            CODE_GEN(gen_retval_nil);
            cnt.ret_vals--;
        }
        CODE_GEN(init_cnt);
        //////////////

        EXPECTED_TOKEN(token.keyword == KW_END);

        // GEN_CODE //
		CODE_GEN(gen_func_end);
        //////////////

		deep--;
        // remove last local table from array
        DEL_SYMTAB();
        NEXT_TOKEN();

        return prog();
    }
    else if (token.type == T_ID) {
        print_rule("4.  <prog> -> id ( <param> ) <prog>");

        tmp_func = FIND_FUNC_IN_SYMTAB;

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        // check if function was declared/defined
        CHECK_SEM_DEF_ERR(!tmp_func);

        // tmp_func = calling func
        // tps_left = tmp_func.argv
        // expected in tps_right types of arguments
        STR_COPY_STR(&tps_left,                           tmp_func->data.func->def == true,
                     &tmp_func->data.func->def_attr.argv, &tmp_func->data.func->decl_attr.argv);

		// GEN_CODE //
		CODE_GEN(gen_func_call_start); 	// createframe
		QUEUE_ADD_ID(tmp_func); 		// only for func name, args are printed individually
		//////////////

        NEXT_TOKEN();
        NEXT_NONTERM(param());

		// GEN_CODE //
		cnt.param_cnt = 0;
		CODE_GEN(gen_func_call_label); // call label
		//////////////

        // after args() in tps_left are expected types of arguments
        // in tps_right we have real types of arguments
        // do comparing of to arrays and then clear string
        CHECK_COUNT_OF_ARGS();
        CHECK_COMPATIBILITY(SEM_FUNC_ERR);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();

        return prog();
    }
    else if (token.type == T_EOF) {
        // end of program
        print_rule("5.  <prog> -> EOF");

        return true;
    }

    return false;
}

bool arg_T() {
    // add type of param to decl_function in symtab
    if (working_func == 0) {
        FILL_TYPE(&item->data.func->decl_attr.argv);
    }

    if (type()) {
        print_rule("6. <arg_T> -> <type> <next_arg_T>");

        return next_arg_T();
    }

    str_clear(&item->data.func->decl_attr.argv);
    print_rule("7. <arg_T> -> e");
    return true;
}

bool next_arg_T() {
    if (token.type == T_COMMA) {
        print_rule("8. <next_arg_T> -> , <type> <next_arg_T>");

        NEXT_TOKEN();

        // add type of param to decl_function in symtab
        if (working_func == 0) {
            FILL_TYPE(&item->data.func->decl_attr.argv);
        }
        NEXT_NONTERM(type());

        return next_arg_T();
    }

    print_rule("9. <next_arg_T> -> e");
    return true;
}

bool ret_T() {
    if (token.type == T_COLON) {
        print_rule("10. <ret_T> -> : <type> <next_ret_T>");

        NEXT_TOKEN();
        // add type of param to function in symtab
        FILL_TYPE(working_func == 1 ?
                  &item->data.func->def_attr.rets :
                  &item->data.func->decl_attr.rets);

        NEXT_NONTERM(type());

        cnt.ret_vals++;

        return next_ret_T();
    }

    print_rule("11. <ret_T> -> e");
    return true;
}

bool next_ret_T() {
    if (token.type == T_COMMA) {
        print_rule("12. <next_ret_T> -> , <type> <next_ret_T>");

        NEXT_TOKEN();
        // add type of param to function in symtab
        FILL_TYPE(working_func == 1 ?
                  &item->data.func->def_attr.rets :
                  &item->data.func->decl_attr.rets);

        NEXT_NONTERM(type());

        cnt.ret_vals++;
        return next_ret_T();
    }

    print_rule("13. <next_ret_T> -> e");
    return true;
}

bool arg() {
    if (token.type == T_ID) {
        print_rule("14. <arg> -> id : <type> <next_arg>");

        // check if we have declare function with the same name
        CHECK_SEM_DEF_ERR(symtab_find(&global_symtab, token.attr.id.str));

        ALLOC_VAR_IN_SYMTAB(&token.attr.id);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();

        FILL_TYPE(&item->data.func->def_attr.argv);
        FILL_TYPE(&tmp_var->data.var->type);

        NEXT_NONTERM(type());

        ///////////////////////
        QUEUE_ADD_ID(tmp_var);
        ///////////////////////

        return next_arg();
    }

    print_rule("15. <arg> -> e");
    return true;
}

bool next_arg() {
    if (token.type == T_COMMA) {
        print_rule("16. <next_arg> -> , id : <type> <next_arg>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        // check if we have declare function with the same name
        CHECK_SEM_DEF_ERR(symtab_find(&global_symtab, token.attr.id.str));

        ALLOC_VAR_IN_SYMTAB(&token.attr.id);
        QUEUE_ADD_ID(tmp_var);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();

        FILL_TYPE(&item->data.func->def_attr.argv);
        FILL_TYPE(&tmp_var->data.var->type);
        NEXT_NONTERM(type());

        return next_arg();
    }

    print_rule("17. <next_arg> -> e");
    return true;
}

bool type() {
    if (token.type != T_KEYWORD) {
        return false;
    }

    if (token.keyword == KW_INTEGER || token.keyword == KW_NUMBER ||
        token.keyword == KW_STRING  || token.keyword == KW_NIL)
    {
        print_rule("18. 19. 20. 21.  <type> -> int|num|str|nil");
    }
    else {
        return false;
    }

    NEXT_TOKEN();
    return true;
}

bool stmt() {
    cnt.in_return = 0;              // reset - because if multiple_returns in function cancle func earlier.
    if (token.keyword == KW_IF) {
        print_rule("22. <stmt> -> if <expr> then <stmt> else <stmt>"
                " end <stmt>");

        NEXT_TOKEN();
        // after expression() in tps_right is final type of expr
        NEXT_NONTERM(expr(true, false));

        // GEN_CODE //
        CODE_GEN(gen_expression);
        CODE_GEN(gen_if_eval);
        CODE_GEN(gen_if_start);
        //////////////

        // we dont need final type of expr in conditions
        // therefore clear this string
        str_clear(&tps_right);
        EXPECTED_TOKEN(token.keyword == KW_THEN);

        // create new local table of symbols
        ADD_SYMTAB();
        deep++;

        NEXT_TOKEN();
        NEXT_NONTERM(stmt());

        CODE_GEN(gen_if_end_jump);

        EXPECTED_TOKEN(token.keyword == KW_ELSE);

        // delete last local table of symbols
        deep--;
        DEL_SYMTAB();

        // create local table 0f symbols for new scope
        ADD_SYMTAB();

        NEXT_TOKEN();

        CODE_GEN(gen_if_else);
        deep++;

        NEXT_NONTERM(stmt());

        CODE_GEN(gen_if_end);

        EXPECTED_TOKEN(token.keyword == KW_END);

        // delete last local table of symbols
        deep--;
        DEL_SYMTAB();

        NEXT_TOKEN();

        return stmt();
    }
    else if (token.keyword == KW_WHILE) {
        print_rule("23. <stmt> -> while <expr> do <stmt>"
                " end <stmt>");

        NEXT_TOKEN();

		// GEN_CODE //
        strcpy(cnt.func_name.str, item->key_id);
        cnt.while_cnt_deep++;
        cnt.in_while = true;
        CODE_GEN(gen_while_label);
		//////////////

        // after expression() in tps_right is final type of expr
        NEXT_NONTERM(expr(true, false));

		// GEN_CODE //
        CODE_GEN(gen_expression);
        CODE_GEN(gen_while_eval);
		//////////////

        // we dont need final type of expr in conditions
        // therefore clear this string
        str_clear(&tps_right);
        EXPECTED_TOKEN(token.keyword == KW_DO);

        // add new local table of symbols fot while scope
        ADD_SYMTAB();
        deep++;

        NEXT_TOKEN();
        NEXT_NONTERM(stmt());
        EXPECTED_TOKEN(token.keyword == KW_END);

        // GEN_CODE //
        CODE_GEN(gen_while_end);
        cnt.while_cnt_deep--;
        cnt.in_while = (cnt.while_cnt_deep == 0) ? false : true;
        if (cnt.while_cnt_deep == 0) {
            CODE_GEN(gen_concat_while_functions);
        }
        //////////////

        // delete last table of symbols for variables
        deep--;
        DEL_SYMTAB();

        NEXT_TOKEN();

        return stmt();
    }
    else if (token.keyword == KW_LOCAL) {
        print_rule("24. <stmt> -> local id : <type> <def_var> <stmt>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        // check if we have declare function with the same name
        CHECK_SEM_DEF_ERR(symtab_find(&global_symtab, token.attr.id.str));

        // Allocate structure for variable in symtable //
        // remember name of declaration variable
        ret = str_copy_str(&left_new_var, &token.attr.id);
        CHECK_INTERNAL_ERR(!ret, false);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();

        // set to tps_left type of variable
        FILL_TYPE(&tps_left);

        NEXT_NONTERM(type());
        NEXT_NONTERM(def_var());

        return stmt();
    }
    else if (token.keyword == KW_RETURN) {
        cnt.in_return = true;
        cnt.ret_vals = 0;

        print_rule("25. <stmt> -> return <expr> <next_expr> <stmt>");

        // copy rets types of function to tps_left
        STR_COPY_STR(&tps_left,                       item->data.func->def == true,
                     &item->data.func->def_attr.rets, &item->data.func->decl_attr.rets);

        NEXT_TOKEN();
        // set to the tps_right final type of expr
        NEXT_NONTERM(expr(false, true));

        // GEN_CODE //
        CODE_GEN(gen_expression);
        //////////////

        NEXT_NONTERM(next_expr());

        // if number of expected arguments is less then real number
        if (str_get_len(&tps_right) > str_get_len(&tps_left)) {
            err = SEM_FUNC_ERR;
            return false;
        }

        // set N to other types in return if it is needed
        for (int i = str_get_len(&tps_right); i < str_get_len(&tps_left); i++) {
            str_add_char(&tps_right, 'N');
        }

        // check compatibility of types in <return>
        CHECK_COMPATIBILITY(SEM_FUNC_ERR);

        return stmt();
    }
    else if (token.type == T_ID) {
        print_rule("26. <stmt> -> id <fork_id> <stmt>");

        // remember name of variable
        ret = str_copy_str(&left_new_var, &token.attr.id);
        CHECK_INTERNAL_ERR(!ret, false);

        NEXT_TOKEN();
        NEXT_NONTERM(fork_id());

        return stmt();
    }

    print_rule("27. <stmt> -> e");
    return true;
}

bool def_var() {
    if (token.type == T_ASSIGN) {
        print_rule("28. <def_var> -> = <one_assign>");

        NEXT_TOKEN();

        return one_assign();
    }

    ALLOC_VAR_IN_SYMTAB(&left_new_var);
    ret = str_copy_str(&tmp_var->data.var->type, &tps_left);
    str_clear(&left_new_var);

	CHECK_INTERNAL_ERR(!ret, false);

	//////////////////////////////
	QUEUE_ADD_ID(tmp_var);
	CODE_GEN(gen_def_var);
	//////////////////////////////

    str_clear(&tps_left);

    print_rule("29. <def_var> -> e");
    queue_remove_rear(queue_id);
    return true;
}


bool one_assign() {
    const int  num_var = 1;
    int  num_return = 0;

    tmp_func = symtab_find(&global_symtab, token.attr.id.str);
    if (token.type == T_ID && tmp_func) {
        print_rule("30. <one_assign> -> id ( <param> )");
        left_new_var_type = tps_left.str[0];

        STR_COPY_STR(&tps_right,                          tmp_func->data.func->def == true,
                     &tmp_func->data.func->def_attr.rets, &tmp_func->data.func->decl_attr.rets);
        CHECK_COMPATIBILITY(SEM_TYPE_COMPAT_ERR);
        num_return = str_get_len(tmp_func->data.func->def == true ? &tmp_func->data.func->def_attr.rets : &tmp_func->data.func->decl_attr.rets);
        STR_COPY_STR(&tps_left,                           tmp_func->data.func->def == true,
                     &tmp_func->data.func->def_attr.argv, &tmp_func->data.func->decl_attr.argv);

        // GEN_CODE //
        CODE_GEN(gen_func_call_start);
        strcpy(cnt.func_call.str, token.attr.id.str);
        QUEUE_ADD_ID(tmp_func);
        //////////////

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
        NEXT_TOKEN();

        NEXT_NONTERM(param());

        CHECK_COUNT_OF_ARGS();
        CHECK_COMPATIBILITY(SEM_FUNC_ERR);

        // create new variable in a local table of symbols
        ALLOC_VAR_IN_SYMTAB(&left_new_var);
        tmp_var->data.var->type.str[0] = left_new_var_type;
        str_clear(&left_new_var);
        left_new_var_type = '\0';

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();

		// GEN_CODE //
		cnt.param_cnt = 0;
        CODE_GEN(gen_func_call_label);
        for(int i = num_return - num_var; i > 0; i--){
            PRINT_FUNC(1, "pops GF@&var1" NON_VAR , EMPTY_STR);
        }
        QUEUE_ADD_ID(tmp_var);
        CODE_GEN(gen_def_var);
        while(!queue_isEmpty(queue_id)){
            CODE_GEN(gen_init_var);
        }
		//////////////

        return true;
    }

    print_rule("31. <one_assign> -> <expr>");
    NEXT_NONTERM(expr(false, false));

    CODE_GEN(gen_expression);

    // add new declare variable to local table of symbols
    ALLOC_VAR_IN_SYMTAB(&left_new_var);

    // GEN_CODE //
    QUEUE_ADD_ID(tmp_var);
    cnt.in_while = false;
    CODE_GEN(gen_def_var);
    cnt.in_while = (cnt.while_cnt_deep == 0) ? false : true;
    CODE_GEN(gen_init_var);
    //////////////

    // fill type of new declare variable
    ret = str_copy_str(&tmp_var->data.var->type, &tps_left);
    CHECK_INTERNAL_ERR(!ret, false);

    CHECK_COMPATIBILITY(SEM_TYPE_COMPAT_ERR);
    str_clear(&left_new_var);

    return true;
}

bool param() {
    if (param_val()) {
        print_rule("32. <param> -> <param_val> <next_param>");
        return next_param();
    }

    print_rule("33. <param> -> e");
    return true;
}

bool param_val() {
    if (token.type == T_ID) {
        print_rule("34. <param_val> -> id");

        tmp_var = FIND_VAR_IN_SYMTAB;
        CHECK_SEM_DEF_ERR(!tmp_var);

        ///////////////////////////
        if (strcmp(cnt.func_call.str, "write") == 0) {
            CODE_GEN(gen_func_call_start);
            CODE_GEN(gen_func_call_args_var, tmp_var);
            CODE_GEN(gen_func_call_label);
        } else {
            CODE_GEN(gen_func_call_args_var, tmp_var);
        }
        ///////////////////////////

        // add to tps_right types of tokens
        ret = str_add_char(&tps_right, tmp_var->data.var->type.str[0]);
        CHECK_INTERNAL_ERR(!ret, false);
    }
    else if (TOKEN_TERM()) {
        print_rule("35. <param_val> -> <term>");
        NEXT_NONTERM(term());
    }
    else {
        return false;
    }

    NEXT_TOKEN();
    return true;
}

bool term() {
    print_rule("36. 37. 38. 39. <term> -> str|int|num|nil");
    // add to tps_right types of tokens
    ret = str_add_char(&tps_right, token.type == T_INT    ? 'I' :
                                   token.type == T_STRING ? 'S' :
                                   token.type == T_FLOAT  ? 'F' : 'N');
	///////////////////////////////
    CHECK_INTERNAL_ERR(!ret, false);

    if (strcmp(cnt.func_call.str, "write") == 0) {
        CODE_GEN(gen_func_call_start);
        CODE_GEN(gen_func_call_args_const, &token);
         CODE_GEN(gen_func_call_label);
    } else {
        CODE_GEN(gen_func_call_args_const, &token);
    }
    ///////////////////////////////

    return true;
}

bool next_param() {
    if (token.type == T_COMMA) {
        print_rule("40. <next_param> -> , <param_val> <next_param>");

        NEXT_TOKEN();
        NEXT_NONTERM(param_val());
        return next_param();
    }

    print_rule("41. <next_param> -> e");
    return true;
}

bool next_expr() {
    unsigned int len = (unsigned int)str_get_len(&item->data.func->def_attr.rets);
    if (token.type == T_COMMA) {
        print_rule("42. <next_exp> -> , <expr> <next_expr>");

        NEXT_TOKEN();

        NEXT_NONTERM(expr(false, false));

		///////////////////
        cnt.ret_vals++;
        CODE_GEN(gen_expression);
		///////////////////

        return next_expr();
    } else if (len > cnt.ret_vals + 1 && len != 0) { //        return  || return 1, 2 (3)
        cnt.ret_vals++;
        gen_retval_nil();
        next_expr();
    } else {
        if (cnt.in_return)
            CODE_GEN(gen_func_end);
    }

    print_rule("43. <next_expr> -> e");
    return true;
}

bool fork_id() {
    if (token.type == T_L_ROUND_BR) {
        // if it is function calling and name of function
        // exists in global table of symbols
        tmp_func = symtab_find(&global_symtab, left_new_var.str);
        if (tmp_func != NULL) {
            print_rule("44. <fork_id> -> ( <param> )");

            // GEN_CODE //
            CODE_GEN(gen_func_call_start);
            strcpy(cnt.func_call.str, tmp_func->key_id);
            QUEUE_ADD_ID(tmp_func);
            //////////////

            // clear tm_left_new_var
            str_clear(&left_new_var);

            // copy to tps_left types of arguments of function
            STR_COPY_STR(&tps_left,                           tmp_func->data.func->def == true,
                         &tmp_func->data.func->def_attr.argv, &tmp_func->data.func->decl_attr.argv);

            NEXT_TOKEN();

            NEXT_NONTERM(param());

            CHECK_COUNT_OF_ARGS();
            CHECK_COMPATIBILITY(SEM_FUNC_ERR);

            EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

            // GEN_CODE //
            if (strcmp(cnt.func_call.str, "write") != 0)
                CODE_GEN(gen_func_call_label);
            cnt.param_cnt = 0;
            //////////////

            NEXT_TOKEN();
            str_clear(&cnt.func_call);

            return true;
        }

        err = SEM_DEF_ERR;
        return false;
    }

    print_rule("45. <fork_id> -> <next_id>");

    tmp_var = find_id_symtbs(&local_symtbs, left_new_var.str);
    CHECK_SEM_DEF_ERR(!tmp_var);
    str_clear(&left_new_var);

    ret = str_add_char(&tps_left, tmp_var->data.var->type.str[0]);
    CHECK_INTERNAL_ERR(!ret, false);

	/////////////////////////
    QUEUE_ADD_ID(tmp_var);
    /////////////////////////

    NEXT_NONTERM(next_id());

    return true;
}

bool next_id() {
    if (token.type == T_COMMA) {
        print_rule("46. <next_id> -> , id <next_id>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        tmp_var = FIND_VAR_IN_SYMTAB;
        CHECK_SEM_DEF_ERR(!tmp_var);

        ret = str_add_char(&tps_left, tmp_var->data.var->type.str[0]);
        CHECK_INTERNAL_ERR(!ret, false);

        ///////////////////////
        QUEUE_ADD_ID(tmp_var);
        ///////////////////////

        NEXT_TOKEN();

        return next_id();
    }
    else if (token.type == T_ASSIGN) {
        print_rule("47. <next_id> -> = <mult_assign>");

        NEXT_TOKEN();

        return mult_assign();
    }

    return false;
}

bool mult_assign() {
    tmp_func = FIND_FUNC_IN_SYMTAB;
    int  num_var = 0;
    int  num_return = 0;

    if (token.type == T_ID && tmp_func) {
        print_rule("48. <mult_assign> -> id ( <param> )");

        STR_COPY_STR(&tps_right,                          tmp_func->data.func->def == true,
                     &tmp_func->data.func->def_attr.rets, &tmp_func->data.func->decl_attr.rets);
        num_var = str_get_len(&tps_left);

        CHECK_COMPATIBILITY(SEM_TYPE_COMPAT_ERR);

        num_return = str_get_len(tmp_func->data.func->def == true ? &tmp_func->data.func->def_attr.rets : &tmp_func->data.func->decl_attr.rets);
        STR_COPY_STR(&tps_left,                           tmp_func->data.func->def == true,
                     &tmp_func->data.func->def_attr.argv, &tmp_func->data.func->decl_attr.argv);

        // CODE_GEN //
        CODE_GEN(gen_func_call_start);
        strcpy(cnt.func_call.str, token.attr.id.str);
        QUEUE_ADD_ID(tmp_func);
        //////////////

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
        NEXT_TOKEN();

        NEXT_NONTERM(param());


        CHECK_COUNT_OF_ARGS();
        CHECK_COMPATIBILITY(SEM_FUNC_ERR);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

        // CODE_GEN //
        cnt.param_cnt = 0;
        CODE_GEN(gen_func_call_label);
        //////////////

        NEXT_TOKEN();

        // CODE_GEN //
        for(int i = num_return - num_var; i > 0; i--){
            PRINT_FUNC(1, "pops GF@&var1" NON_VAR , EMPTY_STR);
        }
        while(!queue_isEmpty(queue_id))
            CODE_GEN(gen_init_var);
        //////////////

        return true;
    }

    print_rule("49. <mult_assign> -> <expr> <next_expr>");

    NEXT_NONTERM(expr(false, false));

    // CODE_GEN //
    CODE_GEN(gen_expression);
    //////////////

    NEXT_NONTERM(next_expr());

    // CODE_GEN //
    while (!queue_isEmpty(queue_id)) {
        CODE_GEN(gen_init_var);
    }
    //////////////

    if (str_get_len(&tps_left) > str_get_len(&tps_right)) {
        err = SEM_OTHER_ERR;
        return false;
    }

    CHECK_COMPATIBILITY(SEM_TYPE_COMPAT_ERR);
    return true;
}

bool init_default_funcs_ifj21() {
    string_t def_funcs[COUNT_DEF_FUNCS] = { {.length = 3, .alloc_size = 0, .str = "chr"      },
                                            {.length = 9, .alloc_size = 0, .str = "tointeger"},
                                            {.length = 5, .alloc_size = 0, .str = "reads"    },
                                            {.length = 5, .alloc_size = 0, .str = "readi"    },
                                            {.length = 5, .alloc_size = 0, .str = "readn"    },
                                            {.length = 5, .alloc_size = 0, .str = "write"    },
                                            {.length = 6, .alloc_size = 0, .str = "substr"   },
                                            {.length = 3, .alloc_size = 0, .str = "ord"      }, };

    char *argv[COUNT_DEF_FUNCS] = {"I", "F",  "",  "",  "", "...FSIN", "SFF", "SI"};
    char *rets[COUNT_DEF_FUNCS] = {"S", "I", "S", "I", "F",        "",   "S",  "I"};

    for (int i = 0; i < COUNT_DEF_FUNCS; i++) {
        item = symtab_add(&global_symtab, &(def_funcs[i]));

        if (!item) {
            return false;
        }

        item->data.func = calloc(1, sizeof(func_t));
        CHECK_INTERNAL_ERR(!item->data.func, false);

        item->type = FUNC;
        item->data.func->decl = true;
        item->data.func->def = true;

        if (!str_init(&item->data.func->def_attr.rets,  strlen(rets[i]) + 1) ||
            !str_init(&item->data.func->decl_attr.rets, strlen(rets[i]) + 1) ||
            !str_init(&item->data.func->def_attr.argv,  strlen(argv[i]) + 1) ||
            !str_init(&item->data.func->decl_attr.argv, strlen(argv[i]) + 1))
        {
            return false;
        }

        for (int j = 0; argv[i][j] != '\0'; j++) {
            if (!str_add_char(&item->data.func->def_attr.argv,  argv[i][j]) ||
                !str_add_char(&item->data.func->decl_attr.argv, argv[i][j]))
            {
                return false;
            }
        }

        for (int j = 0; rets[i][j] != '\0'; j++) {
            if (!str_add_char(&item->data.func->def_attr.rets, rets[i][j]) ||
                !str_add_char(&item->data.func->decl_attr.rets, rets[i][j]))
            {
                return false;
            }
        }

        item->data.func->func_write = i == 5 ? true : false;
    }

    return true;
}

bool check_def_of_decl_func() {
    for (int i = 0; i < MAX_HT_SIZE; i++) {
        item = global_symtab[i];

        while (item) {
            CHECK_SEM_DEF_ERR(item->data.func->decl == 1 && item->data.func->def == 0);
            item = item->next;
        }
    }

    return true;
}

int parser() {
    FILE *f = stdin;
    err = NO_ERR;
    set_source_file(f);

    queue_expr = queue_init();
    queue_id   = queue_init();

#ifndef DEBUG_ANDREJ
// profesionalni debug, master of C language by Andrej Binovsky
#endif

    ret = str_init(&token.attr.id, 20);
    CHECK_INTERNAL_ERR(!ret, INTERNAL_ERR);
    local_symtbs.size = 0;


    ret = init_default_funcs_ifj21();
    CHECK_INTERNAL_ERR(!ret, INTERNAL_ERR);

    ret = str_init(&tps_left, 5);
    CHECK_INTERNAL_ERR(!ret, INTERNAL_ERR);
    ret = str_init(&tps_right, 5);
    CHECK_INTERNAL_ERR(!ret, INTERNAL_ERR);
    ret = str_init(&left_new_var, 20);
    CHECK_INTERNAL_ERR(!ret, INTERNAL_ERR);



    FIRST_TOKEN();
    ret = prolog();

    if (!ret) {
        err = err == NO_ERR ? PARSER_ERR : err;
        goto end_parser;
    }

    ret = check_def_of_decl_func();

end_parser:
    str_free(&tps_right);
    str_free(&tps_left);
    str_free(&left_new_var);
    str_free(&token.attr.id);
    free_symtbs(&local_symtbs);
    symtab_free(&global_symtab);
    queue_free(queue_expr);
    queue_free(queue_id);


    return err;
}
