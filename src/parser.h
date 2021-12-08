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
#include "queue.h"

#define COUNT_DEF_FUNCS 8

// If you want to see all rules, which are used for process your program
// Pass RULES to cmake or read README.md
#ifdef DEBUG_RULES
    #define print_rule(s) printf("%s\n", (s))
#else
    #define print_rule(s)
#endif

#define CHECK_INTERNAL_ERR(COND, ret) \
        do {                          \
            if (COND) {               \
                err = INTERNAL_ERR;   \
                return ret;           \
            }                         \
        } while(0)

#define CHECK_SEM_DEF_ERR(COND)    \
        do {                       \
            if (COND) {            \
                err = SEM_DEF_ERR; \
                return false;      \
            }                      \
        } while(0)

// Get next token and check for INTERNAL/SCANNER error
#define NEXT_TOKEN()                  \
        err = get_next_token(&token); \
        if (err != NO_ERR) {          \
            return false;             \
        }

// If condition is false, PARSER error
#define EXPECTED_TOKEN(cond) \
        if (!(cond)) {       \
            return false;    \
        }

// Call function for expected nonterminal,
// in case of error return false
#define NEXT_NONTERM(nonterm) \
        ret = (nonterm);      \
        if (!ret) {           \
            return false;     \
        }

// Get the first token from code
#define FIRST_TOKEN()                 \
        err = get_next_token(&token); \
        if (err != NO_ERR) {          \
            goto end_parser;          \
        }

// Check if type of token is id or some term
#define TOKEN_TERM() token.keyword == KW_NIL || \
                     token.type == T_INT     || \
                     token.type == T_FLOAT   || \
                     token.type == T_STRING

extern token_t token;
extern int err;
extern Queue* queue_id;
extern Queue* queue_expr;
extern arr_symtbs_t local_symtbs;

/*
 * @brief process start of the code
 *
 * 1. <prolog> -> require t_string <prog>
 */
bool prolog();

/*
 * @brief process all rules start with nonterminal <prog>
 *
 * 2. <prog> -> GLOBAL ID : FUNCTION ( <arg_T> ) <ret_T> <prog>
 * 3. <prog> -> FUNCTION ID ( <arg> ) <ret_T> <stmt> END <prog>
 * 4. <prog> -> ID ( <param> ) <prog>
 * 5. <prog> -> EOF
 */
bool prog();

/*
 * @brief process all possible types of variables as arguments in functions
 *
 * 6. <arg_T> -> <type> <next_arg_T>
 * 7. <arg_T> -> e
 */
bool arg_T();

/*
 * @brief process all possible types of variables in function arguments
 *
 * 8. <next_arg_T> -> , <type> <next_arg_T>
 * 9. <next_arg_T> -> e
 */
bool next_arg_T();

/*
 * @brief process all possible types of variables in function return
 *
 * 10. <ret_T> -> : <type> <next_ret_T>
 * 11. <ret_T> -> e
 */
bool ret_T();

/*
 * @brief process all possible types of variables in function return
 *
 * 12. <next_ret_T> -> , <type> <next_ret_T>
 * 13. <next_ret_T> -> e
 */
bool next_ret_T();

/*
 * @brief process params as arguments for function
 *
 * 14. <arg> -> ID : <type> <next_arg>
 * 15. <arg> -> e
 */
bool arg();

/*
 * @brief process params as arguments for function
 *
 * 16. <next_arg> -> , ID : <type> <next_arg>
 * 17. <next_arg> -> e
 */
bool next_arg();

/*
 * @brief process all possible types of variables
 *
 * 18. <type> -> integer
 * 19. <type> -> number
 * 20. <type> -> string
 * 21. <type> -> nil
 */
bool type();

/*
 * @brief process all rules start with nonterminal <statement>
 *
 * 22. <stmt> -> IF <expr> THEN <stmt> ELSE <stmt> END <stmt>
 * 23. <stmt> -> WHILE <expr> DO <stmt> END <stmt>
 * 24. <stmt> -> LOCAL ID : <type> <def_var> <stmt>
 * 25. <stmt> -> RETURN <expr> <next_exp> <stmt>
 * 26. <stmt> -> ID <fork_id> <stmt>
 * 27. <stmt> -> e
 */
bool stmt();

/*
 * @brief process rules for definition of variables
 *
 * 28. <def_var> -> = <one_assign>
 * 29. <def_var> -> e
 */
bool def_var();

/*
 * @brief process rules for the first initializing of variables
 *
 * 30. <one_assign> -> ID ( <param> )
 * 31. <one_assign> -> <expr>
 */
bool one_assign();

/*
 * @brief process all possible args
 *
 * 32. <param> -> <param_val> <next_param>
 * 33. <param> -> e
 */
bool param();

/*
 * @brief process args
 *
 * 34. <param_val> -> ID
 * 35. <param_val> -> <term>
 */
bool param_val();

/*
 *
 * 36. <term> -> t_string
 * 37. <term> -> t_integer
 * 38. <term> -> t_number
 * 39. <term> -> nil
 */
bool term();

/*
 * @brief process all possible other args
 *
 * 40. <next_param> -> , <param_val> <next_param>
 * 41. <next_param> -> e
 */
bool next_param();

/*
 * @brief process multiply expressions
 *
 * 42. <next_expr> -> , <expr> <next_expr>
 * 43. <next_expr> -> e
 */
bool next_expr();

/*
 * @brief
 *
 * 44. <fork_id> -> ( <param> )
 * 45. <fork_id> -> <next_id>
 */
bool fork_id();

/*
 * @brief process rules for multiply assigning
 *
 * 46. <next_id> -> , ID <next_id>
 * 47. <next_id> -> = <mult_assign>
 */
bool next_id();

/*
 * @brief process rules for assigning
 *
 * 48. <mult_assign> -> ID ( <param> )
 * 49. <mult_assign> -> <expr> <next_expr>
 */
bool mult_assign();

/*
 * @brief get first token and start the parser
 *
 * @return On success NO_ERR, otherwise PARSER/INTERNAL/SCANNER error
 */
int parser();

#endif // _PARSER_H
