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
#include "symtable.h"

extern int err;
extern string_t tps_right;
extern arr_symtbs_t local_symtbs;
char Precedence_Table[][NUMBER_OF_OPERATORS] = {
        //#    *    /    //   +    -    ..   <    <=   >    >=   ==   ~=   (    )    i    $
        {'<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // #

        {'<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // *

        {'<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // /

        {'<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // //

        {'<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // +

        {'<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // -

        {'<', '<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // ..

        {'<', '<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // <

        {'<', '<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // <=

        {'<', '<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // >

        {'<', '<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // >=

        {'<', '<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // ==

        {'<', '<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, // ~=

        {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', 'c'}, // (

        {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'c', '>', 's', '>'}, // )

        {'c', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'c', '>', 's', '>'}, // i

        {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'c', '<', 'c'}, // $
};

char Chars[][LENGTH_OF_OPERATORS] = {
        {"#"}, {"*"}, {"/"}, {"//"},
        {"+"}, {"-"},
        {".."}, {"<"}, {"<="}, {">"}, {">="}, {"=="}, {"~="},
        {"("},
        {")"},
        {"i"},
        {"$"}
};

// We reversed the rules because when we copy from stack
// from top to bottom the expression (E) will become )E(
char Rules[][LENGTH_OF_RULES] = {
        {"i"}, {")E("}, {"E+E"}, {"E-E"}, {"E*E"}, {"E/E"},
        {"E//E"}, {"E#"}, {"E<E"}, {"E<=E"}, {"E>E"},
        {"E>=E"}, {"E==E"}, {"E~=E"}, {"E..E"}
};

#define GET_TYPE_ID(IDX) \
        do { \
            var = FIND_VAR_IN_SYMTAB; \
            if (!var) {  \
            err = SEM_DEF_ERR;             \
            return false;             \
            } \
            types_E[IDX] = var->data.var->type.str[0]; \
        } while(0);

#define GET_TYPE_TERM(IDX) \
        do {               \
            if((IDX) == 0){               \
                types_E[IDX] = Ei->element_token.type == T_INT    ? 'I' :      \
                               Ei->element_token.type == T_STRING ? 'S' :      \
                               Ei->element_token.type == T_FLOAT  ? 'F' : 'N'; \
            } else {       \
                types_E[IDX] = Ej->element_token.type == T_INT    ? 'I' :      \
                               Ej->element_token.type == T_STRING ? 'S' :      \
                               Ej->element_token.type == T_FLOAT  ? 'F' : 'N'; \
            }              \
        } while(0);

#define CLEAR_TYPES_E() types_E[0] = types_E[1] = '\0';

#define CHECK_NUMBER() \
        do { \
            if (strcmp(types_E, "II") && strcmp(types_E, "IF") && \
                strcmp(types_E, "FI") && strcmp(types_E, "FF")  ) \
            { \
                err = SEM_ARITHM_REL_ERR; \
                return false; \
            } \
        } while(0);

#define CHECK_CONC_LENGHT() \
        do { \
            if (strcmp(types_E, "SS")) \
            { \
                err = SEM_ARITHM_REL_ERR;                         \
                return false;                \
            }               \
            \
        } while(0);
#define CHECK_COMPARISON() \
        do { \
            if (strcmp(types_E, "II") && strcmp(types_E, "IF") && \
                strcmp(types_E, "FI") && strcmp(types_E, "FF") && \
                strcmp(types_E, "SS")) \
            { \
                err = SEM_ARITHM_REL_ERR; \
                return false; \
            } \
        } while(0);
#define ASSIGN_TYPE() \
        do {          \
            if ((strcmp(types_E, "II") == 0) || (j >= 7 && j <= 13)) \
            { \
                find->element_token.type = T_INT;          \
            } \
            else if (strcmp(types_E, "SS") == 0) \
            { \
                find->element_token.type = T_STRING;          \
            } \
            else \
            { \
                find->element_token.type = T_FLOAT;          \
            } \
        } while(0);
char types_E[2];
htab_item_t *var;
void print_stack_debug(List * list) {
    ElementPtr PrintElement = list->firstElement;
    while (PrintElement != NULL) {
        printf("%s", PrintElement->data);
        PrintElement = PrintElement->nextElement;
    }

    printf("\n");
}

// Return on which index we can find our precedence rule
int Get_Index_Of_String(char * data) {
    for (int j = 0; j < NUMBER_OF_OPERATORS; j++) {
        if (strcmp(data, Chars[j]) == 0) {
            return j;
        }
    }

    // if i remains 15 at the end of the for cycle,
    // it means data contains not one of our chars (+, -, <= etc.)
    // but a string or a variable
    return INDEX_OF_IDENTIFICATOR;
}

List * Init(List * list) {
    // We create the list and check for malloc
    list = malloc(sizeof(List));
    if (list == NULL) {
        return NULL;
    }

    // We create the first element and check for malloc
    ElementPtr TempElement = malloc(sizeof(struct Element));
    if (TempElement == NULL) {
        free(list);
        return NULL;
    }

    // Because there is only one element,
    // its both first and last one in the List
    list->firstElement = TempElement;
    list->lastElement = TempElement;

    // Because element is both first and last one in the List
    // it has no previous nor next element
    list->firstElement->previousElement = NULL;
    list->firstElement->nextElement = NULL;

    // We add $ as the first element of stack
    strcpy(list->firstElement->data, "$");
    TempElement->element_token.type = T_NONE;
    return list;
}

void Dispose(ElementPtr Element) {
    if (Element == NULL) {
        return;
    }

    // We create additional Element for deleting
    ElementPtr TempElement = Element;
    ElementPtr DelElement;

    // Deleting
    while (TempElement != NULL) {
        DelElement = TempElement;
        TempElement = TempElement->nextElement;
        free(DelElement);
        DelElement = NULL;
    }
}

bool Insert(List * list, char * data) {
    if (list == NULL) {
        return 0;
    }

    int flag = 0;

    ElementPtr find = list->lastElement;
    // If stack contains for example $E and data contains +,
    // we skip to previous element until we find one of
    // our chars(+, -, <= etc.) and
    // connect the element in a way to create $<<E+,

    // otherwise if there is no E on top of stack,
    // for example $<<E+, we just copy our << with data,
    // $<<E+ -> $<<E+<<(
    // Whether there is E on top of the stack we check with int flag
    while ((strcmp(find->data, "E") == 0) && find->previousElement != NULL) {
        find = find->previousElement;
        flag = 1;
    }

    // We check if malloc was successful
    ElementPtr TempElement_first = malloc(sizeof(struct Element));
    if (TempElement_first == NULL) {
        Deallocate(list);
        return false;
    }
    ElementPtr TempElement_second = malloc(sizeof(struct Element));
    if (TempElement_second == NULL) {
        free(TempElement_first);
        Deallocate(list);
        return false;
    }
    // If we found expression character on stack without going through E
    if (flag == 0) {
        find->nextElement = TempElement_first;
        list->lastElement = TempElement_second;
        // We connect the chains
        // Found_Element -> first -> second -> NULL
        // $             -> <<    -> i      -> NULL
        TempElement_first->previousElement = find;
        TempElement_first->nextElement = TempElement_second;

        TempElement_second->previousElement = TempElement_first;
        // If we found E on stack, means we are copying
        // << before E and data after E
    }
    else {
        ElementPtr Element_With_E = find->nextElement;

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
    }

    TempElement_second->nextElement = NULL;
    strcpy(TempElement_first->data, "<<");
    // If we are dealing not with character, but with an expression we copy the data of token into our structure
    if((strcmp(data, "i")) == 0){
        TempElement_second->element_token.attr = token.attr;
        TempElement_second->element_token.type = token.type;
        TempElement_second->element_token.keyword = token.keyword;
        // else we just set type to T_NONE so we can differentiate between an expression and a character
    } else {
        TempElement_second->element_token.type = T_NONE;
    }
    strcpy(TempElement_second->data, data);

    print_stack_expr(list);
    return true;
}

bool Close(List * list) {
    if (list == NULL){
        return false;
    }

    char Array_To_Check_Against_Rules[5] = {'\0'};

    ElementPtr find = list->lastElement;
    ElementPtr Ei = NULL;
    ElementPtr Ej = NULL;
    CLEAR_TYPES_E();

    // We copy into our array until we dont find closing <<
    while ((strcmp(find->data, "<<") != 0) && find->previousElement != NULL) {
        // We found Expression and the expression isn't in the form of "i" but "E" on stack
        // because we dont care about rule <i>
        // Also because stack is other way around we firstly start pointing with Ej and then with Ei
        if(find->element_token.type != T_NONE && (strcmp(find->data, "E")) == 0){
            if(Ej == NULL){
                Ej = find;
            } else {
                Ei = find;
            }
        } else if((strcmp(find->data, "i")) == 0){
            Ej = find;
        }
        strcat(Array_To_Check_Against_Rules, find->data);
        find = find->previousElement;
    }

    if(Ej != NULL){

        // Special case with rule #E or <i> where there is only one expression so Ei will stay NULL, we make sure Ei isn't NULL
        // so we can work without passing null pointer
        if(Ei == NULL){
            Ei = Ej;
        }
        if (Ei->element_token.type == T_ID) {
            GET_TYPE_ID(0);
        } else {
            GET_TYPE_TERM(0);
        }

        if (Ej->element_token.type == T_ID) {
            GET_TYPE_ID(1);
        } else {
            GET_TYPE_TERM(1);
        }
    } else {
        // todo There is no expression (missing E) i.e. <<+>>
//        return false;
    }
    // We check against rules
    for(int j = 0; j < 15; j++) {
        // If we found correct rule
        if (strcmp(Array_To_Check_Against_Rules, Rules[j]) == 0) {
            // Assigns type, if II, type == int, SS = string, else = float
            ASSIGN_TYPE()
            // In this if statement we make sure that if rules #E = 7 and E..E = 14 were
            // found the expressions are also strings
            if(j == 7 || j == 14){
                CHECK_CONC_LENGHT()
                // Special case where assign type would assign string due to #E being string operation, but
                // the result of this operation is size in numbers
                if(j == 7){
                    find->element_token.type = T_INT;
                }
                // If we are doing comparison rules
            } else if(j >= 8 && j <= 13){
                find->element_token.type = T_INT;
                CHECK_COMPARISON()
                // If we found any other rules other than
                // i = 0, (E) = 1 operations where it doesn't matter of what type the expression is
                // #E = 7, E..E = 14 string operations
                // and the type is string, that means we are trying to do number operation with strings
            } else if(j != 0 && j != 1){
                CHECK_NUMBER()
            }
            // We change << with E
            printf("Rule: %d TYPE of reduced expression: %d\n\n", j, find->element_token.type);
            strcpy(find->data, "E");
            // We delete everything after <<
            Dispose(find->nextElement);
            find->nextElement = NULL;
            list->lastElement = find;

            return true;
        }
    }

    // We were not successful in finding a rule
    return false;
}

void Top(List * list, char data[]) {
    if (list == NULL){
        return;
    }

    ElementPtr find = list->lastElement;
    // If stack contains for example $E, we skip to previous element
    // until we find one of our chars($, +, -, <= etc.)
    while ((strcmp(find->data, "E") == 0) && find->previousElement != NULL) {
        find = find->previousElement;
    }

    strcpy(data, find->data);
}

bool Push(List * list, char * data) {
    if (list == NULL) {
        return false;
    }

    // We create our new element
    ElementPtr TempElement = malloc(sizeof(struct Element));
    if (TempElement == NULL) {
        Deallocate(list);
        return false;
    }

    // We find the position of last element on the stack
    ElementPtr find = list->lastElement;
    find->nextElement = TempElement;

    // Connect the chains
    TempElement->previousElement = find;
    TempElement->nextElement = NULL;
    list->lastElement = TempElement;

    // Copy onto the stack and we also copy T_NONE into token because there will never be situation
    // where we just arbitrary copy i onto the stack because
    // there doesnt exist precedence rule "=" with expressions, only with characters
    TempElement->element_token.type = T_NONE;
    strcpy(TempElement->data, data);
    return true;
}

bool Check_Correct_Closure(List * list) {
    if (list != NULL) {
        // We just forcefully check if first element on stack
        // is $ and the second one E
        if ((strcmp(list->firstElement->data, "$") == 0) &&
            list->firstElement->nextElement != NULL &&
           (strcmp(list->firstElement->nextElement->data, "E") == 0))
        {
            return true;
        }
    }

    return false;
}

void Deallocate(List * list) {
    if (list != NULL) {
        Dispose(list->firstElement);
        free(list);
        list = NULL;
    }
}

bool expression() {
    bool nothing_todo = true;
    char data[3] = {"$"};
    List * list = NULL;
    list = Init(list);

    char precedence;

    while ((TOKEN_ID_EXPRESSION()) && (list != NULL)) {
        nothing_todo = false;

start_expr:
        // Look what char is on top of the stack (+, -, <= etc.)
        Top(list, data);

        /* Take index of a character that is in data,
         * for example + has index of 5 due to being 5th in Chars[] array,
         * we find that through comparing strings Chars[] and data
         * takes index of a character that is in token,
         * for example i has index of 15 due to being 15th in Chars[] array,
         * we find that through comparing strings Chars[] and token.attr.id.str
         * now we can get precedence with command Precedence_Table[5][15]
         * and the precedence is <, because +<i in our precedence table
         * precedence = '<';
         */
        precedence = Precedence_Table[GET_ID(data)][GET_ID(token.attr.id.str)];
        print_dbg_msg_multiply("on top: %s, token: %s, precedence: %c\n",
                    data, token.attr.id.str, precedence);

        if (precedence == '<') {
            // If token is a variable or a string we put i on the stack instead
            // of copying the whole name of the variable or whole string
            if (GET_ID(token.attr.id.str) == INDEX_OF_IDENTIFICATOR) {
                strcpy(data, "i");
            }
            else {
                // else we copy the character from the token (+, -, <= etc.)
                strcpy(data, token.attr.id.str);
            }

            // We copy the character from the token with << added
            // infront of E $<<E+ or $<<E+<<( (+, -, <= etc.)
            if(!Insert(list, data)){
                list = NULL;
                err = INTERNAL_ERR;
                goto err_expr;
            }
        }
        else if (precedence == '>') {
            // We find the first << from the top of the stack to the bottom
            // and check it against the rules, if we didnt find a rule
            // we return false as expression is wrong
            if (!Close(list)) {
                goto err_expr;
            }

            print_stack_expr(list);
            // If we found a rule, we check the precedence of the new created stack
            // by returning to the start of the while cycle
            goto start_expr;
        }
        else if (precedence == '=') {
            // We just copy the data onto the stack
            if(!Push(list, token.attr.id.str)){
                err = INTERNAL_ERR;
                list = NULL;
                goto err_expr;
            }
            print_stack_expr(list);
        }
        else if (precedence == 's') {
            // Special incident with finding identificator after identificator,
            // which means the expression has ended and we close our stack
            // and check against the rules
            goto end_expr;
        }
        else if (precedence == 'c') {
            // If we found something that doesnt have correct order, for example )(
            goto err_expr;
        }
        NEXT_TOKEN();
    }

    if (nothing_todo) {
        Deallocate(list);
        return false;
    }

    // If there is internal error such as failure to allocate,
    // list will free itself which means none of the functions
    // will do anything and eventually we will learn

    // that if list is empty and we never did combination of return true
    // false with deallocation of list that there was an error along the way
    if (list == NULL) {
        err = INTERNAL_ERR;
        return false;
    }

    print_dbg_msg_single("reduction: ");
    print_stack_expr(list);

end_expr:

    // We are reducing the expression by using our rules
    while (Close(list)) {
        print_stack_expr(list);
    }

    // If we were successful in reducing the expression and there wasn't any error
    if (Check_Correct_Closure(list) && err == NO_ERR) {
        printf("expression type: %d\n", list->lastElement->element_token.type);
        Deallocate(list);
        return true;
    }

err_expr:
    Deallocate(list);
    return false;
}
