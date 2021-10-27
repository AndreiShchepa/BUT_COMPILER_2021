#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "str.h"

token_t token;
int err;

#define NEXT_TOKEN() \
        err = get_next_token(&token); \
        if (err != NO_ERR) { \
            return false; \
        }

#define EXPECTED_TOKEN(cond) \
        if (!(cond)) { \
            return false; \
        }

#define NEXT_NONTERM(nonterm, ret) \
        (ret) = (nonterm)(); \
        if (!(ret)) { \
            return false; \
        }

#define FIRST_TOKEN() \
        err = get_next_token(&token); \
        if (err != NO_ERR) { \
            return err; \
        }

bool prog() {
    bool ret;

    if (token.keyword == KW_REQUIRE) {
        printf("1.  <prog> -> <prolog> <prog>\n");

        NEXT_NONTERM(prolog, ret);
        return prog();
    }
    else if (token.keyword == KW_GLOBAL) {
        printf("2.  <prog> -> global id : function ( <type_params> ) <type_returns> <prog>\n");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.keyword == KW_FUNCTION);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        NEXT_TOKEN();
        NEXT_NONTERM(type_params, ret);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

        NEXT_TOKEN();
        NEXT_NONTERM(type_returns, ret);

        return prog();
    }
    else if (token.keyword == KW_FUNCTION) {
        printf("3.  <prog> -> function id ( <params> ) <type_returns> <statement>"
                " <return_type> end <prog>\n");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        NEXT_TOKEN();
        NEXT_NONTERM(params, ret);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

        NEXT_TOKEN();
        NEXT_NONTERM(type_returns, ret);

        NEXT_NONTERM(statement, ret);

        EXPECTED_TOKEN(token.keyword == KW_END);
        NEXT_TOKEN();

        return prog();
    }
    else if (token.type == T_ID) {
        printf("4.  <prog> -> id ( <args> ) <prog>\n");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        NEXT_TOKEN();
        NEXT_NONTERM(args, ret);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);
        NEXT_TOKEN();

        return prog();
    }
    else if (token.type == T_EOF) {
        printf("5.  <prog> -> EOF\n");

        return true;
    }

    return false;
}

bool prolog() {
    printf("6.  <prolog> -> require T_STRING\n");

    EXPECTED_TOKEN(token.keyword == KW_REQUIRE);

    NEXT_TOKEN();
    EXPECTED_TOKEN(!str_cmp_const_str(&token.attr.id, "ifj21") && token.type == T_STRING);

    NEXT_TOKEN();

    return true;
}

