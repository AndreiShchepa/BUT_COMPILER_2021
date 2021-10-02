#ifndef _SCANER_H
#define _SCANER_H

#include "str.h"
#define LETTERS_CASE case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': \
                     case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': \
                     case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': case 'a': \
                     case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': \
                     case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': \
                     case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z'

#define DIGITS_CASE case '1': case '2': case '3': \
                    case '4': case '5': case '6': \
                    case '7': case '8': case '9'

typedef enum states {
    START,
    I1,
    N1, N2, N3, N4, N5, N6,
    S1, S2, S3, S4, S5, S6, S7, S8, S9,
    C1, C2, C3, C4, C5, C6, C7,
    D1, D2,
    B1, B2,
    R1, R2, R3, R4, R5, R6, R7, R8
} states_t;

typedef enum keywords {
    KW_DO,
    KW_GLOBAL,
    KW_REQUIRE,
    KW_ELSE,
    KW_IF,
    KW_RETURN,
    KW_END,
    KW_LOCAL,
    KW_THEN,
    KW_FUNCTION,
    KW_NIL,
    KW_WHILE
} keywords_t;

typedef enum token_type {
    T_EOF,
    T_ID,
    T_KEYWORD,
    T_INT,
    T_FLOAT,
    T_PLUS,
    T_MINUS,
    T_MUL,
    T_DIV,
    T_DIV_INT,
    T_LT,
    T_GT,
    T_LE,
    T_GE,
    T_EQ,
    T_NEQ,
    T_LENGTH,
    T_ASSIGN,
    T_L_CURL_BR,
    T_R_CURL_BR,
    T_L_ROUND_BR,
    T_R_ROUND_BR,
    T_STRING,
    T_COMMA,
    T_SEMICOLON,
    T_CONCAT,
    T_EOL
} token_type_t;

typedef union token_attr {
    string_t id;
    uint64_t num_i;
    double num_f;
} token_attr_t;

typedef struct token {
    token_type_t type;
    token_attr_t attr;
} token_t;

int scan_id(FILE *f, token_t *token);

int scan_number(FILE *f, token_t *token);

int scan_string(FILE *f, token_t *token);

int scan_comment_or_sub(FILE *f, token_t *token);

int scan_relate_op(FILE *f, token_t *token);

int scan_concat(FILE *f, token_t *token);

int scan_div(FILE *f, token_t *token);

int scan_other_lexem(FILE *f, token_t *token);

#endif // _SCANER_H
