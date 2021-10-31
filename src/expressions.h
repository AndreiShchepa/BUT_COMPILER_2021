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
DLList * DLL_Init( DLList * );
void DLL_Dispose( DLLElementPtr );
void DLL_Insert(DLList *, char *);
bool DLL_Close(DLList * );
void DLL_Top(DLList *, char * );
void DLL_Push(DLList *, char * );
void print_stack_debug(DLList * );
bool Check_Correct_Closure(DLList * );
int Get_Index_Of_String(char * );
void Deallocate(DLList * );
#endif // _EXPRESSION_H
