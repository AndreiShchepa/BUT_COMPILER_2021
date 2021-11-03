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

int main() {

#ifdef DEBUG_SCANNER
    FILE *f = stdin;
    set_source_file(f);

    token_t token;
    str_init(&token.attr.id, 20);
    int err;

    do {
        err = get_next_token(&token);
        if (err == SCANNER_ERR) {
            fprintf(stderr, "\nError in scanner, wrong token %s\n", token.attr.id.str);
            return 1;
        }
    } while (token.type != T_EOF);

    str_free(&token.attr.id);
    fprintf(stderr, "\nNo error in scanner\n");
#else
    int ret;

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
        else {
            fprintf(stderr, "\nParser err\n");
        }
    }
#endif

	return 0;
}
