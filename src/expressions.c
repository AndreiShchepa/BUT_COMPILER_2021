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
#include "queue.h"
#include "symstack.h"

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

#define ASSIGN_TYPE_FROM_TOKEN()                                                    \
        do {                                                                        \
            TempElement_second->type =  token.type == T_INT          ? 'I' :        \
                                        token.type == T_STRING       ? 'S' :        \
                                        token.type == T_FLOAT        ? 'F' :        \
                                        token.type == T_KEYWORD      ? 'N' : 'X';   \
        } while(0)

#define GET_TYPE_ID(IDX, KEY)                           \
        do {                                            \
            var = find_id_symtbs(&local_symtbs, (KEY)); \
            CHECK_SEM_DEF_ERR(!var);                    \
            types_E[IDX] = var->data.var->type.str[0];  \
        } while(0)

#define GET_TYPE_TERM(IDX)               \
        do {                             \
            if((IDX) == 0) {             \
                types_E[IDX] = Ei->type; \
            } else {                     \
                types_E[IDX] = Ej->type; \
            }                            \
        } while(0)

#define CLEAR_TYPES_E() types_E[0] = types_E[1] = '\0';

#define ASSIGN_TYPE()                                                            \
        do {                                                                     \
            if (types_E[0] == 'N' && types_E[1] == 'N') {                        \
                find->element_token.type = T_NONE;                               \
                find->element_token.keyword = KW_NIL;                            \
                find->type = 'N';                                                \
            }                                                                    \
            else if (rule == 5) {                                                \
                find->element_token.type = T_FLOAT;                              \
                find->type = 'F';                                                \
            }                                                                    \
            else if (rule == 7) {                                                \
                find->element_token.type = T_INT;                                \
                find->type = 'I';                                                \
            }                                                                    \
            else if ((rule >= 8 && rule <= 13) || strcmp(types_E, "CC") == 0) {  \
                find->element_token.type = T_NONE;                               \
                find->type = 'C';                                                \
            }                                                                    \
            else if (strcmp(types_E, "SS") == 0) {                               \
                find->element_token.type = T_STRING;                             \
                find->type = 'S';                                                \
            }                                                                    \
            else if (strcmp(types_E, "II") == 0) {                               \
                find->element_token.type = T_INT;                                \
                find->type = 'I';                                                \
            }                                                                    \
            else {                                                               \
                find->element_token.type = T_FLOAT;                              \
                find->type = 'F';                                                \
            }                                                                    \
        } while(0)

#define CHECK_CONC_LENGTH()                                         \
        do {                                                        \
            if (strcmp(types_E, "SS"))                              \
            {                                                       \
                err = SEM_ARITHM_REL_ERR;                           \
                return false;                                       \
            }                                                       \
        } while(0)

#define CHECK_COMPARISON()                                          \
        do {                                                        \
            if (!strcmp(types_E, "SI") || !strcmp(types_E, "IS") || \
                !strcmp(types_E, "FS") || !strcmp(types_E, "SF") || \
                types_E[0] == 'C'      || types_E[1] == 'C'    )    \
            {                                                       \
                err = SEM_ARITHM_REL_ERR;                           \
                return false;                                       \
            }                                                       \
        } while(0)

#define CHECK_NUMBER()                                              \
        do {                                                        \
            if (strcmp(types_E, "II") && strcmp(types_E, "IF") &&   \
                strcmp(types_E, "FI") && strcmp(types_E, "FF"))     \
            {                                                       \
                err = SEM_ARITHM_REL_ERR;                           \
                return false;                                       \
            }                                                       \
        } while(0)

#define FIRST_SYMBOL_NO_IN_EXPR                                 \
        token.type != T_L_ROUND_BR && token.type != T_LENGTH && \
        token.type != T_ID         && token.type != T_STRING && \
        token.type != T_FLOAT      && token.type != T_INT    && \
        token.keyword != KW_NIL

#define SYMBOLS_FOR_CONDITION                                             \
        token.type == T_LT || token.type == T_GT || token.type == T_LE || \
        token.type == T_GE || token.type == T_EQ || token.type == T_NEQ

char types_E[2];
htab_item_t *var;

