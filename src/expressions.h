/**
 * Project: Compiler IFJ21
 *
 * @file expression.h
 *
 * @brief Implement functions processing the correct order of identificators in expressions
 *
 * @author Richard Gajdosik <xgajdo33>
 */

#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include <stdbool.h>
#define INDEX_OF_IDENTIFICATOR 15
#define NUMBER_OF_OPERATORS 17
#define LENGHT_OF_OPERATORS 3
#define LENGHT_OF_RULES 5

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
List * Init(List * );

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
 * @brief debug tool
 */
void print_stack_debug(List * );

/*
 * @brief We check if only thing left on stack is "$E"
 */
bool Check_Correct_Closure(List * );

/*
 * @brief Return on which index we can find our precedence rule
 */
int Get_Index_Of_String(char * );

/*
 * @brief Frees list amd all its elements
 */
void Deallocate(List *list);

#endif // _EXPRESSION_H
