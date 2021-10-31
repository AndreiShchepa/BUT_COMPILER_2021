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
bool DLL_Close(DLList *);
void DLL_Top(DLList *, char * );
void print_stack_debug(DLList * );
void DLL_InsertLast( DLList *, char * );
void DLL_GetFirst( DLList *, int * );
void DLL_GetLast( DLList *, int * );
int Get_Index_Of_String(char * );
#endif // _EXPRESSION_H
