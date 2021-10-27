#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "str.h"

token_t token;
int err;

#ifdef DEBUG_RULES
    #define print_rule(s) printf("%s\n", (s))
#else
    #define print_rule(s)
#endif

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
        print_rule("1.  <prog> -> <prolog> <prog>");

        NEXT_NONTERM(prolog, ret);
        return prog();
    }
    else if (token.keyword == KW_GLOBAL) {
        print_rule("2.  <prog> -> global id : function ( <type_params> ) <type_returns> <prog>");

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
        print_rule("3.  <prog> -> function id ( <params> ) <type_returns> <statement>"
                " <return_type> end <prog>");

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
        print_rule("4.  <prog> -> id_func ( <args> ) <prog>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        NEXT_TOKEN();
        NEXT_NONTERM(args, ret);

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

bool prolog() {
    print_rule("6.  <prolog> -> require term_str");

    EXPECTED_TOKEN(token.keyword == KW_REQUIRE);

    NEXT_TOKEN();
    EXPECTED_TOKEN(!str_cmp_const_str(&token.attr.id, "ifj21") && token.type == T_STRING);

    NEXT_TOKEN();

    return true;
}

bool type() {
    if (token.type == T_KEYWORD) {

        if (token.keyword == KW_INTEGER) {
            print_rule("7.  <type> -> integer");
        }
        else if (token.keyword == KW_NUMBER) {
            print_rule("8.  <type> -> number");
        }
        else if (token.keyword == KW_STRING) {
            print_rule("9.  <type> -> string");
        }
        else if (token.keyword == KW_NIL) {
            print_rule("10. <type> -> nil");
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
        print_rule("11. <statement> -> if <expression> then <statement> else <statement>"
                " end <statement>");

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
        print_rule("12. <statement> -> while <expression> do <statement>"
                " end <statement>");

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
        print_rule("13. <statement> -> local id_var : <type> <def_var> <statement>");

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
        print_rule("15. <statement> -> return <expression> <other_exp> <statement>");
        NEXT_TOKEN();

        NEXT_NONTERM(expression, ret);

        NEXT_NONTERM(other_exp, ret);

        return statement();
    }
    else if (token.type == T_ID) {
        print_rule("14. <statement> -> id <work_var> <statement>");

        NEXT_TOKEN();

        NEXT_NONTERM(work_var, ret);

        return statement();
    }

    print_rule("16. <statement> -> e");
    return true;
}

bool work_var() {
    bool ret;

    if (token.type == T_L_ROUND_BR) {
        print_rule("17. <work_var> -> ( <args> )");
        NEXT_TOKEN();

        NEXT_NONTERM(args, ret);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

        NEXT_TOKEN();
    }
    else {
        print_rule("18. <work_var> -> <vars>");
        return vars();
    }

    return true;
}

bool vars() {
    if (token.type == T_COMMA) {
        print_rule("19. <vars> -> , id_var <vars>");

        NEXT_TOKEN();

        EXPECTED_TOKEN(token.type == T_ID);

        NEXT_TOKEN();
        return vars();
    }
    else if (token.type == T_ASSIGN) {
        print_rule("20. <vars> -> = <type_expr>");

        NEXT_TOKEN();

        return type_expr();
    }

    return false;
}

bool type_expr() {
    bool ret;

    if (token.type == T_ID) { // if var func
        print_rule("21. <type_expr> -> id_func ( <args> )");
        NEXT_TOKEN();

        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        NEXT_TOKEN();
        NEXT_NONTERM(args, ret);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

        NEXT_TOKEN();

        return true;
    }

    print_rule("22. <type_expr> -> <expression> <other_exp>");
    NEXT_NONTERM(expression, ret);
    return other_exp();
}

bool other_exp() {
    bool ret;

    if (token.type == T_COMMA) {
        print_rule("23. <other_exp> -> , <expression> <other_exp>");
        NEXT_TOKEN();
        NEXT_NONTERM(expression, ret);

        return other_exp();
    }

    print_rule("24. <other_exp> -> e");
    return true;
}

bool def_var() {
    if (token.type == T_ASSIGN) {
        print_rule("25. <def_var> -> = <init_assign>");

        NEXT_TOKEN();
        return init_assign();
    }

    print_rule("26. <def_var> -> e");
    return true;
}

bool init_assign() {
    bool ret;

    if (token.type == T_ID) {
        print_rule("27. <init_assign> -> id_func ( <args> )");

        NEXT_TOKEN();

        EXPECTED_TOKEN(token.type == T_L_ROUND_BR);

        NEXT_TOKEN();
        NEXT_NONTERM(args, ret);

        EXPECTED_TOKEN(token.type == T_R_ROUND_BR);

        NEXT_TOKEN();
        return true;
    }

    print_rule("28. <init_assign> -> <expression>");
    return expression();
}

bool expression() {
    NEXT_TOKEN();
    return true;
}

bool type_returns() {
    bool ret;

    if (token.type == T_COLON) {
        print_rule("29. <type_returns> -> : <type> <other_types>");

        NEXT_TOKEN();
        NEXT_NONTERM(type, ret);

        return other_types();
    }

    print_rule("30. <type_returns> -> e");
    return true;
}

bool other_types() {
    bool ret;

    if (token.type == T_COMMA) {
        print_rule("31. <other_types> -> , <type> <other_types>");

        NEXT_TOKEN();
        NEXT_NONTERM(type, ret);

        return other_types();
    }

    print_rule("32. <other_types> -> e");
    return true;
}

bool params() {
    bool ret;

    if (token.type == T_ID) {
        print_rule("34. <params> -> id : <type> <other_params>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);

        NEXT_TOKEN();
        NEXT_NONTERM(type, ret);

        return other_params();
    }

    print_rule("33. <params> -> e");
    return true;
}

bool other_params() {
    bool ret;

    if (token.type == T_COMMA) {
        print_rule("35. <other_params> -> , id : <type> <other_params>");

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_ID);

        NEXT_TOKEN();
        EXPECTED_TOKEN(token.type == T_COLON);

        NEXT_TOKEN();
        NEXT_NONTERM(type, ret);

        return other_params();
    }

    print_rule("36. <other_params> -> e");
    return true;
}

bool type_params() {
    if (type()) {
        print_rule("37. <type_params> -> <type> <other_types>");

        return other_types();
    }

    print_rule("38. <type_params> -> e");
    return true;
}

bool args() {
    if (token.type == T_ID     ||
        token.type == T_STRING ||
        token.type == T_FLOAT  ||
        token.type == T_INT)
    {
        print_rule("39. <args> -> id_var <other_args>");

        NEXT_TOKEN();
        return other_args();
    }

    print_rule("40. <args> -> e");
    return true;
}

bool other_args() {
    if (token.type == T_COMMA) {
        print_rule("41. <other_args> -> , <arg> <other_args>");

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

    print_rule("42. <other_args> -> e");
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
        err = PARSER_ERR;
    }

    str_free(&token.attr.id);

    return err;
}
