/**
 * Project: Compiler IFJ21
 *
 * @file scanner.h
 *
 * @brief Contain functions declaration and data types representing
 *        tokens, DFA, states, keywords.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */

#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>
#include <stdint.h>
#include "str.h"
#define COUNT_KEYWORDS 15

#define LETTERS_CASE case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': \
                     case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': \
                     case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': case 'a': \
                     case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': \
                     case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': \
                     case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z'

#define DIGITS_CASE case '1': case '2': case '3': \
                    case '4': case '5': case '6': \
                    case '7': case '8': case '9'

#define HEX_CASE case '0': case '1': case '2': case '3': \
                 case '4': case '5': case '6': case '7': \
                 case '8': case '9': case 'a': case 'b': \
                 case 'c': case 'd': case 'e': case 'f': \
                 case 'A': case 'B': case 'C': case 'D': \
                 case 'E': case 'F'

#define NEW_LINE case 10: case 13

typedef enum states {
    START,                  // state  for starting
    I1,                     // state  for identifier scanning
    N1, N2, N3, N4, N5, N6, // states for number scanning

    S1, S2, S3, S4, S5, S6, // states for string scanning
      S7, S8, S9, S10, S11,

    C1, C2, C3, C4, C5, C6, // states for comments scanning
                        C7,
    D1, D2,                 // states for concatinating scanning
    B1, B2,                 // states for division scannnig
    R1, R2, R3, R4, R5, R6, // states for relations operators scanning
                    R7, R8
} states_t;

typedef enum keywords {
    KW_DO,
    KW_GLOBAL,
    KW_NUMBER,
    KW_ELSE,
    KW_IF,
    KW_REQUIRE,
    KW_END,
    KW_INTEGER,
    KW_RETURN,
    KW_FUNCTION,
    KW_LOCAL,
    KW_STRING,
    KW_NIL,
    KW_THEN,
    KW_WHILE,
    KW_NONE // init value
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
    T_L_ROUND_BR,
    T_R_ROUND_BR,
    T_STRING,
    T_COMMA,
    T_COLON,
    T_CONCAT,
    T_NONE // init value
} token_type_t;

typedef union token_attr {
    string_t id;
    uint64_t num_i;
    double num_f;
} token_attr_t;

typedef struct token {
    token_type_t type;
    keywords_t keyword;
    token_attr_t attr;
} token_t;

void set_source_file(FILE *f);

/*
 * @brief recognize if identifier is one of the keywords
 */
void recognize_keyword(token_t *token);

/*
 * @brief handle tokens that start with {_, a-z, A-Z}
 */
int scan_id(token_t *token);

/*
 * @brief handle tokens that start with {0-9}
 */
int scan_number(token_t *token);

/*
 * @brief handle tokens that start with {"}
 */
int scan_string(token_t *token);

/*
 * @brief handle tokens that start with {-}
 */
int scan_comment_or_sub(token_t *token);

/*
 * @brief handle tokens that start with {<, >, ~, =}
 */
int scan_relate_op(token_t *token);

/*
 * @brief handle token that start with {.}
 */
int scan_concat(token_t *token);

/*
 * @brief handle tokens that start with {/}
 */
int scan_div(token_t *token);

/*
 * @brief handle another tokens that contain only one symbol
 */
int scan_other_lexem(token_t *token);

/*
 * @brief main function of scanner, which scans tokens
 *
 * @return On success NO_ERR, otherwise SCANNER_ERR
 */
int get_next_token(token_t *token);

#endif // _SCANNER_H
