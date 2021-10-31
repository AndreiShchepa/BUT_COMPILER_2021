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

typedef struct DLLElement {
    char data[3];
    struct DLLElement *previousElement;
    struct DLLElement *nextElement;
} *DLLElementPtr;


typedef struct {
    DLLElementPtr firstElement;
    DLLElementPtr lastElement;
} DLList;

bool expression();

/*
 * @brief The first initialization, we create the stack and put $ as the first element of the stack
 */
DLList * Init(DLList * );

/*
* @brief We delete everything after Element with Element included
*/
void Dispose(DLLElementPtr );

/*
* @brief We are inserting << with its char (+, -, <= etc.)
*/
void Insert(DLList *, char *);

/*
* @brief We close the first part of the expression we find, for example <<E+<<i -> <<E+E
*/
bool Close(DLList * );

/*
* @brief Returns top of the stack
*/
void Top(DLList *, char * );

/*
* @brief We Copy a string on top of the stack
*/
void Push(DLList *, char * );

/*
* @brief debug tool
*/
void print_stack_debug(DLList * );

/*
* @brief We check if only thing left on stack is "$E"
*/
bool Check_Correct_Closure(DLList * );

/*
* @brief Return on which index we can find our precedence rule
*/
int Get_Index_Of_String(char * );

/*
* @brief Frees list amd all its elements
*/
void Deallocate(DLList *list);
#endif // _EXPRESSION_H
