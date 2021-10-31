/**
 * Project: Compiler IFJ21
 *
 * @file parser.h
 *
 * @brief Contain functions declaration for processing rules based on LL-table.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */

#ifndef _PARSER_H
#define _PARSER_H

#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"

// If you want to see all rules, which are used for process your program
// Pass RULES to cmake or read README.md
#ifdef DEBUG_RULES
    #define print_rule(s) printf("%s\n", (s))
#else
    #define print_rule(s)
#endif

// Get next token and check for INTERNAL/SCANNER error
#define NEXT_TOKEN() \
        err = get_next_token(&token); \
        if (err != NO_ERR) { \
            return false; \
        }

// If condition is false, PARSER error
#define EXPECTED_TOKEN(cond) \
        if (!(cond)) { \
            return false; \
        }

// Call function for expected nonterminal,
// in case of error return false
#define NEXT_NONTERM(nonterm) \
        ret = (nonterm)(); \
        if (!ret) { \
            return false; \
        }

// Get the first token from code
#define FIRST_TOKEN() \
        err = get_next_token(&token); \
        if (err != NO_ERR) { \
            return err; \
        }

// Check if type of token is id or some term
#define TOKEN_ID_TERM() token.type == T_ID    || \
                        token.type == T_INT   || \
                        token.type == T_FLOAT || \
                        token.type == T_STRING

// Check if type of token belongs to expression
#define TOKEN_ID_EXPRESSION() token.type == T_ID          || \
                              token.type == T_INT         || \
                              token.type == T_FLOAT       || \
                              token.type == T_STRING      || \
                              token.type == T_PLUS        || \
                              token.type == T_MINUS       || \
                              token.type == T_MUL         || \
                              token.type == T_DIV         || \
                              token.type == T_DIV_INT     || \
                              token.type == T_LT          || \
                              token.type == T_GT          || \
                              token.type == T_LE          || \
                              token.type == T_GE          || \
                              token.type == T_EQ          || \
                              token.type == T_NEQ         || \
                              token.type == T_LENGTH      || \
                              token.type == T_L_ROUND_BR  || \
                              token.type == T_R_ROUND_BR  || \
                              token.type == T_CONCAT


extern token_t token;
extern int err;
extern htab_t *h_table;

/*
 * @brief process all rules start with nonterminal <prog>
 *
 * 1. <prog> -> <prolog> <prog>
 * 2. <prog> -> GLOBAL ID : FUNCTION ( <type_params> ) <type_returns> <prog>
 * 3. <prog> -> FUNCTION ID ( <params> ) <type_returns> <statement> <return_type> END <prog>
 * 4. <prog> -> ID_FUNC ( <args> ) <prog>
 * 5. <prog> -> EOF
 */
bool prog();

/*
 * @brief process rule start with nonterminal <prolog>
 *
 * 6. <prolog> -> REQUIRE TERM_STR
 */
bool prolog();

/*
 * @brief process all possible types of variables
 *
 * 7. <type> -> integer
 * 8. <type> -> number
 * 9. <type> -> string
 * 10. <type> -> nil
 */
bool type();

/*
 * @brief process all rules start with nonterminal <statement>
 *
 * 11. <statement> -> IF <expression> THEN <statement> ELSE <statement> END <statement>
 * 12. <statement> -> WHILE <expression> DO <statement> END <statement>
 * 13. <statement> -> LOCAL ID_VAR : <type> <def_var> <statement>
 * 14. <statement> -> ID <work_var> <statement>
 * 15. <statement> -> RETURN <expression> <other_exp> <statement>
 * 16. <statement> -> e
 */
bool statement();

/*
 * @brief process rules for manipulating with tokens after ID
 *
 * 17. <work_var> -> ( <args> )
 * 18. <work_var> -> <vars>
 */
bool work_var();

/*
 * @brief process rules for multiply assigning
 *
 * 19. <vars> -> , ID_VAR <vars>
 * 20. <vars> -> = <type_expr>
 */
bool vars();

/*
 * @brief process rules for assigning
 *
 * 21. <type_expr> -> ID_FUNC ( <args> )
 * 22. <type_expr> -> <expression> <other_exp>
 */
bool type_expr();

/*
 * @brief process myltiply assigning
 *
 * 23. <other_exp> -> , <expression> <other_exp>
 * 24. <other_exp> -> e
 */
bool other_exp();

/*
 * @brief process rules for definition of variables
 *
 * 25. <def_var> -> <init_assign>
 * 26. <def_var> -> e
 */
bool def_var();

/*
 * @brief process rules for the first initializing of variables
 *
 * 27. <init_assign> -> ID_FUNC ( <args> )
 * 28. <init_assign> -> <expression>
 */
bool init_assign();

/*
 * @brief process all possible types of variables in function return
 *
 * 29. <type_returns> -> : <type> <other_types>
 * 30. <type_returns> -> e
 */
bool type_returns();

/*
 * @brief process all possible types of variables in function return
 *
 * 31. <other_types> -> , <type> <other_types>
 * 32. <other_types> -> e
 */
bool other_types();

/*
 * @brief process params as arguments for function
 *
 * 33. <params> -> e
 * 34. <params> -> ID : <type> <other_params>
 */
bool params();

/*
 * @brief process params as arguments for function
 *
 * 35. <other_params> -> , ID : <type> <other_params>
 * 36. <other_params> -> e
 */
bool other_params();

/*
 * @brief process all possible types of variables as arguments in functions
 *
 * 37. <type_params> -> <type> <other_types>
 * 38. <type_params> -> e
 */
bool type_params();

/*
 * @brief process all possible args
 *
 * 39. <args> -> ID_VAR <other_args>
 * 40. <args> -> e
 */
bool args();

/*
 * @brief process all possible other args
 *
 * 41. <other_args> -> , ID_VAR <other_args>
 * 42. <other_args> -> e
 */
bool other_args();

/*
 * @brief get first token and start the parser
 *
 * @return On success NO_ERR, otherwise PARSER/INTERNAL/SCANNER error
 */
int parser();

#endif // _PARSER_H
