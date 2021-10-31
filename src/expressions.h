/**
 * Project: Compiler IFJ21
 *
 * @file expression.h
 *
 * @brief
 *
 * @author
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
DLList * Init(DLList * );
void Dispose(DLLElementPtr );
void Insert(DLList *, char *);
bool Close(DLList * );
void Top(DLList *, char * );
void Push(DLList *, char * );
void print_stack_debug(DLList * );
bool Check_Correct_Closure(DLList * );
int Get_Index_Of_String(char * );
void Deallocate(DLList *list);
#endif // _EXPRESSION_H
