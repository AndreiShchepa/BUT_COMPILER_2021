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

extern int err;

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

char Chars[][LENGHT_OF_OPERATORS] = {
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
char Rules[][LENGHT_OF_RULES] = {
        {"i"}, {")E("}, {"E+E"}, {"E-E"}, {"E*E"}, {"E/E"},
        {"E//E"}, {"E#"}, {"E<E"}, {"E<=E"}, {"E>E"},
        {"E>=E"}, {"E==E"}, {"E~=E"}, {"E..E"}
};

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
        TempElement_second->element_token.type = token.type;
        TempElement_second->element_token.attr = token.attr;
        TempElement_second->element_token.keyword = token.keyword;
//        printf("We are copying: %s %f %lu %s\n", data, token.attr.num_f, token.attr.num_i, token.attr.id.str);
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

    // We copy into our array until we dont find closing <<
    while ((strcmp(find->data, "<<") != 0) && find->previousElement != NULL) {
        strcat(Array_To_Check_Against_Rules, find->data);
        find = find->previousElement;
    }
    // Todo check if types are correct between expressions
    // We check against rules
    for(int j = 0; j < 15; j++) {
        // If we found correct rule
        if (strcmp(Array_To_Check_Against_Rules, Rules[j]) == 0) {
            // We delete everything after <<
            Dispose(find->nextElement);

            // We change << with E
            strcpy(find->data, "E");
            find->nextElement = NULL;
            list->lastElement = find;

            // We were successful in finding a rule
            // todo call gen_code_func(Ei, Ej)[j];
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

    // Copy onto the stack
    //todo copy only data? we should copy token data too
    strcpy(TempElement->data, data);
    return true;
}

bool Check_Correct_Closure(List * list) {
    if (list != NULL) {
        // We just forcefully check if first element on stack
        // is $ and the second one E

        // also should add if statements to check if
        // we are not dealing with NULL pointer
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
                // todo copy only data ? we  should copy token too
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
        return true;
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

    // If we were successful in reducing the expression
    if (Check_Correct_Closure(list)) {
        Deallocate(list);
        return true;
    }

err_expr:
    Deallocate(list);
    return false;
}
