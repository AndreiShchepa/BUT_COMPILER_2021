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
char Chars[][3] = {
        {"#"}, {"*"}, {"/"}, {"//"},
        {"+"}, {"-"},
        {".."}, {"<"}, {"<="}, {">"}, {">="}, {"=="}, {"~="},
        {"("},
        {")"},
        {"i"},
        {"$"}
};
char Rules[][5] = {
        {"i"}, {"(E)"}, {"E+E"}, {"E-E"}, {"E*E"}, {"E/E"},
        {"E//E"}, {"#E"}, {"E<E"}, {"E<=E"}, {"E>E"}, {"E>=E"}, {"E==E"}, {"E~=E"}, {"E..E"}
};

void DLL_Init( DLList *list ) {
    list = malloc(sizeof(DLList));
    if(list == NULL){
        //todo Call error handler
    }
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

    // We add $ as the first element of stack
    list->firstElement->data[0] = '$';
    list->firstElement->data[1] = '\0';
}

void DLL_Dispose( DLLElementPtr Element ) {
    if(Element == NULL){
        //todo Call error handler
        return;
    }
    DLLElementPtr TempElement = Element;
    DLLElementPtr DelElement;
    while(TempElement != NULL){
        DelElement = TempElement;
        TempElement = TempElement->nextElement;
        free(DelElement);
    }
}



void DLL_Insert(DLList *list, char * data ) {

    if(list == NULL){
        //todo Call error handler
        return;
    }
    int flag = 0;

    DLLElementPtr find = list->lastElement;
    //todo ak next neni empty ked najdem prvy znak znamena to ze som preskocil E inak iba pridam na vrch stacku
    while((strcmp(find->data, "E") == 0) && find->previousElement != NULL){
        find = find->previousElement;
        flag = 1;
    }

    // We check if malloc was successful
    DLLElementPtr TempElement_first = malloc(sizeof(struct DLLElement));
    DLLElementPtr TempElement_second = malloc(sizeof(struct DLLElement));
    if (TempElement_first == NULL || TempElement_second == NULL) {
        //todo Call error handler
    }

    // If we found expression character on stack without going through E
    if(flag == 0){
        find->nextElement = TempElement_first;
        list->lastElement = TempElement_second;
        // We connect the chains
        // Found_Element -> first -> second -> NULL
        // $             -> <<    -> i      -> NULL
        TempElement_first->previousElement = find;
        TempElement_first->nextElement = TempElement_second;

        TempElement_second->previousElement = TempElement_first;
        TempElement_second->nextElement = NULL;

        strcpy(TempElement_first->data, "<<\0");
        strcpy(TempElement_second->data, data);
    // If we found E on stack, means we are copying << before E and data after E
    } else {
        DLLElementPtr Element_With_E = find->nextElement;

        find->nextElement = TempElement_first;
        list->lastElement = TempElement_second;
        // We connect the chains
        // Found_Element -> first -> E -> second -> NULL
        // $             -> <<    -> E -> +      -> NULL
        TempElement_first->previousElement = find;
        TempElement_first->nextElement = Element_With_E;

        Element_With_E->previousElement = TempElement_first;
        Element_With_E->nextElement = TempElement_second;

        TempElement_second->previousElement = Element_With_E;
        TempElement_second->nextElement = NULL;

        strcpy(TempElement_first->data, "<<\0");
        strcpy(TempElement_second->data, data);
    }
}

bool DLL_Close(DLList *list) {

    if(list == NULL){
        //todo Call error handler
        return false;
    }
    char Array_To_Check_Against_Rules[5] = {'\0'};

    DLLElementPtr find = list->lastElement;
    // We copy into our array until we dont find closing <<
    while((strcmp(find->data, "<<") != 0) && find->previousElement != NULL){
        strcat(Array_To_Check_Against_Rules, find->data);
    }
    // We check against rules
    for(int j = 0; j < 15; j++){
        // If we found correct rule
        if(strcmp(Array_To_Check_Against_Rules, Rules[j]) == 0){
            // We delete everything after <<
            DLL_Dispose (find->nextElement);
            // We change << with E
            strcpy(find->data, "E\0");
            find->nextElement = NULL;
            return true;
        }
    }
    return false;
}
// Returns top of the stack
void DLL_Top(DLList *list, char * data) {
    if(list == NULL){
        //todo Call error handler
        return;
    }
    strcpy(data, list->lastElement->data);
}

int Get_Index_Of_String(char * data){
    int i = 15;
    for(int j = 0; j < 17; j++){
        if(strcmp(data, Chars[j]) == 0){
            return j;
        }
    }
    return i;
}

bool expression() {
    // todo get rid of magic numbers
    // todo conflicts with < char as in <i> and < as in f<5, solution, "<" = "<<"
    char data[3] = {"$\0"};
    DLList list;
    DLL_Init(&list);

    char precedence = Precedence_Table[Get_Index_Of_String(data)][Get_Index_Of_String(token.attr.id.str)];

    while(TOKEN_ID_EXPRESSION()){
        xyz:
        DLL_Top(&list, data);
        precedence = Precedence_Table[Get_Index_Of_String(data)][Get_Index_Of_String(token.attr.id.str)];
        if(precedence == '<'){
            if(Get_Index_Of_String(token.attr.id.str) == 15){
                data[0] = 'i', data[1] = '\0';
            } else {
                strcpy(data, token.attr.id.str);
            }
            DLL_Insert(&list, data);
        } else if(precedence == '>'){
            DLL_Close(&list);
            goto xyz;
        }
        printf("%c TOKEN = \"%s\"\n", precedence, token.attr.id.str);
        NEXT_TOKEN();
    }

    return true;
}
