#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include "str.h"
#include "error.h"
#include "scaner.h"

#define PUSH_CHAR() ret = str_add_char(&token->attr.id, (char)ch); \
                    if (!ret) { \
                        return INTERNAL_ERR; \
                    }

#define ACCEPT_LEXEM() ungetc(ch, f); \
                       flag = true

int ch;
states_t state;
bool ret, flag;

int scan_id(FILE* f, token_t *token) {
    state = START;
    flag = false;

    while (!flag) {
        ch = fgetc(f);

        switch (state) {
            case START:
                switch (ch) {
                    case '_': LETTERS_CASE:
                        state = I1;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case I1:
                switch (ch) {
                    case '_': LETTERS_CASE: case '0': DIGITS_CASE:
                        state = I1;
                        PUSH_CHAR();
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            default:
                return SCANER_ERR;
        }
    }

    token->type = T_ID;

    return NO_ERR;
}

int scan_number(FILE *f, token_t *token) {
    state = START;
    flag = false;
    bool float_num = false;

    while (!flag) {
        ch = fgetc(f);

        switch (state) {
            case START:
                switch (ch) {
                    case '0': DIGITS_CASE:
                        state = N1;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case N1:
                switch (ch) {
                    case '0': DIGITS_CASE:
                        state = N1;
                        PUSH_CHAR();
                        break;
                    case '.':
                        state = N3;
                        float_num = true;
                        PUSH_CHAR();
                        break;
                    case 'E': case 'e':
                        state = N2;
                        PUSH_CHAR();
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            case N2:
                switch (ch) {
                    case '-': case '+':
                        state = N5;
                        PUSH_CHAR();
                        break;
                    case '0': DIGITS_CASE:
                        state = N6;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case N3:
                switch (ch) {
                    case '0': DIGITS_CASE:
                        state = N4;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case N4:
                switch (ch) {
                    case '0': DIGITS_CASE:
                        state = N4;
                        PUSH_CHAR();
                        break;
                    case 'E': case 'e':
                        state = N2;
                        PUSH_CHAR();
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            case N5:
                switch (ch) {
                    case '0': DIGITS_CASE:
                        state = N6;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case N6:
                switch (ch) {
                    case '0': DIGITS_CASE:
                        state = N6;
                        PUSH_CHAR();
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            default:
                return SCANER_ERR;
        }
    }

    token->type = float_num ? T_FLOAT : T_INT;

    return NO_ERR;
}

int scan_string(FILE *f, token_t *token) {
    state = START;
    flag = false;

    while (!flag) {
        ch = fgetc(f);

        switch (state) {
            case START:
                switch (ch) {
                    case '\"':
                        state = S1;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case S1:
                switch (ch) {
                    case '\"':
                        ACCEPT_LEXEM();
                        break;
                    case '\\':
                        state = S2;
                        PUSH_CHAR();
                        break;
                    default:
                        state = S1;
                        PUSH_CHAR();
                        break;
                }

                break;
            case S2:
                switch (ch) {
                    case 'n': case 't': case '\"': case '\\':
                        state = S1;
                        PUSH_CHAR();
                        break;
                    case '0':
                        state = S3;
                        PUSH_CHAR();
                        break;
                    case '1':
                        state = S4;
                        PUSH_CHAR();
                        break;
                    case '2':
                        state = S5;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case S3:
                switch (ch) {
                    case '0':
                        state = S6;
                        PUSH_CHAR();
                        break;
                    DIGITS_CASE:
                        state = S7;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case S4:
                switch (ch) {
                    case '0': DIGITS_CASE:
                        state = S7;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case S5:
                switch (ch) {
                    case '0': case '1': case '2': case '3': case '4':
                        state = S7;
                        PUSH_CHAR();
                        break;
                    case '5':
                        state = S8;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case S6:
                switch (ch) {
                    DIGITS_CASE:
                        state = S1;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case S7:
                switch (ch) {
                    case '0': DIGITS_CASE:
                        state = S1;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case S8:
                switch (ch) {
                    case '0': case '1': case '2': case '3': case '4': case '5':
                        state = S1;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            default:
                return SCANER_ERR;
        }
    }

    token->type = T_STRING;

    return NO_ERR;
}

int scan_comment_or_sub(FILE *f, token_t *token) {
    state = START;
    flag = false;

    while (!flag) {
        ch = fgetc(f);

        switch (state) {
            case START:
                switch (ch) {
                    case '-':
                        state = C1;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case C1:
                switch (ch) {
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
                    case '[':
                        state = C3;
                        break;
                    case '\n':
                        str_clear(&token->attr.id);
                        return NO_ERR;
                    default:
                        state = C2;
                        break;
                }

                break;
            case C3:
                switch (ch) {
                    case '[':
                        state = C4;
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case C4:
                switch (ch) {
                    case ']':
                        state = C5;
                        break;
                    default:
                        state = C4;
                        break;
                }

                break;
            case C5:
                switch (ch) {
                    case ']':
                        state = C6;
                        return NO_ERR;
                    default:
                        return SCANER_ERR;

                }

                break;
            default:
                return SCANER_ERR;
        }
    }

    token->type = T_MINUS;

    return NO_ERR;
}

int scan_relate_op(FILE *f, token_t *token) {
    state = START;
    flag = false;

    while (!flag) {
        ch = fgetc(f);

        switch (state) {
            case START:
                switch (ch) {
                    case '<':
                        state = R1;
                        PUSH_CHAR();
                        token->type = T_LT;
                        break;
                    case '>':
                        state = R3;
                        PUSH_CHAR();
                        token->type = T_GT;
                        break;
                    case '=':
                        state = R5;
                        PUSH_CHAR();
                        break;
                    case '~':
                        state = R7;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case R1:
                switch (ch) {
                    case '=':
                        state = R2;
                        PUSH_CHAR();
                        token->type = T_LE;
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            case R2:
                ACCEPT_LEXEM();

                break;
            case R3:
                switch (ch) {
                    case '=':
                        state = R4;
                        PUSH_CHAR();
                        token->type = T_GE;
                        break;
                    default:
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            case R4:
                ACCEPT_LEXEM();

                break;
            case R5:
                switch (ch) {
                    case '=':
                        state = R6;
                        PUSH_CHAR();
                        token->type = T_EQ;
                        break;
                    default:
                        token->type = T_ASSIGN;
                        ACCEPT_LEXEM();
                        break;
                }

                break;
            case R6:
                ACCEPT_LEXEM();

                break;
            case R7:
                switch (ch) {
                    case '=':
                        state = R8;
                        PUSH_CHAR();
                        token->type = T_NEQ;
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case R8:
                ACCEPT_LEXEM();

                break;
            default:
                return SCANER_ERR;
        }
    }

    return NO_ERR;
}

int scan_concat(FILE *f, token_t *token) {
    state = START;
    flag = false;

    while (!flag) {
        ch = fgetc(f);

        switch(START) {
            case START:
                switch (ch) {
                    case '.':
                        state = D1;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case D1:
                switch (ch) {
                    case '.':
                        state = D2;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case D2:
                ACCEPT_LEXEM();

                break;
            default:
                return SCANER_ERR;
            }
    }

    token->type = T_CONCAT;

    return NO_ERR;
}

int scan_div(FILE *f, token_t *token) {
    state = START;
    flag = false;
    bool div_int = false;

    while (!flag) {
        ch = fgetc(f);

        switch(START) {
            case START:
                switch (ch) {
                    case '/':
                        state = B1;
                        PUSH_CHAR();
                        break;
                    default:
                        return SCANER_ERR;
                }

                break;
            case B1:
                switch (ch) {
                    case '/':
                        state = B2;
                        PUSH_CHAR();
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
                return SCANER_ERR;
            }
    }

    token->type = div_int ? T_DIV_INT : T_DIV;

    return NO_ERR;
}

int scan_other_lexem(FILE *f, token_t *token) {
    ch = fgetc(f);

    switch (ch) {
        case '\n':
            token->type = T_EOL;
            break;
        case '+':
            token->type = T_PLUS;
            break;
        case '*':
            token->type = T_MUL;
            break;
        case '-':
            token->type = T_MINUS;
            break;
        case '(':
            token->type = T_L_ROUND_BR;
            break;
        case ')':
            token->type = T_R_ROUND_BR;
            break;
        case '{':
            token->type = T_L_CURL_BR;
            break;
        case '}':
            token->type = T_R_CURL_BR;
            break;
        case ',':
            token->type = T_COMMA;
            break;
        case ';':
            token->type = T_SEMICOLON;
            break;
        case '#':
            token->type = T_LENGTH;
            break;
        case EOF:
            token->type = T_EOF;
            break;
        default:
            return SCANER_ERR;
        }

    return NO_ERR;
}

int get_next_token(token_t *token) {
    int err;
    FILE *f = stdin;
    str_clear(&token->attr.id);

skip_whitespace:
    ch = fgetc(f);

    switch (ch) {
        case '\t': case ' ':
            goto skip_whitespace;
        case '_': LETTERS_CASE:
            ungetc(ch, f);
            err = scan_id(f, token);
            break;
        case '0': DIGITS_CASE:
            ungetc(ch, f);
            err = scan_number(f, token);
            break;
        case '>': case '<': case '=': case '~':
            ungetc(ch, f);
            err = scan_relate_op(f, token);
            break;
        case '\"':
            ungetc(ch, f);
            err = scan_string(f, token);
            break;
        case '-':
            ungetc(ch, f);
            err = scan_comment_or_sub(f, token);
            break;
        case '.':
            ungetc(ch, f);
            err = scan_concat(f, token);
            break;
        case '/':
            ungetc(ch, f);
            err = scan_div(f, token);
            break;
        default:
            ungetc(ch, f);
            err = scan_other_lexem(f, token);
            break;
    }

    return err;
}
