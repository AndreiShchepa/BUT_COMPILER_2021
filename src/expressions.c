/**
 * Project: Compiler IFJ21
 *
 * @file expression.c
 *
 * @brief Implement functions processing the correct order of identificators in expressions
 *
 * @author Richard Gajdosik <xgajdo33>
 */

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "parser.h"
#include "error.h"
#include "expressions.h"
//typedef union token_attr {
//    string_t id;
//    uint64_t num_i;
//    double num_f;
//} token_attr_t;
//
//typedef struct token {
//    token_type_t type;
//    keywords_t keyword;
//    token_attr_t attr;
//} token_t;

//    11. <statement> → if <expression>  then <statement>  else
//        <statement>  end <statement>
//    12. <statement> → while <expression>  do <statement>  end
//                <statement>
//    15. <statement> → return <expression>  <other_exp>
//    <statement>
//    22. <type_expr> → <expression>  <other_exp>
//    23. <other_exp> → , <expression> <other_exp>
//    28. <init_assign> → <expression>

// We allocate space for the struct and check whether the malloc was successful

char Precedence_Table[][17] = {
        {"<>>>>>>>>>>>><><>"}, // #
        {"<>>>>>>>>>>>><><>"}, // *
        {"<>>>>>>>>>>>><><>"}, // /
        {"<>>>>>>>>>>>><><>"}, // //

        {"<<<<>>>>>>>>><><>"}, // +
        {"<<<<>>>>>>>>><><>"}, // -

        {"<<<<<<<>>>>>><><>"}, // ..
        {"<<<<<<<>>>>>><><>"}, // <<
        {"<<<<<<<>>>>>><><>"}, // <=
        {"<<<<<<<>>>>>><><>"}, // >>
        {"<<<<<<<>>>>>><><>"}, // >=
        {"<<<<<<<>>>>>><><>"}, // ==
        {"<<<<<<<>>>>>><><>"}, // ~=

        {"<<<<<<<<<<<<<<=<c"}, // (

        {">>>>>>>>>>>>>c>c>"}, // )

        {"c>>>>>>>>>>>>c>s>"}, // i

        {"<<<<<<<<<<<<<<c<c"} // $
};
char Chars[][17] = {
        {"#"}, {"*"}, {"/"}, {"//"},
        {"+"}, {"-"},
        {".."}, {"<"}, {"<="}, {">"}, {">="}, {"=="}, {"~="},
        {"("},
        {")"},
        {"i"},
        {"$"}
};
char Rules[][17] = {
        {"i"}, {"+E"}, {"(E)"}, {"E+E"}, {"E-E"}, {"E*E"}, {"E/E"},
        {"E//E"}, {"#E"}, {"E<E"}, {"E<=E"}, {"E>E"}, {"E>=E"}, {"E==E"}, {"E~=E"}, {"E..E"}
};

void DLL_Init( DLList *list ) {
    list = malloc(sizeof(DLList));
    if(list == NULL){
       printf("not working");
    }
}

void DLL_Dispose( DLList *list ) {
    if(list == NULL){
        //todo Call error handler
        return;
    }
    DLLElementPtr TempElement = list->firstElement;
    DLLElementPtr DelElement;
    while(TempElement != NULL){
        DelElement = TempElement;
        TempElement = TempElement->nextElement;
        free(DelElement);
    }
    list->firstElement = NULL;
    list->lastElement = NULL;
}

void DLL_InsertFirst( DLList *list, char * data ) {

    if(list == NULL){
        //todo Call error handler
        return;
    }
    // We check if malloc was successful
    DLLElementPtr TempElement = malloc(sizeof(struct DLLElement));
    if (TempElement == NULL) {
        //todo Call error handler
    }

    // Because there is only one element, its both first and last one in the List
    list->firstElement = TempElement;
    list->lastElement = TempElement;

    // Because element is both first and last one in the List it has no previous nor next element

    list->firstElement->previousElement = NULL;
    list->firstElement->nextElement = NULL;

    //todo we have to recognize whether the token is one of our chars OR identificator which can be longer than our char array of size 3
    strcpy(list->firstElement->data, data);
    list->firstElement->data[2] = '\0';
}

bool expression() {
    char data[3] = {"$\0"};
    // todo conflicts with < char as in <i> and <=, solution, "<" = "<<"

    DLList list;
    DLL_Init(&list);
    DLL_InsertFirst(&list, data);

    printf("%s\n", list.firstElement->data);
    printf("%c %c %s %s\n", data[0], Precedence_Table[0][0], Chars [0], Rules[0]);
    NEXT_TOKEN();
    return true;
}
