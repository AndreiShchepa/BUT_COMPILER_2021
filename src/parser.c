/**
 * Project: Compiler IFJ21
 *
 * @file parser.c
 *
 * @brief Implement functions processing rules for syntax analysis
 *
 * @author Andrei Shchapaniak <xshcha00>
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

token_t token;
int err;
unsigned long deep = 0;
static bool ret;
char left_new_var_type;
string_t left_new_var;
arr_symtbs_t local_symtbs;
htable_t global_symtab;
htab_item_t *item;
htab_item_t *tmp_var;
htab_item_t *tmp_func;
string_t tps_left;
string_t tps_right;
bool working_func; // 0 - decl_fun, 1 - def_func
Queue* queue_id;
Queue* queue_args;
Queue* queue_expr;



#define CHECK_INTERNAL_ERR(COND, ret) \
        do { \
            if (COND) { \
                err = INTERNAL_ERR; \
                return ret; \
            } \
        } while(0);

#define CHECK_SEM_DEF_ERR(COND) \
        do { \
            if (COND) { \
                err = SEM_DEF_ERR; \
                return false; \
            } \
        } while(0);

#define FILL_TYPE(IDX) \
        do { \
            switch (token.keyword) { \
                case KW_INTEGER: \
                    ret = str_add_char(IDX, 'I'); \
                    break; \
                case KW_STRING: \
                    ret = str_add_char(IDX, 'S'); \
                    break; \
                case KW_NUMBER: \
                    ret = str_add_char(IDX, 'F'); \
                    break; \
                case KW_NIL: \
                    ret = str_add_char(IDX, 'N'); \
                    break; \
                default: \
                    ret = str_add_char(IDX, 'U'); \
                    break; \
            } \
            CHECK_INTERNAL_ERR(!ret, false); \
        } while(0);

#define ADD_FUNC_TO_SYMTAB(SUSPECT_REDECLARATION, LABEL) \
        do { \
            if (!symtab_add(&global_symtab, &token.attr.id)) { \
                if (err == INTERNAL_ERR) { \
                    return false; \
                } \
                else if (err == SEM_DEF_ERR) { \
                    item = symtab_find(&global_symtab, token.attr.id.str); \
                    if (!item && err == INTERNAL_ERR) { \
                        return false; \
                    } \
                    else if (SUSPECT_REDECLARATION) { \
                        return false; \
                    } \
                    err = NO_ERR; \
                    goto LABEL; \
                } \
            } \
            item = symtab_find(&global_symtab, token.attr.id.str); \
            if (!item) { \
                return false; \
            } \
            item->data.func = calloc(1, sizeof(func_t)); \
            CHECK_INTERNAL_ERR(!item->data.func, false); \
            item->data.func->decl = false; \
            item->data.func->def = false; \
            item->type = FUNC; \
        } while(0);

#define CHECK_TPS_DEF_DECL_FUNCS() \
        do { \
            if (item->data.func->decl && item->data.func->def) { \
                if (strcmp(item->data.func->def_attr.argv.str, item->data.func->decl_attr.argv.str) || \
                    strcmp(item->data.func->def_attr.rets.str, item->data.func->decl_attr.rets.str)) \
                { \
                    err = SEM_DEF_ERR; \
                    return false; \
                } \
            } \
        } while(0);

#define ALLOC_VAR_IN_SYMTAB(KEY) \
        do { \
            tmp_var = symtab_add(&local_symtbs.htab[local_symtbs.size - 1], (KEY)); \
            if (!tmp_var) { \
                return false; \
            } \
            tmp_var->data.var = calloc(1, sizeof(var_t)); \
            CHECK_INTERNAL_ERR(!tmp_var->data.var, false); \
            tmp_var->deep = deep; \
            tmp_var->type = VAR; \
            str_init(&tmp_var->data.var->type, 2); \
        } while(0);

#define CHECK_COMPATIBILITY(ERROR) \
        do { \
            if (!type_compatibility()) { \
                err = (ERROR); \
                return false; \
            } \
            else { \
                str_clear(&tps_left); \
                str_clear(&tps_right); \
            } \
        } while(0);

#define STR_COPY_STR(DST, COND, SRC_1, SRC_2) \
        ret = str_copy_str(DST, COND ? SRC_1 : SRC_2); \
        CHECK_INTERNAL_ERR(!ret, false);

bool type_compatibility() {

    if (tmp_func != NULL && tmp_func->data.func->func_write) {
        for (long unsigned int i = 0; i < tps_right.length; i++) {
            if (tps_right.str[i] == 'U') {
                err = SEM_FUNC_ERR;
                return false;
            }
        }

        return true;
    }

    if (tps_left.length > tps_right.length) {
        err = SEM_FUNC_ERR;
        return false;
    }
    else {
        for (long unsigned int i = 0; i < tps_left.length; i++) {
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
        print_rule("1.  <prolog> -> require term_str <prog>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(!str_cmp_const_str(&token.attr.id, "ifj21") && token.type == T_STRING);
        NEXT_TOKEN();

        CODE_GEN(gen_init);
        return prog();
    }

    return false;
}

bool prog() {
    if (token.keyword == KW_GLOBAL) {
        print_rule("2.  <prog> -> global id : function ( <type_params> ) <type_returns> <prog>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        if (FIND_VAR_IN_SYMTAB) {
            err = SEM_DEF_ERR;
            return false;
        }

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
        NEXT_NONTERM(type_params());
        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(type_returns());

        CHECK_TPS_DEF_DECL_FUNCS();
        return prog();
    }
    else if (token.keyword == KW_FUNCTION) {
        print_rule("3.  <prog> -> function id ( <params> ) <type_returns> <statement>"
                " <return_type> end <prog>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        //////////////////
        strcpy(cnt.func_name.str, token.attr.id.str);
        CODE_GEN(gen_func_start, token.attr.id.str);
        //////////////////

        if (FIND_VAR_IN_SYMTAB) {
            err = SEM_DEF_ERR;
            return false;
        }

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

        ADD_SYMTAB();

        deep++;
        NEXT_TOKEN();
        NEXT_NONTERM(params());

        ////////////////////////
        CODE_GEN(gen_params);
        ////////////////////////

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(type_returns());

        CHECK_TPS_DEF_DECL_FUNCS();

        NEXT_NONTERM(statement());


        /////////////////////////
        CODE_GEN(init_cnt);
        /////////////////////////

        EXPECTED_TOKEN(token.keyword == KW_END);

        /////////////////////////
		CODE_GEN(gen_func_end);
        /////////////////////////

		deep--;

        DEL_SYMTAB();

        NEXT_TOKEN();

        return prog();
    }
    else if (token.type == T_ID) {
        print_rule("4.  <prog> -> id ( <args> ) <prog>");

        tmp_func = FIND_FUNC_IN_SYMTAB;

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        CHECK_SEM_DEF_ERR(!tmp_func);
        // tmp_func = calling func
        // tps_left = tmp_func.argv
        // expected in tps_right types of arguments
        STR_COPY_STR(&tps_left,                           tmp_func->data.func->def == true,
                     &tmp_func->data.func->def_attr.argv, &tmp_func->data.func->decl_attr.argv);

		////////////////////////////////
		CODE_GEN(gen_func_call_start); 	// createframe
		QUEUE_ADD_ID(tmp_func); 		// only for func name, args are printed individually
		////////////////////////////////

        NEXT_TOKEN();
        NEXT_NONTERM(args());

		////////////////////////////////
		CODE_GEN(gen_func_call_label); // call label
		////////////////////////////////

        // after argc() we have in tps_left expected types of argv
        // in tps_right we have real types of argv
        // do comparing of to arrays anc then clear
        //
        CHECK_COMPATIBILITY(SEM_FUNC_ERR);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();

        return prog();
    }
    else if (token.type == T_EOF) {
        print_rule("5.  <prog> -> EOF");

        return true;
    }

    return false;
}

bool type() {
    if (token.type == T_KEYWORD) {
        if (token.keyword == KW_INTEGER || token.keyword == KW_NUMBER ||
            token.keyword == KW_STRING  || token.keyword == KW_NIL)
        {
            print_rule("6. 7. 8. 9.  <type> -> int|num|str|nil");
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }

    NEXT_TOKEN();
    return true;
}

bool statement() {
    if (token.keyword == KW_IF) {
        print_rule("10. <statement> -> if <expression> then <statement> else <statement>"
                " end <statement>");

        NEXT_TOKEN();
        NEXT_NONTERM(expression(true, false));
        CODE_GEN(gen_expression); // todo Andrej
        CODE_GEN(gen_if_eval); // todo Andrej
        CODE_GEN(gen_if_start); // todo Andrej

        str_clear(&tps_right);
        EXPECTED_TOKEN(token.keyword == KW_THEN);

        ADD_SYMTAB();

        NEXT_TOKEN();
        deep++;
        NEXT_NONTERM(statement());
        CODE_GEN(gen_if_end_jump); // todo Andrej
        EXPECTED_TOKEN(token.keyword == KW_ELSE);
        deep--;

        DEL_SYMTAB();

        ADD_SYMTAB();

        NEXT_TOKEN();
        CODE_GEN(gen_if_else); // todo Andrej
        deep++;
        NEXT_NONTERM(statement());
        CODE_GEN(gen_if_end); // todo Andrej
        EXPECTED_TOKEN(token.keyword == KW_END);
        deep--;

        DEL_SYMTAB();

        NEXT_TOKEN();

        return statement();
    }
    else if (token.keyword == KW_WHILE) {
        print_rule("11. <statement> -> while <expression> do <statement>"
                " end <statement>");

        NEXT_TOKEN();

		///////////////////////////////////
        strcpy(cnt.func_name.str, item->key_id);
        CODE_GEN(gen_while_label);
		///////////////////////////////////

        NEXT_NONTERM(expression(true, false));

		///////////////////////////////////
        CODE_GEN(gen_expression);
        CODE_GEN(gen_while_eval);
		///////////////////////////////////

        str_clear(&tps_right);
        EXPECTED_TOKEN(token.keyword == KW_DO);

        ADD_SYMTAB();

        NEXT_TOKEN();
        deep++;
        NEXT_NONTERM(statement());
        EXPECTED_TOKEN(token.keyword == KW_END);
        CODE_GEN(gen_while_end);
        deep--;

        DEL_SYMTAB();

        NEXT_TOKEN();

        return statement();
    }
    else if (token.keyword == KW_LOCAL) {
        print_rule("12. <statement> -> local id : <type> <def_var> <statement>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        // Allocate structure for variable in symtable //
        ret = str_copy_str(&left_new_var, &token.attr.id);
        CHECK_INTERNAL_ERR(!ret, false);

        if (FIND_FUNC_IN_SYMTAB) {
            err = SEM_DEF_ERR;
            return false;
        }

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();

        //FILL_TYPE(&tmp_var->data.var->type);
        FILL_TYPE(&tps_left);

        NEXT_NONTERM(type());
        NEXT_NONTERM(def_var());

        //ALLOC_VAR_IN_SYMTAB(&left_new_var);
        //ret = str_copy_str(&tmp_var->data.var->type, &tps_left);
        //CHECK_INTERNAL_ERR(!ret, false);
        //str_clear(&tps_left);

        return statement();
    }
    else if (token.keyword == KW_RETURN) {
        cnt.ret_vals = 0;

        print_rule("13. <statement> -> return <expression> <other_exp> <statement>");

        STR_COPY_STR(&tps_left,                       item->data.func->def == true,
                     &item->data.func->def_attr.rets, &item->data.func->decl_attr.rets);

        NEXT_TOKEN();
        NEXT_NONTERM(expression(false, true));

        /////////////////////////////////////////
        CODE_GEN(gen_expression);
        /////////////////////////////////////////

        NEXT_NONTERM(other_exp());

        ////////////////////// comment ////////////////////////
        if (tps_right.length > tps_left.length) {
            err = SEM_FUNC_ERR;
            return false;
        }

        for (uint64_t i = 0; i < tps_left.length; i++) {
            str_add_char(&tps_right, 'N');
        }
        ///////////////////////////////////////////////////////
        CHECK_COMPATIBILITY(SEM_FUNC_ERR);

        return statement();
    }
    else if (token.type == T_ID) {
        print_rule("14. <statement> -> id <work_with_id> <statement>");

        ret = str_copy_str(&left_new_var, &token.attr.id);
        CHECK_INTERNAL_ERR(!ret, false);
        NEXT_TOKEN();
        NEXT_NONTERM(work_with_id());

        return statement();
    }

    print_rule("15. <statement> -> e");
    return true;
}

bool work_with_id() {
    if (token.type == T_L_ROUND_BR) {
        if (symtab_find(&global_symtab, left_new_var.str)) {
            print_rule("16. <work_with_id> -> ( <args> )");

            tmp_func = symtab_find(&global_symtab, left_new_var.str);

            /////////////////////////
            strcpy(cnt.func_call.str, tmp_func->key_id);
            QUEUE_ADD_ID(tmp_func);
            /////////////////////////

            str_clear(&left_new_var);
            CHECK_SEM_DEF_ERR(!tmp_func);

            STR_COPY_STR(&tps_left,                           tmp_func->data.func->def == true,
                         &tmp_func->data.func->def_attr.argv, &tmp_func->data.func->decl_attr.argv);

//            /////////////////////////
//            strcpy(cnt.func_call.str, tmp_func->key_id);
//            QUEUE_ADD_ID(tmp_func);
//            /////////////////////////

            NEXT_TOKEN();

            NEXT_NONTERM(args());

            CHECK_COMPATIBILITY(SEM_FUNC_ERR);

            EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

            ///////////////////////////
            if (strcmp(cnt.func_call.str, "write") != 0)
                CODE_GEN(gen_func_call_label);
            ///////////////////////////

            NEXT_TOKEN();
            str_clear(&cnt.func_call); // TODO - via CODE_GEN

            return true;
        }

        err = SEM_DEF_ERR;
        return false;
    }

    print_rule("17. <work_with_id> -> <vars>");

    tmp_var = find_id_symtbs(&local_symtbs, left_new_var.str);
    CHECK_SEM_DEF_ERR(!tmp_var);
    str_clear(&left_new_var);

    ret = str_add_char(&tps_left, tmp_var->data.var->type.str[0]);
    CHECK_INTERNAL_ERR(!ret, false);

	/////////////////////////
    QUEUE_ADD_ID(tmp_var); // todo Andrej
    /////////////////////////

    //NEXT_TOKEN();
    NEXT_NONTERM(vars());

    return true;
}

bool vars() {
    if (token.type == T_COMMA) {
        print_rule("18. <vars> -> , id_var <vars>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        tmp_var = FIND_VAR_IN_SYMTAB;
        CHECK_SEM_DEF_ERR(!tmp_var);

        ret = str_add_char(&tps_left, tmp_var->data.var->type.str[0]);
        CHECK_INTERNAL_ERR(!ret, false);

        ///////////////////////
        QUEUE_ADD_ID(tmp_var); // todo Andrej
        ///////////////////////

        NEXT_TOKEN();

        return vars();
    }
    else if (token.type == T_ASSIGN) {
        print_rule("19. <vars> -> = <type_expr>");

        NEXT_TOKEN();

        return type_expr();
    }

    return false;
}

bool type_expr() {
    tmp_func = FIND_FUNC_IN_SYMTAB;
    int  num_var = 0;
    int  num_return = 0;
    if (token.type == T_ID && tmp_func) {
        print_rule("20. <type_expr> -> id_func ( <args> )");

        STR_COPY_STR(&tps_right,                          tmp_func->data.func->def == true,
                     &tmp_func->data.func->def_attr.rets, &tmp_func->data.func->decl_attr.rets);
        num_var = str_get_len(&tps_left);

        CHECK_COMPATIBILITY(SEM_TYPE_COMPAT_ERR);
        num_return = str_get_len(tmp_func->data.func->def == true ? &tmp_func->data.func->def_attr.rets : &tmp_func->data.func->decl_attr.rets);
        STR_COPY_STR(&tps_left,                           tmp_func->data.func->def == true,
                     &tmp_func->data.func->def_attr.argv, &tmp_func->data.func->decl_attr.argv);

        ///////////////////////////
        CODE_GEN(gen_func_call_start);
        strcpy(cnt.func_call.str, token.attr.id.str);
        QUEUE_ADD_ID(tmp_func);
        ///////////////////////////

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(args());

        CHECK_COMPATIBILITY(SEM_FUNC_ERR);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

        /////////////////
        cnt.param_cnt = 0;
        CODE_GEN(gen_func_call_label);
        /////////////////
        NEXT_TOKEN();

        //////////////////////
        for(int i = num_return - num_var; i > 0; i--){
            PRINT_FUNC(1, "pops GF@&var1" NON_VAR , EMPTY_STR);
        }
        while(!queue_isEmpty(queue_id))
            CODE_GEN(gen_init_var);
        //////////////////////

        return true;
    }

    print_rule("21. <type_expr> -> <expression> <other_exp>");

    NEXT_NONTERM(expression(false, false));

    CODE_GEN(gen_expression); //todo Andrej

    NEXT_NONTERM(other_exp());

    if (str_get_len(&tps_left) > str_get_len(&tps_right)) {
        err = SEM_OTHER_ERR;
        return false;
    }

    CHECK_COMPATIBILITY(SEM_TYPE_COMPAT_ERR);
    return true;
}

bool other_exp() {
    unsigned int len = (unsigned int)str_get_len(&item->data.func->def_attr.rets);
    if (token.type == T_COMMA) {
        print_rule("22. <other_exp> -> , <expression> <other_exp>");

        NEXT_TOKEN();

        bool is_return = false;
        if(queue_isEmpty(queue_expr)){ // is return
            is_return = true;
        }

        NEXT_NONTERM(expression(false, false));

        cnt.ret_vals++;
        CODE_GEN(gen_expression); //todo Andrej

        if(!is_return){
            CODE_GEN(gen_init_var);  //todo Andrej
        }
        return other_exp();
    } else if(len > cnt.ret_vals+1 && len != 0) { //        return  || return 1, 2 (3)
        cnt.ret_vals++;
        gen_retval_nil();
        other_exp();
    }

    print_rule("23. <other_exp> -> e");
    return true;
}

bool def_var() {
    if (token.type == T_ASSIGN) {
        print_rule("24. <def_var> -> = <init_assign>");

        NEXT_TOKEN();

        // variable has initial value //
        //tmp_var->data.var->val_nil = false;
        ////////////////////////////////

        return init_assign();
    }

    ALLOC_VAR_IN_SYMTAB(&left_new_var);
    ret = str_copy_str(&tmp_var->data.var->type, &tps_left);
    str_clear(&left_new_var);

	CHECK_INTERNAL_ERR(!ret, false);

	//////////////////////////////
	QUEUE_ADD_ID(tmp_var); //todo Andrej
	CODE_GEN(gen_def_var); // todo Andrej
	//////////////////////////////

    str_clear(&tps_left);

    print_rule("25. <def_var> -> e");
    queue_remove_rear(queue_id); //todo Andrej mozno front
    return true;
}


bool init_assign() {
    const int  num_var = 1;
    int  num_return = 0;

    tmp_func = symtab_find(&global_symtab, token.attr.id.str);
    if (token.type == T_ID && tmp_func) {
        print_rule("26. <init_assign> -> id ( <args> )");
        left_new_var_type = tps_left.str[0];

        STR_COPY_STR(&tps_right,                          tmp_func->data.func->def == true,
                     &tmp_func->data.func->def_attr.rets, &tmp_func->data.func->decl_attr.rets);
        CHECK_COMPATIBILITY(SEM_TYPE_COMPAT_ERR);
        num_return = str_get_len(tmp_func->data.func->def == true ? &tmp_func->data.func->def_attr.rets : &tmp_func->data.func->decl_attr.rets);
        STR_COPY_STR(&tps_left,                           tmp_func->data.func->def == true,
                     &tmp_func->data.func->def_attr.argv, &tmp_func->data.func->decl_attr.argv);

        ///////////////////////////
        CODE_GEN(gen_func_call_start);
        strcpy(cnt.func_call.str, token.attr.id.str);
        QUEUE_ADD_ID(tmp_func);
        ///////////////////////////

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(args());

        ALLOC_VAR_IN_SYMTAB(&left_new_var);
        tmp_var->data.var->type.str[0] = left_new_var_type;
        CHECK_COMPATIBILITY(SEM_FUNC_ERR);
        str_clear(&left_new_var);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();

		/////////////////////////
        for(int i = num_return - num_var; i > 0; i--){
            PRINT_FUNC(1, "pops GF@&var1" NON_VAR , EMPTY_STR);
        }
        QUEUE_ADD_ID(tmp_var);      // todo Andrej
        CODE_GEN(gen_def_var);      // todo Andrej
        while(!queue_isEmpty(queue_id)){ // todo andrej
            CODE_GEN(gen_init_var);
        }
		/////////////////////////

        return true;
    }

    print_rule("27. <init_assign> -> <expression>");
    NEXT_NONTERM(expression(false, false));
    CODE_GEN(gen_expression);     // todo Andrej
    ALLOC_VAR_IN_SYMTAB(&left_new_var);

    ////////////////////////
    QUEUE_ADD_ID(tmp_var);      // todo Andrej
    CODE_GEN(gen_def_var);      // todo Andrej
    CODE_GEN(gen_init_var);     // todo Andrej
    ////////////////////////

    ret = str_copy_str(&tmp_var->data.var->type, &tps_left);
    CHECK_INTERNAL_ERR(!ret, false);
    CHECK_COMPATIBILITY(SEM_TYPE_COMPAT_ERR);
    str_clear(&left_new_var);

    return true;
}

bool type_returns() {
    if (token.type == T_COLON) {
        print_rule("28. <type_returns> -> : <type> <other_types_returns>");

        NEXT_TOKEN();
        // add type of param to function in symtab //
        FILL_TYPE(working_func == 1 ?
                  &item->data.func->def_attr.rets :
                  &item->data.func->decl_attr.rets);
        /////////////////////////////////////////////
        NEXT_NONTERM(type());

        return other_types_returns();
    }

    print_rule("29. <type_returns> -> e");
    return true;
}

bool other_types_returns() {
    if (token.type == T_COMMA) {
        print_rule("30. <other_types_returns> -> , <type> <other_types_returns>");

        NEXT_TOKEN();
        // add type of param to function in symtab //
        FILL_TYPE(working_func == 1 ?
                  &item->data.func->def_attr.rets :
                  &item->data.func->decl_attr.rets);
        /////////////////////////////////////////////
        NEXT_NONTERM(type());

        return other_types_returns();
    }

    print_rule("31. <other_types_returns> -> e");
    return true;
}

bool other_types_params() {
    if (token.type == T_COMMA) {
        print_rule("32. <other_types_params> -> , <type> <other_types_params>");

        NEXT_TOKEN();
        // add type of param to decl_function in symtab //
        if (working_func == 0) {
            FILL_TYPE(&item->data.func->decl_attr.argv);
        }
        //////////////////////////////////////////////////

        NEXT_NONTERM(type());

        return other_types_params();
    }

    print_rule("33. <other_types_params> -> e");
    return true;
}

bool params() {
    if (token.type == T_ID) {
        print_rule("34. <params> -> id : <type> <other_params>");

        if (FIND_FUNC_IN_SYMTAB) {
            err = SEM_DEF_ERR;
            return false;
        }
        ALLOC_VAR_IN_SYMTAB(&token.attr.id);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();

        // add type of param to def_function in symtab //
        if (working_func == 1) {
            FILL_TYPE(&item->data.func->def_attr.argv);
        }

        FILL_TYPE(&tmp_var->data.var->type);

        NEXT_NONTERM(type());

        ///////////////////////
        QUEUE_ADD_ID(tmp_var);
        ///////////////////////

        return other_params();
    }
    //	CODE_GEN(gen_params);

    //if (working_func == 1) {
    //    FILL_TYPE(&item->data.func->def_attr.argv);
    //}

    print_rule("35. <params> -> e");
    return true;
}

bool other_params() {
    if (token.type == T_COMMA) {
        print_rule("36. <other_params> -> , id : <type> <other_params>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        if (FIND_FUNC_IN_SYMTAB) {
            err = SEM_DEF_ERR;
            return false;
        }
        ALLOC_VAR_IN_SYMTAB(&token.attr.id);
        QUEUE_ADD_ID(tmp_var);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();

        // add type of param to def_function in symtab //
        if (working_func == 1) {
            FILL_TYPE(&item->data.func->def_attr.argv);
        }
        /////////////////////////////////////////////////

        FILL_TYPE(&tmp_var->data.var->type);
        NEXT_NONTERM(type());

        return other_params();
    }

    print_rule("37. <other_params> -> e");
    return true;
}

bool type_params() {
    // add type of param to decl_function in symtab //
    if (working_func == 0) {
        FILL_TYPE(&item->data.func->decl_attr.argv);
    }
    //////////////////////////////////////////////////

    if (type()) {
        print_rule("38. <type_params> -> <type> <other_types_params>");

        return other_types_params();
    }

    str_clear(&item->data.func->decl_attr.argv);
    print_rule("39. <type_params> -> e");
    return true;
}

bool args() {
    if (param_to_func()) {
        print_rule("40. <args> -> <param_to_func> <other_args>");
        return other_args();
    }

    print_rule("41. <args> -> e");
    return true;
}

bool param_to_func() {
    if (token.type == T_ID) {
        print_rule("42. <param_to_func> -> id");

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
        //printf("id with name %s has type %c\n", tmp_var->key_id, tmp_var->data.var->type.str[0]);
        ret = str_add_char(&tps_right, tmp_var->data.var->type.str[0]);
    }
    else if (TOKEN_TERM()) {
        print_rule("43. <param_to_func> -> <term>");
        NEXT_NONTERM(term());
    }
    else {
        return false;
    }

    // CHECK_INTERNAL_ERR(!ret, false);

    NEXT_TOKEN();
    return true;
}

bool term() {
    print_rule("44. 45. 46. 47. <term> -> str|int|float|nil");
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

bool other_args() {
    if (token.type == T_COMMA) {
        print_rule("48. <other_args> -> , <param_to_func> <other_args>");

        NEXT_TOKEN();
        NEXT_NONTERM(param_to_func());
        return other_args();
    }

    print_rule("49. <other_args> -> e");
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
    queue_id = queue_init();
    queue_args = queue_init();

#ifndef DEBUG_ANDREJ
// profesionalni debug, master of C language
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
