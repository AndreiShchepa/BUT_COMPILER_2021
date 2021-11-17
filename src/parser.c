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

token_t token;
int err;
static bool ret;
arr_symtbs_t local_symtbs;
htable_t global_symtab;
htab_item_t *item;
htab_item_t *tmp_var;
htab_item_t *tmp_func;
string_t tps_left;
string_t tps_right;
bool working_func; // 0 - decl_fun, 1 - def_func

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

#define ALLOC_VAR_IN_SYMTAB() \
        do { \
            tmp_var = symtab_add(&local_symtbs.htab[local_symtbs.size - 1], &token.attr.id); \
            if (!tmp_var) { \
                return false; \
            } \
            tmp_var->data.var = calloc(1, sizeof(var_t)); \
            CHECK_INTERNAL_ERR(!tmp_var->data.var, false); \
            tmp_var->type = VAR; \
            tmp_var->data.var->init = true; \
            tmp_var->data.var->val_nil = true; \
            str_init(&tmp_var->data.var->type, 2); \
        } while(0);

#define CHECK_COMPATIBILITY() \
        do { \
            if (!type_compatibility()) { \
                err = SEM_FUNC_ERR; \
                return false; \
            } \
            else { \
                str_clear(&tps_left); \
                str_clear(&tps_right); \
            } \
        } while(0);

bool type_compatibility() {

    printf("SEg\n");
    if(tmp_func != NULL && tmp_func->data.func->func_write) {
        for (long unsigned int i = 0; i < tps_right.length; i++) {
            if (tps_right.str[i] == 'U') {
                err = SEM_FUNC_ERR;
                return false;
            }
        }

        return true;
    }
    printf("SEg\n");

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

        return prog();
    }

    return false;
}

