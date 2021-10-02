/**
 * Project: Compiler IFJ21
 *
 * @file compiler.c
 * @author Andrei Shchapaniak <xshcha00>
 *
 */

#include <stdio.h>
#include "scanner.h"
#include "error.h"
#include "str.h"

int main() {
    FILE *f = stdin;
    set_source_file(f);

    token_t token;
    str_init(&token.attr.id, 20);
    int err;

    do {
        err = get_next_token(&token);
        if (err == SCANNER_ERR) {
            fprintf(stderr, "\nError in scanner, wrong token\n");
        }
    } while (token.type != T_EOF);

    str_free(&token.attr.id);
    fprintf(stderr, "\nNo error in scanner\n");
	return 0;
}