void print_stack_debug(List * list) {
    ElementPtr PrintElement = list->firstElement;
    while (PrintElement != NULL) {
        printf("\t%s\t", PrintElement->data);
        printf("%c\t%d\t%d\tReduced:%d", PrintElement->type, PrintElement->element_token.type, PrintElement->element_token.keyword,
               PrintElement->already_reduced);
        if(PrintElement->element_token.type == T_ID){
            printf("\tvariable: %s\n", PrintElement->element_token.attr.id.str);
        } else if (PrintElement->element_token.type == T_STRING && !PrintElement->already_reduced) {
            printf("\tstring: \"%s\"\n", PrintElement->element_token.attr.id.str);
        } else {
            printf("\n");
        }
        PrintElement = PrintElement->nextElement;
    }

    printf("\n\n");
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
    TempElement->type = 'N';
    TempElement->element_token.type = T_NONE;
    TempElement->already_reduced = 0;

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
        // We previously saved the names of the variables, we need to free them
        if (DelElement->element_token.type == T_ID ||
           (DelElement->element_token.type == T_STRING && !DelElement->already_reduced)) {
            str_free(&DelElement->element_token.attr.id);
        }

        free(DelElement);
        DelElement = NULL;
    }
}

bool Insert(List * list, char * data) {
    bool ret;
    if (list == NULL) {
        return 0;
    }

    int flag = 0;

    ElementPtr find = list->lastElement;
    /* If stack contains for example $E and data contains +,
    * we skip to previous element until we find one of
    * our chars(+, -, <= etc.) and
    * connect the element in a way to create $<<E+,
    * otherwise if there is no E on top of stack,
    * for example $<<E+, we just copy our << with data,
    * $<<E+ -> $<<E+<<(
    * Whether there is E on top of the stack we check with int flag
    */
    while ((strcmp(find->data, "E") == 0) && find->previousElement != NULL) {
        find = find->previousElement;
        flag = 1;
    }

    // We check if malloc was successful
    ElementPtr TempElement_first = malloc(sizeof(struct Element));
    if (TempElement_first == NULL) {
        err = INTERNAL_ERR;
        Deallocate(list);
        return false;
    }
    ElementPtr TempElement_second = malloc(sizeof(struct Element));
    if (TempElement_second == NULL) {
        err = INTERNAL_ERR;
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
    }
    // If we found E on stack, means we are copying
    // << before E and data after E
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
    TempElement_first->type = 'N';
    TempElement_first->element_token.type = T_NONE;
    TempElement_first->already_reduced = 0;
    // If we are dealing not with character,
    // but with an expression we copy the data of token into our structure
    if ((strcmp(data, "i")) == 0) {
        // If we are dealing with variable we also save the name of the variable to be able to look into symtab
        // or we are saving the string of variable of type T_STRING
        if (token.type == T_ID || token.type == T_STRING) {

            if (token.type == T_ID) {
                var = find_id_symtbs(&local_symtbs, token.attr.id.str);
                CHECK_SEM_DEF_ERR(!var);
            }

            ret = str_init(&TempElement_second->element_token.attr.id, 20);
            CHECK_INTERNAL_ERR(!ret, false);

            ret = str_copy_str(&TempElement_second->element_token.attr.id, &token.attr.id);
            CHECK_INTERNAL_ERR(!ret, false);
        }
        // Fill values into element
        ASSIGN_TYPE_FROM_TOKEN();
        TempElement_second->element_token.type = token.type;
        TempElement_second->element_token.keyword = token.keyword;
        TempElement_second->element_token.attr.num_i = token.attr.num_i;
        TempElement_second->element_token.attr.num_f = token.attr.num_f;

        strcpy(TempElement_second->data, data);
        TempElement_second->already_reduced = 0;
    }
    // else we just set type to T_NONE so we can
    // differentiate between an expression and a character
    else {
        TempElement_second->type = 'N';
        TempElement_second->element_token.type = token.type;
        TempElement_second->already_reduced = 0;
    }

    strcpy(TempElement_second->data, data);

    print_stack_expr(list);
    return true;
}

