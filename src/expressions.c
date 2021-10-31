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

char Precedence_Table[][NUMBER_OF_OPERATORS] = {
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
char Chars[][LENGHT_OF_OPERATORS] = {
        {"#"}, {"*"}, {"/"}, {"//"},
        {"+"}, {"-"},
        {".."}, {"<"}, {"<="}, {">"}, {">="}, {"=="}, {"~="},
        {"("},
        {")"},
        {"i"},
        {"$"}
};

// We reversed the rules because when we copy from stack from top to bottom the expression (E) will become )E(
char Rules[][LENGHT_OF_RULES] = {
        {"i"}, {")E("}, {"E+E"}, {"E-E"}, {"E*E"}, {"E/E"},
        {"E//E"}, {"E#"}, {"E<E"}, {"E=<E"}, {"E>E"}, {"E=>E"}, {"E==E"}, {"E=~E"}, {"E..E"}
};

DLList * Init(DLList * list) {
    // We create the list and check for malloc
    list = malloc(sizeof(DLList));
    if(list == NULL){
        return NULL;
    }

    // We create the first element and check for malloc
    DLLElementPtr TempElement = malloc(sizeof(struct DLLElement));
    if (TempElement == NULL) {
        return NULL;
    }

    // Because there is only one element, its both first and last one in the List
    list->firstElement = TempElement;
    list->lastElement = TempElement;

    // Because element is both first and last one in the List it has no previous nor next element
    list->firstElement->previousElement = NULL;
    list->firstElement->nextElement = NULL;

    // We add $ as the first element of stack
    strcpy(list->firstElement->data, "$\0");
    return list;
}
void Dispose(DLLElementPtr Element) {
    if(Element == NULL){
        return;
    }
    // We create additional Element to help us with deletion
    DLLElementPtr TempElement = Element;
    DLLElementPtr DelElement;
    // Deleting
    while(TempElement != NULL){
        DelElement = TempElement;
        TempElement = TempElement->nextElement;
        free(DelElement);
    }
}
void Insert(DLList * list, char * data) {
//    printf("insert: %s -> ", data);
    if(list == NULL){
        return;
    }
    int flag = 0;

    DLLElementPtr find = list->lastElement;
    // If stack contains for example $E and data contains +, we skip to previous element until we find one of our chars(+, -, <= etc.) and
    // connect the element in a way to create $<<E+,

    // otherwise if there is no E on top of stack, for example $<<E+, we just copy our << with data, $<<E+ -> $<<E+<<(
    // Whether there is E on top of the stack we check with int flag
    while((strcmp(find->data, "E") == 0) && find->previousElement != NULL){
        find = find->previousElement;
        flag = 1;
    }

    // We check if malloc was successful
    DLLElementPtr TempElement_first = malloc(sizeof(struct DLLElement));
    DLLElementPtr TempElement_second = malloc(sizeof(struct DLLElement));
    if (TempElement_first == NULL || TempElement_second == NULL) {
        Deallocate(list);
        return;
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
    print_stack_debug(list);
}
bool Close(DLList * list) {
    if(list == NULL){
        return false;
    }

    char Array_To_Check_Against_Rules[5] = {'\0'};

    DLLElementPtr find = list->lastElement;

    // We copy into our array until we dont find closing <<
    while((strcmp(find->data, "<<") != 0) && find->previousElement != NULL){
        strcat(Array_To_Check_Against_Rules, find->data);
        find = find->previousElement;
    }
    // We check against rules
    for(int j = 0; j < 15; j++){
        // If we found correct rule
        if(strcmp(Array_To_Check_Against_Rules, Rules[j]) == 0){
            // We delete everything after <<
            Dispose(find->nextElement);
            // We change << with E
            strcpy(find->data, "E");
            find->nextElement = NULL;
            list->lastElement = find;
            // We were successful in finding a rule
            return true;
        }
    }
    // We were not successful in finding a rule
    return false;
}
void Top(DLList * list, char data[]) {
    if(list == NULL){
        return;
    }
    DLLElementPtr find = list->lastElement;
    // If stack contains for example $E, we skip to previous element until we find one of our chars($, +, -, <= etc.)
    while((strcmp(find->data, "E") == 0) && find->previousElement != NULL){
        find = find->previousElement;
    }
    strcpy(data, find->data);
}
void Push(DLList * list, char * data) {
    if(list == NULL){
        return;
    }
    // We create our new element
    DLLElementPtr TempElement = malloc(sizeof(struct DLLElement));
    if(TempElement == NULL){
        Deallocate(list);
        return;
    }
    // We find the position of last element on the stack
    DLLElementPtr find = list->lastElement;

    find->nextElement = TempElement;
    // Connect the chains
    TempElement->previousElement = find;
    TempElement->nextElement = NULL;
    list->lastElement = TempElement;
    // Copy onto the stack
    strcpy(TempElement->data, data);
}
void print_stack_debug(DLList * list){
    DLLElementPtr PrintElement = list->firstElement;
    while(PrintElement != NULL) {
        printf("%s", PrintElement->data);
        PrintElement = PrintElement->nextElement;
    }
    printf("\n");
}
bool Check_Correct_Closure(DLList * list){
    if(list != NULL){
        // We just forcefully check if first element on stack is $ and the second one E
        //Todo this is very obscure way to do this, also should add if statements to check if we are not dealing with NULL pointer
        if((strcmp(list->firstElement->data, "$") == 0) && (strcmp(list->firstElement->nextElement->data, "E") == 0)){
            return true;
        }
    }
    return false;
}
int Get_Index_Of_String(char * data){
    // if i remains 15 at the end of the for cycle, it means data contains not one of our chars (+, -, <= etc.) but a string or a variable
    int i = INDEX_OF_IDENTIFICATOR;
    for(int j = 0; j < NUMBER_OF_OPERATORS; j++){
        if(strcmp(data, Chars[j]) == 0){
            return j;
        }
    }
    return i;
}
void Deallocate(DLList * list){
    if(list != NULL){
        Dispose(list->firstElement);
        free(list);
    }
}
bool expression() {
    //todo     ret = INTERNAL_ERR; pridat vsade kde moze vzniknut chyba kvoli malloc
    char data[3] = {"$"};
    DLList *list = NULL;
    list = Init(list);
    char precedence;

    while((TOKEN_ID_EXPRESSION()) && (list != NULL)){
        start:
        // Look what char is on top of the stack (+, -, <= etc.)
        Top(list, data);
        // Check the characters precedence against the found token
        precedence = Precedence_Table[Get_Index_Of_String(data)][Get_Index_Of_String(token.attr.id.str)];
        printf("top nasiel: %s oproti tokenu: %s, precedencia: %c \n", data, token.attr.id.str, precedence);
        if(precedence == '<'){
            // If token is a variable or a string we put i on the stack instead of copying the whole name of the variable or whole string
            if(Get_Index_Of_String(token.attr.id.str) == INDEX_OF_IDENTIFICATOR){
                strcpy(data, "i");
            } else {
                // else we copy the character from the token (+, -, <= etc.)
                strcpy(data, token.attr.id.str);
            }
            // We copy the character from the token with << added infront of E $<<E+ or $<<E+<<( (+, -, <= etc.)
            Insert(list, data);
        } else if(precedence == '>'){
            // We find the first << from the top of the stack to the bottom and check it against the rules, if we didnt find a rule we return false as expression is wrong
            if(!Close(list)){
                Deallocate(list);
                return false;
            }
            print_stack_debug(list);
            // If we found a rule, we check the precedence of the new created stack by returning to the start of the while cycle
            goto start;
        }
            // We just copy the data onto the stack
        else if(precedence == '='){
            Push(list, token.attr.id.str);
            print_stack_debug(list);
        }
            // Special incident with finding identificator after identificator, which means the expression has ended and we close our stack and check against the rules
        else if(precedence == 's'){
            while(Close(list)){
                print_stack_debug(list);
            }
            // If we were successful in reducing the expression
            if(Check_Correct_Closure(list)){
                printf("Vyraz je korektny.\n\n");
                Deallocate(list);
                return true;
            }
            Deallocate(list);
            return false;
            // If we found something that doesnt have correct order, for example )(
        } else if(precedence == 'c'){
            Deallocate(list);
            return false;
        }
        // Test print
//        printf("Precedence: %c%c TOKEN = \"%s\"\n", precedence, precedence, token.attr.id.str);
//        print_stack_debug(list);
        NEXT_TOKEN();
    }
    // If there is internal error such as failure to allocate, list will free itself which means none of the functions will do anything and eventually we will learn
    // that if list is empty and we never did combination of return true false with deallocation of list that there was an error along the way
    if(list == NULL){
        // todo ret
        return INTERNAL_ERR;
    }
    printf("we are closing this: ");
    print_stack_debug(list);

    // We are reducing the expression by using our rules
    while(Close(list)){
        print_stack_debug(list);
    }
    // If we were successful in reducing the expression
    if(Check_Correct_Closure(list)){
        printf("Vyraz je korektny.\n\n");
        Deallocate(list);
        return true;
    }
    Deallocate(list);
    return false;
}
