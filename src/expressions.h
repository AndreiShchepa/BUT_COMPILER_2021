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
#define LENGHT_OF_OPERATORS 3
#define LENGHT_OF_RULES 5

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

typedef struct Element {
    char data[3];
    struct Element *previousElement;
    struct Element *nextElement;
} *ElementPtr;


typedef struct {
    ElementPtr firstElement;
    ElementPtr lastElement;
} List;

bool expression();

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
void Insert(List *, char *);

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
void Push(List *, char * );

/*
 * @brief We check if only thing left on stack is "$E"
 */
bool Check_Correct_Closure(List * );

/*
 * @brief Frees list and all its elements
 */
void Deallocate(List *list);

#endif // _EXPRESSIONS_H