bool Close(List * list) {
    if (list == NULL) {
        return false;
    }
    char Array_To_Check_Against_Rules[5] = {'\0'};

    ElementPtr find = list->lastElement;
    ElementPtr Ei = NULL;
    ElementPtr Ej = NULL;
    ElementPtr operator = NULL;
    CLEAR_TYPES_E();

    // We copy into our array until we dont find closing <<
    while ((strcmp(find->data, "<<") != 0) && find->previousElement != NULL) {
        // We found Expression and the expression isn't in the form of "i" but "E" on stack
        if ((strcmp(find->data, "E")) == 0) {
            if (Ej == NULL)
                Ej = find;
            else
                Ei = find;
        }
        else if ((strcmp(find->data, "i")) == 0) {
            Ej = find;
        } else {
            operator = find;
        }

        strcat(Array_To_Check_Against_Rules, find->data);
        find = find->previousElement;
    }

    if (Ej != NULL) {
        // Special case with rule #E or <i> where there is only one expression
        // so Ei will stay NULL, we make sure Ei isn't NULL
        if (Ei == NULL) {
            Ei = Ej;
        }

        if (Ei->element_token.type == T_ID) {
            GET_TYPE_ID(0, Ei->element_token.attr.id.str);
            // Change type from X (ID) into float / int / string
            Ei->type = var->data.var->type.str[0];

        }
        else {
            GET_TYPE_TERM(0);
        }
        if (Ej->element_token.type == T_ID) {
            GET_TYPE_ID(1, Ej->element_token.attr.id.str);
            // Change type from X (ID) into float / int / string
            Ej->type = var->data.var->type.str[0];
        }
        else {
            GET_TYPE_TERM(1);
        }
    }

    // We check against rules
    for (int rule = 0; rule < 15; rule++) {
        // If we found correct rule
        if (strcmp(Array_To_Check_Against_Rules, Rules[rule]) == 0) {
            // Assigns type, if Comparing type == compare, if II type == int, SS = string else = float
            ASSIGN_TYPE();
            // In this if statement we make sure that if rules #E = 7 and E..E = 14 were
            // found the expressions are also strings
            if (rule == 7 || rule == 14) {
                CHECK_CONC_LENGTH();
            }
            // If we are doing comparison rules
            else if (rule >= 8 && rule <= 13) {
                // Special case where we only allow value nil with comparison == and ~=
                if(rule != 12 && rule != 13 && (Ei->type == 'N' || Ej->type == 'N')){
                    err = SEM_ARITHM_REL_ERR;
                    return false;
                }
                CHECK_COMPARISON();
            }
            // If we found any other rule other than
            // i = 0, (E) = 1, #E, E..E or comparisons
            else if (rule != 0 && rule != 1){
                CHECK_NUMBER();
                // Special case where we are trying to divide with rule
                // for integers and the values are not integers
                if (rule == 6 && strcmp(types_E, "II")){
                    err = SEM_ARITHM_REL_ERR;
                    return false;
                }
            }

            // We add our variable or operator to queue
            if (!Add_Tokens_To_Queue(Ei, operator, rule)) {
                return false;
            }

            // if we are reducing <<i>> or (E) we also need to copy every single information
            // from token to be able to pass it into gen code
            if (rule == 0 || rule == 1) {
                find->element_token.type = Ei->element_token.type;
                find->element_token.keyword = Ei->element_token.keyword;
                find->element_token.attr.num_i = Ei->element_token.attr.num_i;
                find->element_token.attr.num_f = Ei->element_token.attr.num_f;
                find->already_reduced = Ei->already_reduced;
                if ((Ei->element_token.type == T_ID || Ei->element_token.type == T_STRING) && !Ei->already_reduced) {
                    bool ret;
                    ret = str_init(&find->element_token.attr.id, 20);
                    CHECK_INTERNAL_ERR(!ret, false);

                    ret = str_copy_str(&find->element_token.attr.id, &Ei->element_token.attr.id);
                    CHECK_INTERNAL_ERR(!ret, false);
                }
            } else {
                find->already_reduced = 1;
            }

            // We change << with E
            strcpy(find->data, "E");
            // We delete everything after <<
            Dispose(find->nextElement);
            find->nextElement = NULL;
            // The resulting expression is combination of 2 other expressions
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

    // Copy onto the stack and we also copy T_NONE into token
    TempElement->type = 'N';
    TempElement->element_token.type = token.type;
    strcpy(TempElement->data, data);
    return true;
}

bool Check_Correct_Closure(List * list) {
    if (list != NULL) {
        // We check if first element on stack s $ and the second one E
        if ((strcmp(list->firstElement->data, "$") == 0)    &&
                    list->firstElement->nextElement != NULL &&
            (strcmp(list->firstElement->nextElement->data, "E") == 0))
        {
            return true;
        }
    }

    return false;
}

bool Add_Tokens_To_Queue(ElementPtr Ei, ElementPtr operator, int rule) {
    token_t * Token_Ei = NULL;
    token_t * Token_Operator = NULL;
    // We are copying operand
    if (rule == 0) {
        if ((Token_Ei = Copy_Values_From_Token(Token_Ei, &Ei->element_token)) == NULL) {
            return false;
        }
        queue_add_token_rear(queue_expr, Token_Ei);
    // We are copying operator excluding rule (E)
    } else if (rule != 1) {
        if ((Token_Operator = Copy_Values_From_Token(Token_Operator, &operator->element_token)) == NULL) {
            return false;
        }
        queue_add_token_rear(queue_expr, Token_Operator);
    }

    return true;
}

token_t * Copy_Values_From_Token(token_t * to, token_t * from) {
    to = malloc(sizeof(token_t));
    CHECK_INTERNAL_ERR(!to, NULL);

    to->type = from->type;
    to->keyword = from->keyword;
    to->attr.num_i = from->attr.num_i;
    to->attr.num_f = from->attr.num_f;

    if (from->type == T_ID || from->type == T_STRING) {
        bool ret;
        ret = str_init(&to->attr.id, 20);
        CHECK_INTERNAL_ERR(!ret, NULL);

        ret = str_copy_str(&to->attr.id, &(from->attr.id));
        CHECK_INTERNAL_ERR(!ret, NULL);
    }

    return to;
}

void Deallocate(List * list) {
    if (list != NULL) {
        Dispose(list->firstElement);
        free(list);
        list = NULL;
    }
}

bool expr(bool bool_condition, bool bool_empty) {
    (void)bool_condition;
    bool ret;
    char data[3] = {"$"};
    List * list = NULL;
    list = Init(list);

    char precedence;

    /*
     * If expression is called, but is empty, check 2 cases:
     * 1. Expression can be empty, no error (assigning, argument, etc.)
     * 2. Expression cant be empty, syntax error (if, while)
     */
    if (FIRST_SYMBOL_NO_IN_EXPR) {
        Deallocate(list);
        return bool_empty == true ? true : false;
    }

    while ((TOKEN_ID_EXPRESSION()) && (list != NULL)) {
        /*
         * If expression get at least one from these symbols:
         * <, >, <=, >=, ==, ~=, check 2 cases:
         * 1. Expression can contain this symbol, no error (if, while)
         * 2. Expression cant contain this symbol, the 6th error (assigning, ret_val, etc.)
         */
        if (!bool_condition) {
            if (SYMBOLS_FOR_CONDITION) {
                err = SEM_TYPE_COMPAT_ERR;
                return false;
            }
        }

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
            // If identificator, copy i onto stack
            if (GET_ID(token.attr.id.str) == INDEX_OF_IDENTIFICATOR) {
                strcpy(data, "i");
            }
            // else copy operator
            else {
                strcpy(data, token.attr.id.str);
            }

            // We copy the character from the token with << added
            // infront of E $<<E+ or $<<E+<<( (+, -, <= etc.)
            if(!Insert(list, data)){
                list = NULL;
                goto err_expr;
            }
        }
        else if (precedence == '>') {
            // If we didnt find a rule we return false as expression is wrong
            if (!Close(list)) {
                goto err_expr;
            }

            print_stack_expr(list);
            // If we found a rule, we load next token
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
            /*
             * Special incident with finding identificator after identificator,
             * which means the expression has ended and we close our stack
             * and check against the rules
             */
            goto end_expr;
        }
        else if (precedence == 'c') {
            // If we found something that doesnt have correct order, for example )(
            goto err_expr;
        }
        NEXT_TOKEN();
    }

    // If there is internal error such as failure to allocate,
    // list will free itself so if list is empty there was an error along the way
    CHECK_INTERNAL_ERR(!list, false);

    print_dbg_msg_single("reduction:\n");
    print_stack_expr(list);

end_expr:
    // We are reducing the expression by using our rules until there is only E left
    while (Close(list)) {
        print_stack_expr(list);
    }

    // If we were successful in reducing the expression and there wasn't any error
    // we save the resulting type
    if (Check_Correct_Closure(list) && err == NO_ERR) {
        ret = str_add_char(&tps_right, list->lastElement->type);
        CHECK_INTERNAL_ERR(!ret, false);
        Deallocate(list);

        return true;
    }

err_expr:
    Deallocate(list);
    return false;
}