bool type() {
    if (token.type == T_KEYWORD) {

        if (token.keyword == KW_INTEGER) {
            printf("7.  <type> -> integer\n");
        }
        else if (token.keyword == KW_NUMBER) {
            printf("8.  <type> -> number\n");
        }
        else if (token.keyword == KW_STRING) {
            printf("9.  <type> -> string\n");
        }
        else if (token.keyword == KW_NIL) {
            printf("10. <type> -> nil\n");
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
    bool ret;

    if (token.keyword == KW_IF) {
        printf("11. <statement> -> if <expression> then <statement> else <statement>"
                " end <statement>\n");

        NEXT_TOKEN();
        NEXT_NONTERM(expression, ret);

        EXPECTED_TOKEN(token.keyword == KW_THEN);

        NEXT_TOKEN();
        NEXT_NONTERM(statement, ret);

        EXPECTED_TOKEN(token.keyword == KW_ELSE);

        NEXT_TOKEN();
        NEXT_NONTERM(statement, ret);

        EXPECTED_TOKEN(token.keyword == KW_END);

        NEXT_TOKEN();
        return statement();
    }
    else if (token.keyword == KW_WHILE) {
        printf("12. <statement> -> while <expression> do <statement>"
                " end <statement>\n");

        NEXT_TOKEN();
        NEXT_NONTERM(expression, ret);

        EXPECTED_TOKEN(token.keyword == KW_DO);

        NEXT_TOKEN();
        NEXT_NONTERM(statement, ret);

        EXPECTED_TOKEN(token.keyword == KW_END);

        NEXT_TOKEN();
        return statement();
    }
    else if (token.keyword == KW_LOCAL) {
        printf("13. <statement> -> local id_var : <type> <def_var> <statement>\n");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);

        NEXT_TOKEN();
        NEXT_NONTERM(type, ret);

        NEXT_NONTERM(def_var, ret);

        return statement();
    }
    else if (token.keyword == KW_RETURN) {
        printf("15. <statement> -> return <expression> <other_exp>\n");
        NEXT_TOKEN();

        NEXT_NONTERM(expression, ret);

        NEXT_NONTERM(other_exp, ret);

        return statement();
    }
    else if (token.type == T_ID) {
        printf("14. <statement> -> id <work_var>\n");

        NEXT_TOKEN();

        NEXT_NONTERM(work_var, ret);

        return statement();
    }

    printf("16. <statement> -> e\n");
    return true;
}

bool work_var() {
    bool ret;

    if (token.type == T_L_ROUND_BR) {
        printf("17. <work_var> -> ( <args> )\n");
        NEXT_TOKEN();

        NEXT_NONTERM(args, ret);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

        NEXT_TOKEN();
    }
    else {
        printf("18. <work_var> -> <vars>\n");
        return vars();
    }

    return true;
}

bool vars() {
    if (token.type == T_COMMA) {
        printf("19. <vars> -> , id_var <vars>\n");

        NEXT_TOKEN();

        EXPECTED_TOKEN(token.type == T_ID);

        NEXT_TOKEN();
        return vars();
    }
    else if (token.type == T_ASSIGN) {
        printf("20. <vars> -> = <type_expr>\n");

        NEXT_TOKEN();

        return type_expr();
    }

    return false;
}

bool type_expr() {
    bool ret;

    if (token.type == T_ID) { // if var func
        printf("21. <type_expr> -> id_func ( <args> )\n");
        NEXT_TOKEN();

        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        NEXT_TOKEN();
        NEXT_NONTERM(args, ret);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

        NEXT_TOKEN();

        return true;
    }

    printf("22. <type_expr> -> <expression> <other_exp>\n");
    NEXT_NONTERM(expression, ret);
    return other_exp();
}

bool other_exp() {
    bool ret;

    if (token.type == T_COMMA) {
        printf("23. <other_exp> -> , <expression> <other_exp>\n");
        NEXT_TOKEN();
        NEXT_NONTERM(expression, ret);

        return other_exp();
    }

    printf("24. <other_exp> -> e\n");
    return true;
}

bool def_var() {
    if (token.type == T_ASSIGN) {
        printf("25. <def_var> -> = <init_assign>\n");

        NEXT_TOKEN();
        return init_assign();
    }

    printf("26. <def_var> -> e\n");
    return true;
}

bool init_assign() {
    bool ret;

    if (token.type == T_ID) {
        printf("27. <init_assign> -> id_func ( <args> )\n");

        NEXT_TOKEN();

        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        NEXT_TOKEN();
        NEXT_NONTERM(args, ret);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

        NEXT_TOKEN();
        return true;
    }

    printf("28. <init_assign> -> <expression>\n");
    return expression();
}

bool expression() {
    NEXT_TOKEN();
    return true;
}

bool type_returns() {
    bool ret;

    if (token.type == T_COLON) {
        printf("29. <type_returns> -> : <type> <other_types>\n");

        NEXT_TOKEN();
        NEXT_NONTERM(type, ret);

        return other_types();
    }

    printf("30. <type_returns> -> e\n");
    return true;
}

bool other_types() {
    bool ret;

    if (token.type == T_COMMA) {
        printf("31. <other_types> -> , <type> <other_types>\n");

        NEXT_TOKEN();
        NEXT_NONTERM(type, ret);

        return other_types();
    }

    printf("32. <other_types> -> e\n");
    return true;
}

bool params() {
    bool ret;

    if (token.type == T_ID) {
        printf("34. <params> -> id : <type> <other_params>\n");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);

        NEXT_TOKEN();
        NEXT_NONTERM(type, ret);

        return other_params();
    }

    printf("33. <params> -> e\n");
    return true;
}

bool other_params() {
    bool ret;

    if (token.type == T_COMMA) {
        printf("35. <other_params> -> , id : <type> <other_params>\n");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);

        NEXT_TOKEN();
        NEXT_NONTERM(type, ret);

        return other_params();
    }

    printf("36. <other_params> -> e\n");
    return true;
}

bool type_params() {
    if (type()) {
        printf("37. <type_params> -> <type> <other_types>\n");

        return other_types();
    }

    printf("38. <type_params> -> e\n");
    return true;
}

bool args() {
    if (token.type == T_ID     ||
        token.type == T_STRING ||
        token.type == T_FLOAT  ||
        token.type == T_INT)
    {
        printf("39. <args> -> id <other_args>\n");

        NEXT_TOKEN();
        return other_args();
    }

    printf("40. <args> -> e\n");
    return true;
}

bool other_args() {
    if (token.type == T_COMMA) {
        printf("41. <other_args> -> , <arg> <other_args>\n");

        NEXT_TOKEN();

        if (token.type == T_ID     ||
            token.type == T_STRING ||
            token.type == T_FLOAT  ||
            token.type == T_INT)
        {
            NEXT_TOKEN();
            return other_args();
        }

        return false;
    }

    printf("42. <other_args> -> e\n");
    return true;
}

int parser() {
    FILE *f = stdin;
    bool ret;
    err = NO_ERR;
    set_source_file(f);

    ret = str_init(&token.attr.id, 20);
    if (!ret) {
        return INTERNAL_ERR;
    }

    FIRST_TOKEN();
    ret = prog();

    if (!ret && err == NO_ERR) {
        printf("%d\n", ret);
        err = PARSER_ERR;
    }

    str_free(&token.attr.id);

    return err;
}
