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

#define FILL_RETS(IDX) \
        switch (token.keyword) { \
            case KW_INTEGER: \
                IDX = INTEGER; \
                break; \
            case KW_STRING: \
                IDX = STRING; \
                break; \
            case KW_NUMBER: \
                IDX = NUMBER; \
                break; \
            case KW_NIL: \
                IDX = NIL; \
                break; \
            default: \
                break; \
        }

#define ADD_FUNC_TO_SYMTAB(SUSPECT_REDECLARATION, LABEL) \
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
        item->type = FUNC;

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

        //////////////////////////////////////////////////////
        ADD_FUNC_TO_SYMTAB(item->data.func->decl == true, add_func_decl);
add_func_decl:
        item->data.func->decl = true;
        item->data.func->decl_attr.num_argv = 0;
        item->data.func->decl_attr.num_rets = 0;
        item->data.func->decl_attr.argv = NULL;
        item->data.func->decl_attr.rets = NULL;
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
        // item = NULL;
        return prog();
    }
    else if (token.keyword == KW_FUNCTION) {
        print_rule("3.  <prog> -> function id ( <params> ) <type_returns> <statement>"
                " <return_type> end <prog>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        //////////////////////////////////////////////////////
        ADD_FUNC_TO_SYMTAB(item->data.func->def == true, add_func_def);
add_func_def:
        item->data.func->def = true;
        item->data.func->def_attr.num_argv = 0;
        item->data.func->def_attr.num_rets = 0;
        item->data.func->def_attr.argv = NULL;
        item->data.func->def_attr.rets = NULL;
        //////////////////////////////////////////////////////

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        ADD_SYMTAB();

        NEXT_TOKEN();
        NEXT_NONTERM(params);
        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(type_returns);
        // item = NULL;
        NEXT_NONTERM(statement);
        EXPECTED_TOKEN(token.keyword == KW_END);

        DEL_SYMTAB();

        NEXT_TOKEN();

        return prog();
    }
    else if (token.type == T_ID) {
        print_rule("4.  <prog> -> id_func ( <args> ) <prog>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(args);
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

        ADD_VAR_TO_SYMTAB();

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();
        NEXT_NONTERM(type);
        NEXT_NONTERM(def_var);

        return statement();
    }
    else if (token.keyword == KW_RETURN) {
        print_rule("15. <statement> -> return <expression> <other_exp> <statement>");

        NEXT_TOKEN();
        NEXT_NONTERM(expression);
        NEXT_NONTERM(other_exp);

        return statement();
    }
    else if (token.type == T_ID) {
        if (FIND_FUNC_IN_SYMTAB) {
            print_rule("13. <statement> -> id_func ( <args> ) <statement>");

            NEXT_TOKEN();
            EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
            NEXT_TOKEN();
            NEXT_NONTERM(args);
            EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
            NEXT_TOKEN();
        }
        else if (FIND_VAR_IN_SYMTAB) {
            print_rule("14. <statement> -> id_var <vars> <statement>");

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

        CHECK_ID(VAR);

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
    if (token.type == T_ID && FIND_FUNC_IN_SYMTAB) {
        print_rule("19. <type_expr> -> id_func ( <args> )");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(args);
        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();

        return true;
    }

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

    print_rule("22. <other_exp> -> e");
    return true;
}

bool def_var() {
    if (token.type == T_ASSIGN) {
        print_rule("23. <def_var> -> = <init_assign>");

        NEXT_TOKEN();

        return init_assign();
    }

    print_rule("24. <def_var> -> e");
    return true;
}

bool init_assign() {
    if (token.type == T_ID && FIND_FUNC_IN_SYMTAB) {
        print_rule("25. <init_assign> -> id_func ( <args> )");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);
        NEXT_TOKEN();
        NEXT_NONTERM(args);
        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();

        return true;
    }

    print_rule("26. <init_assign> -> <expression>");
    return expression();
}

bool type_returns() {
    if (token.type == T_COLON) {
        print_rule("27. <type_returns> -> : <type> <other_types>");

        NEXT_TOKEN();
        NEXT_NONTERM(type);

        ///////////////////////////////////////////
        if (item->data.func->def == true && item->data.func->def_attr.rets == NULL) {
            item->data.func->def_attr.rets = calloc(1, sizeof(type_t)); // RETURN
            FILL_RETS(item->data.func->def_attr.rets[item->data.func->def_attr.num_rets]);
            (item->data.func->def_attr.num_rets)++;
        }
        else if (item->data.func->decl == true && item->data.func->decl_attr.rets == NULL) {
            item->data.func->decl_attr.rets = calloc(1, sizeof(type_t)); // RETURN
            FILL_RETS(item->data.func->decl_attr.rets[item->data.func->decl_attr.num_rets]);
            (item->data.func->decl_attr.num_rets)++;
        }
        ///////////////////////////////////////////

        return other_types();
    }

    print_rule("28. <type_returns> -> e");
    return true;
}

bool other_types() {
    if (token.type == T_COMMA) {
        print_rule("29. <other_types> -> , <type> <other_types>");

        NEXT_TOKEN();
        NEXT_NONTERM(type);

        ///////////////////////////////////////////
        type_t *tmp = NULL;
        if (item->data.func->def == true && item->data.func->def_attr.num_rets == 1) {
            tmp = realloc(item->data.func->def_attr.rets, sizeof(type_t) * (item->data.func->def_attr.num_rets + 1)); // RETURN
            item->data.func->def_attr.rets = tmp;
            FILL_RETS(item->data.func->def_attr.rets[item->data.func->def_attr.num_rets]);
            (item->data.func->def_attr.num_rets)++;
        }
        else if (item->data.func->decl == true && item->data.func->decl_attr.num_rets == 1) {
            tmp = realloc(item->data.func->decl_attr.rets, sizeof(type_t) * (item->data.func->decl_attr.num_rets + 1)); // RETURN
            item->data.func->decl_attr.rets = tmp;
            FILL_RETS(item->data.func->decl_attr.rets[item->data.func->decl_attr.num_rets]);
            (item->data.func->decl_attr.num_rets)++;
        }
        ///////////////////////////////////////////

        return other_types();
    }

    print_rule("30. <other_types> -> e");
    return true;
}

bool params() {
    if (token.type == T_ID) {
        print_rule("32. <params> -> id : <type> <other_params>");

        ADD_VAR_TO_SYMTAB();

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();
        NEXT_NONTERM(type);

        return other_params();
    }

    print_rule("31. <params> -> e");
    return true;
}

bool other_params() {
    if (token.type == T_COMMA) {
        print_rule("33. <other_params> -> , id : <type> <other_params>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        ADD_VAR_TO_SYMTAB();

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);
        NEXT_TOKEN();
        NEXT_NONTERM(type);

        return other_params();
    }

    print_rule("34. <other_params> -> e");
    return true;
}

bool type_params() {
    if (type()) {
        print_rule("35. <type_params> -> <type> <other_types>");

        return other_types();
    }

    print_rule("36. <type_params> -> e");
    return true;
}

bool args() {
    if (param_to_func()) {
        print_rule("37. <args> -> <param_to_func> <other_args>");

        return other_args();
    }

    print_rule("38. <args> -> e");
    return true;
}

bool param_to_func() {
    if (token.type == T_ID) {
        print_rule("39. <param_to_func> -> id_var");

        CHECK_ID(VAR);
    }
    else if (TOKEN_TERM()) {
        print_rule("40. <param_to_func> -> term");
    }
    else {
        return false;
    }

    NEXT_TOKEN();
    return true;
}

bool other_args() {
    if (token.type == T_COMMA) {
        print_rule("41. <other_args> -> , <param_to_func> <other_args>");

        NEXT_TOKEN();
        NEXT_NONTERM(param_to_func);
        return other_args();
    }

    print_rule("42. <other_args> -> e");
    return true;
}

void init_default_funcs_ifj21() {
    string_t def_funcs[COUNT_DEF_FUNCS] = { {.length = 3, .alloc_size = 0, .str = "chr"      },
                                            {.length = 9, .alloc_size = 0, .str = "tointeger"},
                                            {.length = 5, .alloc_size = 0, .str = "reads"    },
                                            {.length = 5, .alloc_size = 0, .str = "readi"    },
                                            {.length = 5, .alloc_size = 0, .str = "readn"    },
                                            {.length = 5, .alloc_size = 0, .str = "write"    },
                                            {.length = 6, .alloc_size = 0, .str = "substr"   },
                                            {.length = 3, .alloc_size = 0, .str = "ord"      }, };

    for (int i = 0; i < COUNT_DEF_FUNCS; i++) {
        symtab_add(&global_symtab, &(def_funcs[i])); // RETURN
    }
}

int parser() {
    FILE *f = stdin;
    err = NO_ERR;
    set_source_file(f);

    ret = str_init(&token.attr.id, 20);
    if (!ret) {
        return INTERNAL_ERR;
    }

    local_symtbs.size = 0;
    init_default_funcs_ifj21();

    FIRST_TOKEN();
    ret = prolog();

    if (!ret && err == NO_ERR) {
        err = PARSER_ERR;
    }

    str_free(&token.attr.id);
    free_symtbs(&local_symtbs);
    symtab_free(&global_symtab);

    return err;
}
