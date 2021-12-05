/**
 * Project: Compiler IFJ21
 *
 * @file compiler.c
 * @author Andrei Shchapaniak <xshcha00>
 *
 */

#ifdef DEBUG_SCANNER
    #include "scanner.h"
#else
    #include "parser.h"
#endif

#include <stdio.h>
#include "error.h"
#include "str.h"
#include "code_generator.h"

extern string_t ifj_code[BLOCKS_NUM];

int main() {


int ret;

#ifdef DEBUG_SCANNER
    FILE *f = stdin;
    set_source_file(f);

    token_t token;
    str_init(&token.attr.id, 20);

    do {
        ret = get_next_token(&token);
        if (ret == SCANNER_ERR) {
            fprintf(stderr, "\nError in scanner, wrong token %s\n", token.attr.id.str);
            return ret;
        }
    } while (token.type != T_EOF);

    str_free(&token.attr.id);
    fprintf(stderr, "\nNo error in scanner\n");
#else
    ret = parser();

    if (ret == NO_ERR) {
        fprintf(stderr, "\nNo error in parser\n");
    }
    else {
        if (ret == INTERNAL_ERR) {
            fprintf(stderr, "\nInternal err\n");
        }
        else if (ret == SCANNER_ERR) {
            fprintf(stderr, "\nScanner err\n");
        }
        else if (ret == SEM_DEF_ERR) {
            fprintf(stderr, "\nSemantic err: undefined function/variable, "
                    "attempt to redefinition of variables, etc.\n");
        }
        else if (ret == SEM_FUNC_ERR) {
            fprintf(stderr, "\nSemantic err: wrong number/type "
                    "of parameters or return values when calling a function "
                    "or return from a function\n");
        }
        else if (ret == SEM_TYPE_COMPAT_ERR) {
            fprintf(stderr, "\nSemantic err: type incompatibility in assignment statement\n");
        }
        else if (ret == PARSER_ERR) {
            fprintf(stderr, "\nParser err\n");
        }
        else if (ret == SEM_OTHER_ERR) {
            fprintf(stderr, "\nSemantic err: another semantic error"
                    " (probably in multiply assigning)\n");
        }
        else if (ret == SEM_ARITHM_REL_ERR) {
            fprintf(stderr, "\nSematic err: type incompatibility in expressions\n");
        }
        else {
            fprintf(stderr, "\nAnother err\n");
        }
    }
#endif

#if !defined(DEBUG_EXPR) && !defined(DEBUG_RULES)
if (ret == NO_ERR) {
    gen_testing_helper();
}
#endif

    dealloc_gen_var();
	return ret;
}
