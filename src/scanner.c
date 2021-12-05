/**
 * Project: Compiler IFJ21
 *
 * @file scanner.c
 *
 * @brief Implement functions representing DFA and working with them.
 *        There is also keyword recognizing from token.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "str.h"
#include "error.h"
#include "scanner.h"

#define PUSH_CHAR(sym)                                        \
        do {                                                  \
            ret = str_add_char(&token->attr.id, (char)(sym)); \
            if (!ret) {                                       \
                return INTERNAL_ERR;                          \
            }                                                 \
        } while(0)

#define ACCEPT_LEXEM() \
        ungetc(ch, f); \
        flag = true

#define TRANSLATE_HEX(ch, symbol, n)               \
        do {                                       \
            if ((ch) >= '0' && (ch) <= '9') {      \
                (symbol) += ((n) * ((ch) - 48));   \
            }                                      \
            else if ((ch) >= 'a' && (ch) <= 'f') { \
                (symbol) += ((n) * ((ch) - 87));   \
            }                                      \
            else if ((ch) >= 'A' && (ch) <= 'F') { \
                (symbol) += ((n) * ((ch) - 55));   \
            }                                      \
        } while(0)

#define CHECK_FIRST_0()                 \
        do {                            \
            if (ch == '0' && first_0) { \
                counter_0++;            \
            }                           \
            else {                      \
                first_0 = false;        \
            }                           \
        } while(0)

int ch;
states_t state;
bool ret, flag;
FILE *f;

void set_source_file(FILE *file) {
    f = file;
}

void recognize_keyword(token_t *token) {
        char* keywords[COUNT_KEYWORDS] = {"do", "global", "number", "else",
                                          "if", "require", "end", "integer",
                                          "return", "function", "local",
                                          "string", "nil", "then", "while"};

        for (keywords_t kw = KW_DO; kw < COUNT_KEYWORDS; kw++) {
            if (!str_cmp_const_str(&token->attr.id, keywords[kw])) {
                token->keyword = kw;
                token->type = T_KEYWORD;
                return;
            }
        }

        token->type = T_ID;
}

int scan_id(token_t *token) {
    state = START;
    flag = false;

    while (!flag) {
        ch = fgetc(f);

        switch (state) {
            case START:
                switch (ch) {
                    // START {_, a-z, A-Z} -> I1
                    case '_': LETTERS_CASE:
                        state = I1;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case I1:
                switch (ch) {
                    // I1 {_, a-z, A-Z, 0-9} -> I1
                    case '_': LETTERS_CASE: case '0': DIGITS_CASE:
                        state = I1;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            default:
                return SCANNER_ERR;
        }
    }

    recognize_keyword(token);

    return NO_ERR;
}

int scan_number(token_t *token) {
    state = START;
    flag = false;
    bool float_num = false, first_0 = true;
    int counter_0 = 0;

    while (!flag) {
        ch = fgetc(f);

        switch (state) {
            case START:
                switch (ch) {
                    // START {0-9} -> N1
                    case '0': DIGITS_CASE:
                        CHECK_FIRST_0();
                        state = N1;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case N1:
                switch (ch) {
                    // N1 {0-9} -> N1
                    DIGITS_CASE: case '0':
                        CHECK_FIRST_0();
                        state = N1;
                        PUSH_CHAR(ch);
                        break;
                    // N1 {.} -> N2
                    case '.':
                        state = N2;
                        float_num = true;
                        PUSH_CHAR(ch);
                        break;
                    // N1 {E, e} -> N4
                    case 'e' : case 'E':
                        state = N4;
                        float_num = true;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            case N2:
                switch (ch) {
                    // N2 {0-9} -> N3
                    case '0': DIGITS_CASE:
                        state = N3;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case N3:
                switch (ch) {
                    // N3 {0-9} -> N3
                    case '0': DIGITS_CASE:
                        state = N3;
                        PUSH_CHAR(ch);
                        break;
                    // N3 {E, e} -> N4
                    case 'E': case 'e':
                        float_num = true;
                        state = N4;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            case N4:
                switch (ch) {
                    // N4 {-, +} -> N5
                    case '-': case '+':
                        state = N5;
                        PUSH_CHAR(ch);
                        break;
                    // N2 {0-9} -> N6
                    case '0': DIGITS_CASE:
                        state = N6;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case N5:
                switch (ch) {
                    // N5 {0-9} -> N6
                    case '0': DIGITS_CASE:
                        state = N6;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case N6:
                switch (ch) {
                    // N6 {0-9} -> N6
                    case '0': DIGITS_CASE:
                        state = N6;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            default:
                return SCANNER_ERR;
        }
    }

    if (float_num) {
        token->attr.num_f = strtod(token->attr.id.str, NULL);
        token->type = T_FLOAT;
    }
    else {
        if ((counter_0 == 2 && str_get_len(&token->attr.id) != 2) ||  counter_0 > 2) {
            return SCANNER_ERR;
        }

        token->attr.num_i = strtol(token->attr.id.str, NULL, 10);
        token->type = T_INT;
    }

    return NO_ERR;
}

int scan_string(token_t *token) {
    state = START;
    flag = false;
    uint8_t symbol = 0;

    while (!flag) {
        ch = fgetc(f);

        switch (state) {
            case START:
                switch (ch) {
                    // START {"} -> S1
                    case '\"':
                        state = S1;
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case S1:
                switch (ch) {
                    // S1 {"} -> S9
                    case '\"':
                        state = S9;
                        break;
                    // S1 {\} -> S2
                    case '\\':
                        state = S2;
                        //PUSH_CHAR();
                        break;
                    // S1 {ch >= 32 && ch != \ && ch != "} -> S1
                    default:
                        if (ch >= 32) {
                            state = S1;
                            PUSH_CHAR(ch);
                            break;
                        }

                        return SCANNER_ERR;
                }

                break;
            case S2:
                switch (ch) {
                    // S2 {n, t, \, "} -> S1
                    case 'n':
                        ch = 10;
                        goto state;
                    case 't':
                        ch = 9;
                        goto state;
                    case '\"':
                        ch = 34;
                        goto state;
                    case '\\':
                        ch = 92;
state:
                        state = S1;
                        PUSH_CHAR(ch);
                        break;
                    // S2 {0} -> S3
                    case '0':
                        state = S3;
                        symbol = 0;
                        break;
                    // S2 {1} -> S4
                    case '1':
                        state = S4;
                        symbol = 100;
                        break;
                    // S2 {2} -> S5
                    case '2':
                        state = S5;
                        symbol = 200;
                        break;
                    // S2 {x} -> S10
                    case 'x':
                        state = S10;
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case S3:
                switch (ch) {
                    // S3 {0} -> S6
                    case '0':
                        state = S6;
                        symbol = 0;
                        break;
                    // S3 {1-9} -> S7
                    DIGITS_CASE:
                        state = S7;
                        symbol += (10 * (ch - 48));
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case S4:
                switch (ch) {
                    // S4 {0-9} -> S7
                    case '0': DIGITS_CASE:
                        state = S7;
                        symbol += (10 * (ch - 48));
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case S5:
                switch (ch) {
                    // S5 {0-4} -> S7
                    case '0': case '1': case '2': case '3': case '4':
                        state = S7;
                        symbol += (10 * (ch - 48));
                        break;
                    // S5 {5} -> S8
                    case '5':
                        state = S8;
                        symbol += (10 * (ch - 48));
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case S6:
                switch (ch) {
                    // S6 {1-9} -> S1
                    DIGITS_CASE:
                        state = S1;
                        symbol += (ch - 48);
                        PUSH_CHAR(symbol);
                        symbol = 0;
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case S7:
                switch (ch) {
                    // S7 {0-9} -> S1
                    case '0': DIGITS_CASE:
                        state = S1;
                        symbol += (ch - 48);
                        PUSH_CHAR(symbol);
                        symbol = 0;
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case S8:
                switch (ch) {
                    // S8 {0-5} -> S1
                    case '0': case '1': case '2': case '3': case '4': case '5':
                        state = S1;
                        symbol += (ch - 48);
                        PUSH_CHAR(symbol);
                        symbol = 0;
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case S9:
                ACCEPT_LEXEM();
                break;

            case S10:
                switch (ch) {
                    // S10 {0-9, a-f, A-F} -> S11
                    HEX_CASE:
                        state = S11;
                        TRANSLATE_HEX(ch, symbol, 16);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case S11:
                switch (ch) {
                    // S11 {0-9, a-f, A-F} -> S1
                    HEX_CASE:
                        state = S1;
                        TRANSLATE_HEX(ch, symbol, 1);
                        PUSH_CHAR(symbol);
                        symbol = 0;
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            default:
                return SCANNER_ERR;
        }
    }

    token->type = T_STRING;

    return NO_ERR;
}

int scan_comment_or_sub(token_t *token) {
    state = START;
    flag = false;

    while (!flag) {
        ch = fgetc(f);

        switch (state) {
            case START:
                switch (ch) {
                    // START {-} -> C1
                    case '-':
                        state = C1;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case C1:
                switch (ch) {
                    // C1 {-} -> C2
                    case '-':
                        state = C2;
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            case C2:
                switch (ch) {
                    // C2 {[} -> C4
                    case '[':
                        state = C4;
                        break;
                    // C2 {\n} -> C3
                    NEW_LINE:
                        state = C3;
                        break;
                    // C2 {EOF} -> DEAD
                    case EOF:
                        return SCANNER_ERR;
                    default:
                        state = C7;
                        break;
                }

                break;
            case C3:
                str_clear(&token->attr.id);
                ungetc(ch, f);

                return NO_ERR;
            case C4:
                switch (ch) {
                    // C4 {[} -> C5
                    case '[':
                        state = C5;
                        break;
                    // C4 {\n} -> C3
                    NEW_LINE:
                        state = C3;
                        break;
                    // C4 {EOF} -> DEAD
                    case EOF:
                        return SCANNER_ERR;;
                    default:
                        state = C7;
                        break;
                }

                break;
            case C5:
                switch (ch) {
                    // C5 {]} -> C6
                    case ']':
                        state = C6;
                        break;
                    // C5 {EOF} -> DEAD
                    case EOF:
                        return SCANNER_ERR;
                    default:
                        state = C5;
                        break;
                }

                break;
            case C6:
                switch (ch) {
                    // C6 {]} -> C3
                    case ']':
                        state = C3;
                        break;
                    // C6 {EOF} -> DEAD
                    case EOF:
                        return SCANNER_ERR;
                    default:
                        state = C5;
                        break;
                }

                break;

            case C7:
                switch (ch) {
                    // C7 {\n} -> C3
                    NEW_LINE:
                        state = C3;
                        break;
                    case EOF:
                        return SCANNER_ERR;
                    default:
                        state = C7;
                        break;
                }

                break;
            default:
                return SCANNER_ERR;
        }
    }

    token->type = T_MINUS;

    return NO_ERR;
}

int scan_relate_op(token_t *token) {
    state = START;
    flag = false;

    while (!flag) {
        ch = fgetc(f);

        switch (state) {
            case START:
                switch (ch) {
                    // START {<} -> R1
                    case '<':
                        state = R1;
                        PUSH_CHAR(ch);
                        break;
                    // START {>} -> R3
                    case '>':
                        state = R3;
                        PUSH_CHAR(ch);
                        break;
                    // START {=} -> R5
                    case '=':
                        state = R5;
                        PUSH_CHAR(ch);
                        break;
                    // START {~} -> R7
                    case '~':
                        state = R7;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case R1:
                switch (ch) {
                    // R1 {=} -> R2
                    case '=':
                        state = R2;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        ACCEPT_LEXEM();
                        token->type = T_LT;
                        break;
                }

                break;
            case R2:
                ACCEPT_LEXEM();
                token->type = T_LE;

                break;
            case R3:
                switch (ch) {
                    // R3 {=} -> R4
                    case '=':
                        state = R4;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        ACCEPT_LEXEM();
                        token->type = T_GT;
                        break;
                }

                break;
            case R4:
                ACCEPT_LEXEM();
                token->type = T_GE;

                break;
            case R5:
                switch (ch) {
                    // R5 {=} -> R6
                    case '=':
                        state = R6;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        ACCEPT_LEXEM();
                        token->type = T_ASSIGN;
                        break;
                }

                break;
            case R6:
                ACCEPT_LEXEM();
                token->type = T_EQ;

                break;
            case R7:
                switch (ch) {
                    // R7 {=} -> R8
                    case '=':
                        state = R8;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case R8:
                ACCEPT_LEXEM();
                token->type = T_NEQ;

                break;
            default:
                return SCANNER_ERR;
        }
    }

    return NO_ERR;
}

int scan_concat(token_t *token) {
    state = START;
    flag = false;

    while (!flag) {
        ch = fgetc(f);

        switch(state) {
            case START:
                switch (ch) {
                    // START {.} -> D1
                    case '.':
                        state = D1;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case D1:
                switch (ch) {
                    // D1 {.} -> D2
                    case '.':
                        state = D2;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case D2:
                ACCEPT_LEXEM();

                break;
            default:
                return SCANNER_ERR;
            }
    }

    token->type = T_CONCAT;

    return NO_ERR;
}

int scan_div(token_t *token) {
    state = START;
    flag = false;
    bool div_int = false;

    while (!flag) {
        ch = fgetc(f);

        switch(state) {
            case START:
                switch (ch) {
                    // START {/} -> B1
                    case '/':
                        state = B1;
                        PUSH_CHAR(ch);
                        break;
                    default:
                        return SCANNER_ERR;
                }

                break;
            case B1:
                switch (ch) {
                    // B1 {/} -> B2
                    case '/':
                        state = B2;
                        PUSH_CHAR(ch);
                        div_int = true;
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            case B2:
                ACCEPT_LEXEM();

                break;
            default:
                return SCANNER_ERR;
            }
    }

    token->type = div_int ? T_DIV_INT : T_DIV;

    return NO_ERR;
}

int scan_other_lexem(token_t *token) {
    ch = fgetc(f);

    switch (ch) {
        case '+':
            token->type = T_PLUS;
            break;
        case '*':
            token->type = T_MUL;
            break;
        case '(':
            token->type = T_L_ROUND_BR;
            break;
        case ')':
            token->type = T_R_ROUND_BR;
            break;
        case ',':
            token->type = T_COMMA;
            break;
        case ':':
            token->type = T_COLON;
            break;
        case '#':
            token->type = T_LENGTH;
            break;
        case EOF:
            token->type = T_EOF;
            break;
        default:
            return SCANNER_ERR;
        }

        PUSH_CHAR(ch);

    return NO_ERR;
}

int get_next_token(token_t *token) {
    int err;
    str_clear(&token->attr.id);
    token->keyword = KW_NONE;
    token->type = T_NONE;

skip:
    ch = fgetc(f);

    switch (ch) {
        case '\t': case ' ': NEW_LINE:
            goto skip;
        case '_': LETTERS_CASE:
            ungetc(ch, f);
            err = scan_id(token);
            break;
        case '0': DIGITS_CASE:
            ungetc(ch, f);
            err = scan_number(token);
            break;
        case '>': case '<': case '=': case '~':
            ungetc(ch, f);
            err = scan_relate_op(token);
            break;
        case '\"':
            ungetc(ch, f);
            err = scan_string(token);
            break;
        case '-':
            ungetc(ch, f);
            err = scan_comment_or_sub(token);
            if ((!str_cmp_const_str(&token->attr.id, "")) && err == NO_ERR) {
                goto skip;
            }
            break;
        case '.':
            ungetc(ch, f);
            err = scan_concat(token);
            break;
        case '/':
            ungetc(ch, f);
            err = scan_div(token);
            break;
        default:
            ungetc(ch, f);
            err = scan_other_lexem(token);
            break;
    }

    return err;
}