bool prog() {
    if (token.keyword == KW_GLOBAL) {
        print_rule("2.  <prog> -> global id : function ( <type_params> ) <type_returns> <prog>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        // Allocate structure for decl_function in symtable //
        ADD_FUNC_TO_SYMTAB(item->data.func->decl == true, add_func_decl);
add_func_decl:
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
        NEXT_NONTERM(type_params);
        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(type_returns);

        CHECK_TPS_DEF_DECL_FUNCS();

        return prog();
    }
    else if (token.keyword == KW_FUNCTION) {
        print_rule("3.  <prog> -> function id ( <params> ) <type_returns> <statement>"
                " <return_type> end <prog>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        // Allocate structure for def_function in symtable //
        ADD_FUNC_TO_SYMTAB(item->data.func->def == true, add_func_def);
add_func_def:
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

        NEXT_TOKEN();
        NEXT_NONTERM(params);
        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(type_returns);

        CHECK_TPS_DEF_DECL_FUNCS();

        NEXT_NONTERM(statement);
        EXPECTED_TOKEN(token.keyword == KW_END);

        DEL_SYMTAB();

        NEXT_TOKEN();

        return prog();
    }
    else if (token.type == T_ID) {
        print_rule("4.  <prog> -> id_func ( <args> ) <prog>");

        tmp_func = FIND_FUNC_IN_SYMTAB;
        CHECK_SEM_DEF_ERR(!tmp_func);

        // tmp_func = calling func
        // tps_left = tmp_func.argv
        // expected in tps_right types of arguments
        ////////////////////////////////
        ret = str_copy_str(&tps_left, tmp_func->data.func->def == true ? &tmp_func->data.func->def_attr.argv  :
                                                                         &tmp_func->data.func->decl_attr.argv);
        CHECK_INTERNAL_ERR(!ret, false);
        ////////////////////////////////

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(args);

        // after argc() we have in tps_left expected types of argv
        // in tps_right we have real types of argv
        // do comparing of to arrays anc then clear
        CHECK_COMPATIBILITY();

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
        if (token.keyword == KW_INTEGER) {
            print_rule("6.  <type> -> integer");
        }
        else if (token.keyword == KW_NUMBER) {
            print_rule("7.  <type> -> number");
        }
        else if (token.keyword == KW_STRING) {
            print_rule("8.  <type> -> string");
        }
        else if (token.keyword == KW_NIL) {
            print_rule("9. <type> -> nil");
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
        NEXT_NONTERM(expression);
        EXPECTED_TOKEN(token.keyword == KW_THEN);

        ADD_SYMTAB();

        NEXT_TOKEN();
        NEXT_NONTERM(statement);
        EXPECTED_TOKEN(token.keyword == KW_ELSE);

        DEL_SYMTAB();

        ADD_SYMTAB();

        NEXT_TOKEN();
        NEXT_NONTERM(statement);
        EXPECTED_TOKEN(token.keyword == KW_END);

        DEL_SYMTAB();

        NEXT_TOKEN();

        return statement();
    }
    else if (token.keyword == KW_WHILE) {
        print_rule("11. <statement> -> while <expression> do <statement>"
                " end <statement>");

        NEXT_TOKEN();
        NEXT_NONTERM(expression);
        EXPECTED_TOKEN(token.keyword == KW_DO);

        ADD_SYMTAB();

        NEXT_TOKEN();
        NEXT_NONTERM(statement);
        EXPECTED_TOKEN(token.keyword == KW_END);

        DEL_SYMTAB();

        NEXT_TOKEN();

        return statement();
    }
    else if (token.keyword == KW_LOCAL) {
        print_rule("12. <statement> -> local id_var : <type> <def_var> <statement>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        // Allocate structure for variable in symtable //
        ALLOC_VAR_IN_SYMTAB();
        /////////////////////////////////////////////////

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();

        // now in tps_left je expected type of new id
        FILL_TYPE(&tmp_var->data.var->type);
        FILL_TYPE(&tps_left);

        NEXT_NONTERM(type);
        NEXT_NONTERM(def_var);
        //ret = str_copy_str(&tps_right, tmp_func->data.func->def == true ? &tmp_func->data.func->def_attr.rets  :
        //                                                                &tmp_func->data.func->decl_attr.rets);
        //CHECK_INTERNAL_ERR(!ret, false);

        // we should check type compatibility
        //CHECK_COMPATIBILITY();

        return statement();
    }
    else if (token.keyword == KW_RETURN) {
        print_rule("15. <statement> -> return <expression> <other_exp> <statement>");

        //////////////////////////////////////////////////////////////////
        ret = str_copy_str(&tps_left, item->data.func->def == true ? &item->data.func->def_attr.rets  :
                                                                     &item->data.func->decl_attr.rets);

        CHECK_INTERNAL_ERR(!ret, false);

        uint64_t tmp_num_rets = item->data.func->def == true ? item->data.func->def_attr.rets.length :
                                                               item->data.func->decl_attr.rets.length;
        for (unsigned long int i = 0; i < tmp_num_rets; i++) {
            ret = str_add_char(&tps_right, 'N');
            CHECK_INTERNAL_ERR(!ret, false);
        }
        //////////////////////////////////////////////////////////////////

        NEXT_TOKEN();
        NEXT_NONTERM(expression);
        NEXT_NONTERM(other_exp);

        //CHECK_COMPATIBILITY();

        return statement();
    }
    else if (token.type == T_ID) {
        if (FIND_FUNC_IN_SYMTAB) {
            print_rule("13. <statement> -> id_func ( <args> ) <statement>");

            tmp_func = FIND_FUNC_IN_SYMTAB;
            CHECK_SEM_DEF_ERR(!tmp_func);

            /////////////////////////////////////////////////
            ret = str_copy_str(&tps_left, tmp_func->data.func->def == true ? &tmp_func->data.func->def_attr.argv  :
                                                                             &tmp_func->data.func->decl_attr.argv);
            CHECK_INTERNAL_ERR(!ret, false);
            /////////////////////////////////////////////////

            NEXT_TOKEN();
            EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
            NEXT_TOKEN();

            NEXT_NONTERM(args);

            CHECK_COMPATIBILITY();

            EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
            NEXT_TOKEN();
        }
        else if (FIND_VAR_IN_SYMTAB) {
            print_rule("14. <statement> -> id_var <vars> <statement>");


            tmp_var = FIND_VAR_IN_SYMTAB;
            CHECK_SEM_DEF_ERR(!tmp_var);

            ret = str_add_char(&tps_left, tmp_var->data.var->type.str[0]);
            CHECK_INTERNAL_ERR(!ret, false);

            NEXT_TOKEN();
            NEXT_NONTERM(vars);
        }
        else {
            err = SEM_DEF_ERR;
            return false;
        }

        return statement();
    }

    print_rule("16. <statement> -> e");
    return true;
}

bool vars() {
    if (token.type == T_COMMA) {
        print_rule("17. <vars> -> , id_var <vars>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        tmp_var = FIND_VAR_IN_SYMTAB;
        CHECK_SEM_DEF_ERR(!tmp_var);

        ret = str_add_char(&tps_left, tmp_var->data.var->type.str[0]);
        CHECK_INTERNAL_ERR(!ret, false);

        NEXT_TOKEN();

        return vars();
    }
    else if (token.type == T_ASSIGN) {
        print_rule("18. <vars> -> = <type_expr>");

        NEXT_TOKEN();

        return type_expr();
    }

    return false;
}

bool type_expr() {
    tmp_func = FIND_FUNC_IN_SYMTAB;

    if (token.type == T_ID && tmp_func) {
        print_rule("19. <type_expr> -> id_func ( <args> )");

        ////////////////////////////////////////////////////////////
        ret = str_copy_str(&tps_right, tmp_func->data.func->def == true ? &tmp_func->data.func->def_attr.rets  :
                                                                         &tmp_func->data.func->decl_attr.rets);
        //printf("left  = %s\n", tps_left.str);
        //printf("right = %s\n", tps_right.str);
        CHECK_INTERNAL_ERR(!ret, false);
        CHECK_COMPATIBILITY();
        /////////////////////////////////////////////////////
        ret = str_copy_str(&tps_left, tmp_func->data.func->def == true ? &tmp_func->data.func->def_attr.argv  :
                                                                         &tmp_func->data.func->decl_attr.argv);
        CHECK_INTERNAL_ERR(!ret, false);
        /////////////////////////////////////////////////////

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(args);

        CHECK_COMPATIBILITY();

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();

        return true;
    }

    str_clear(&tps_left);
    print_rule("20. <type_expr> -> <expression> <other_exp>");

    NEXT_NONTERM(expression);
    return other_exp();
}

bool other_exp() {
    if (token.type == T_COMMA) {
        print_rule("21. <other_exp> -> , <expression> <other_exp>");

        NEXT_TOKEN();
        NEXT_NONTERM(expression);

        return other_exp();
    }

    // check compatibility for expressions
    // expressions are expected in tps_right
    // vars are expected in tps_left
    CHECK_COMPATIBILITY();
    print_rule("22. <other_exp> -> e");
    return true;
}

bool def_var() {
    if (token.type == T_ASSIGN) {
        print_rule("23. <def_var> -> = <init_assign>");

        NEXT_TOKEN();

        // variable has initial value //
        tmp_var->data.var->val_nil = false;
        ////////////////////////////////

        return init_assign();
    }

    str_clear(&tps_left);

    print_rule("24. <def_var> -> e");
    return true;
}

bool init_assign() {
    tmp_func = symtab_find(&global_symtab, token.attr.id.str);

    if (token.type == T_ID && tmp_func) {
        print_rule("25. <init_assign> -> id_func ( <args> )");
        // in tmp_func is calling function
        // write to tps_right expected type return of calling func
        // and do comparing of types with left_tps
        /////////////////////////////////////////////////////////////////////////
        ret = str_copy_str(&tps_right, tmp_func->data.func->def == true ? &tmp_func->data.func->def_attr.rets  :
                                                                         &tmp_func->data.func->decl_attr.rets);
        CHECK_INTERNAL_ERR(!ret, false);
        CHECK_COMPATIBILITY();

        // then we check expected argv for function
        ////////////////////////////////////////////////////////////////////////
        ret = str_copy_str(&tps_left, tmp_func->data.func->def == true ? &tmp_func->data.func->def_attr.argv  :
                                                                         &tmp_func->data.func->decl_attr.argv);
        CHECK_INTERNAL_ERR(!ret, false);
        /////////////////////////////////////////////////////////////////////////

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(args);

        CHECK_COMPATIBILITY();
        // nice, we check whole rule for type compatibility

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();

        return true;
    }

    print_rule("26. <init_assign> -> <expression>");
    printf("ALALALAL\n");
    NEXT_NONTERM(expression);
    printf("ALALALAL\n");
    // comment next two lines, when expression will be ready
    //str_clear(&tps_left);
    //str_clear(&tps_right);
    //
    printf("%s - tps_left\n", tps_left.str);
    printf("%s - tps_right\n", tps_right.str);
    //if (tps_right == NULL || &tps_left = NULL) {printf("NULLLLLLLL\n");}
    CHECK_COMPATIBILITY();
    printf("ALALALAL 20 2020 202\n");

    return true;
}

bool type_returns() {
    if (token.type == T_COLON) {
        print_rule("27. <type_returns> -> : <type> <other_types_returns>");

        NEXT_TOKEN();
        // add type of param to function in symtab //
        FILL_TYPE(working_func == 1 ?
                  &item->data.func->def_attr.rets :
                  &item->data.func->decl_attr.rets);
        /////////////////////////////////////////////
        NEXT_NONTERM(type);

        return other_types_returns();
    }

    print_rule("28. <type_returns> -> e");
    return true;
}

bool other_types_returns() {
    if (token.type == T_COMMA) {
        print_rule("29. <other_types_returns> -> , <type> <other_types_returns>");

        NEXT_TOKEN();
        // add type of param to function in symtab //
        FILL_TYPE(working_func == 1 ?
                  &item->data.func->def_attr.rets :
                  &item->data.func->decl_attr.rets);
        /////////////////////////////////////////////
        NEXT_NONTERM(type);

        return other_types_returns();
    }

    print_rule("30. <other_types_returns> -> e");
    return true;
}

bool other_types_params() {
    if (token.type == T_COMMA) {
        print_rule("31. <other_types_params> -> , <type> <other_types_params>");

        NEXT_TOKEN();
        // add type of param to decl_function in symtab //
        if (working_func == 0) {
            FILL_TYPE(&item->data.func->decl_attr.argv);
        }
        //////////////////////////////////////////////////

        NEXT_NONTERM(type);

        return other_types_params();
    }

    print_rule("32. <other_types_params> -> e");
    return true;
}

bool params() {
    if (token.type == T_ID) {
        print_rule("34. <params> -> id : <type> <other_params>");

        ALLOC_VAR_IN_SYMTAB();

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();

        // add type of param to def_function in symtab //
        if (working_func == 1) {
            FILL_TYPE(&item->data.func->def_attr.argv);
        }

        /////////////////////////////////////////////////
        FILL_TYPE(&tmp_var->data.var->type);
        /////////////////////////////////////////////////

        NEXT_NONTERM(type);

        return other_params();
    }

    print_rule("33. <params> -> e");
    return true;
}

bool other_params() {
    if (token.type == T_COMMA) {
        print_rule("35. <other_params> -> , id : <type> <other_params>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        //ADD_VAR_TO_SYMTAB();
        ALLOC_VAR_IN_SYMTAB();

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();

        // add type of param to def_function in symtab //
        if (working_func == 1) {
            FILL_TYPE(&item->data.func->def_attr.argv);
        }
        /////////////////////////////////////////////////

        NEXT_NONTERM(type);

        return other_params();
    }

    print_rule("36. <other_params> -> e");
    return true;
}

bool type_params() {
    // add type of param to decl_function in symtab //
    if (working_func == 0) {
        FILL_TYPE(&item->data.func->decl_attr.argv);
    }
    //////////////////////////////////////////////////

    if (type()) {
        print_rule("37. <type_params> -> <type> <other_types_params>");

        return other_types_params();
    }

    print_rule("38. <type_params> -> e");
    return true;
}

bool args() {
    if (param_to_func()) {
        print_rule("39. <args> -> <param_to_func> <other_args>");

        return other_args();
    }

    print_rule("40. <args> -> e");
    return true;
}

bool param_to_func() {
    if (token.type == T_ID) {
        print_rule("41. <param_to_func> -> id_var");

        tmp_var = FIND_VAR_IN_SYMTAB;

        CHECK_SEM_DEF_ERR(!tmp_var);

        // add to tps_right types of tokens
        ret = str_add_char(&tps_right, tmp_var->data.var->type.str[0]);
        CHECK_INTERNAL_ERR(!ret, false);
    }
    else if (TOKEN_TERM()) {
        print_rule("42. <param_to_func> -> term");
        // add to tps_right types of tokens
        ret = str_add_char(&tps_right, token.type == T_INT    ? 'I' :
                                       token.type == T_STRING ? 'S' :
                                       token.type == T_FLOAT  ? 'F' : 'N');
        CHECK_INTERNAL_ERR(!ret, false);
    }
    else {
        return false;
    }

    NEXT_TOKEN();
    return true;
}

bool other_args() {
    if (token.type == T_COMMA) {
        print_rule("43. <other_args> -> , <param_to_func> <other_args>");

        NEXT_TOKEN();
        NEXT_NONTERM(param_to_func);
        return other_args();
    }

    print_rule("44. <other_args> -> e");
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

    ret = str_init(&token.attr.id, 20);
    CHECK_INTERNAL_ERR(!ret, INTERNAL_ERR);

    local_symtbs.size = 0;

    ret = init_default_funcs_ifj21();
    CHECK_INTERNAL_ERR(!ret, INTERNAL_ERR);

    ret = str_init(&tps_left, 5);
    CHECK_INTERNAL_ERR(!ret, INTERNAL_ERR);
    ret = str_init(&tps_right, 5);
    CHECK_INTERNAL_ERR(!ret, INTERNAL_ERR);

    FIRST_TOKEN();
    ret = prolog();

    if (!ret && err == NO_ERR) {
        err = PARSER_ERR;
    }

    ret = check_def_of_decl_func();

end_parser:
    str_free(&tps_right);
    str_free(&tps_left);
    str_free(&token.attr.id);
    free_symtbs(&local_symtbs);
    symtab_free(&global_symtab);

    return err;
}
