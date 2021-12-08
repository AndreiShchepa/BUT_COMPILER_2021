/**
 * Project: Compiler IFJ21
 *
 * @file expression.h
 *
 * @brief Implement functions processing the correct order of identificators in expressions
 *
 * @author Richard Gajdosik <xgajdo33>
 */

#ifndef _EXPRESSIONS_H
#define _EXPRESSIONS_H

#include <stdbool.h>
#define INDEX_OF_IDENTIFICATOR 15
#define NUMBER_OF_OPERATORS 17
#define LENGTH_OF_OPERATORS 3
#define LENGTH_OF_RULES 5

#define GET_ID(data) Get_Index_Of_String((data))

#ifdef DEBUG_EXPR
    #define print_dbg_msg_multiply(s, ...) printf((s), __VA_ARGS__)
    #define print_dbg_msg_single(s)        printf((s))
    #define print_stack_expr(list)         print_stack_debug((list))
#else
    #define print_dbg_msg_multiply(s, ...)
    #define print_dbg_msg_single(s)
    #define print_stack_expr(list)
#endif

#include "scanner.h"

// Check if type of token belongs to expression
#define TOKEN_ID_EXPRESSION() token.type    == T_ID         || \
                              token.type    == T_INT        || \
                              token.type    == T_FLOAT      || \
                              token.type    == T_STRING     || \
                              token.keyword == KW_NIL       || \
                              token.type    == T_PLUS       || \
                              token.type    == T_MINUS      || \
                              token.type    == T_MUL        || \
                              token.type    == T_DIV        || \
                              token.type    == T_DIV_INT    || \
                              token.type    == T_LT         || \
                              token.type    == T_GT         || \
                              token.type    == T_LE         || \
                              token.type    == T_GE         || \
                              token.type    == T_EQ         || \
                              token.type    == T_NEQ        || \
                              token.type    == T_LENGTH     || \
                              token.type    == T_L_ROUND_BR || \
                              token.type    == T_R_ROUND_BR || \
                              token.type    == T_CONCAT

typedef struct Element {
    token_t element_token;
    bool already_reduced;
    char type;
    char data[3];
    struct Element *previousElement;
    struct Element *nextElement;
} *ElementPtr;


typedef struct {
    ElementPtr firstElement;
    ElementPtr lastElement;
} List;

bool expr(bool bool_condition, bool bool_empty);

/*
 * @brief The first initialization, we create the stack and
 * put $ as the first element of the stack
 */
List * Init(List *);

/*
 * @brief We delete everything after Element with Element included
 */
void Dispose(ElementPtr );

/*
 * @brief We are inserting << with its char (+, -, <= etc.)
 */
bool Insert(List *, char *);

/*
 * @brief We close the first part of the expression we find,
 * for example <<E+<<i -> <<E+E
 */
bool Close(List * );

/*
 * @brief Returns top of the stack
 */
void Top(List *, char * );

/*
 * @brief We Copy a string on top of the stack
 */
bool Push(List *, char * );

/*
 * @brief We add tokens to queue for gen_code
 */
bool Add_Tokens_To_Queue(ElementPtr, ElementPtr, int );

/*
 * @brief We copy values from one token to another
 */
token_t * Copy_Values_From_Token(token_t *, token_t * );

/*
 * @brief We check if only thing left on stack is "$E"
 */
bool Check_Correct_Closure(List * );

/*
 * @brief Frees list and all its elements
 */
void Deallocate(List *list);

#endif // _EXPRESSIONS_H
