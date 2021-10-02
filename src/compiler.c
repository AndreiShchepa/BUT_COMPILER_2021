/**
 * Project: Compiler IFJ21
 *
 * @file compiler.c
 * @author Andrei Shchapaniak <xshcha00>
 *
 */

#include <stdio.h>
#include "scanner.h"
#include "str.h"

int main() {
    FILE *f = stdin;
    set_source_file(f);

    token_t token;
    str_init(&token.attr.id, 20);
    int err;

    do {
        str_clear(&token.attr.id);
        err = get_next_token(&token);
        if (token.type == T_EOL) {
            printf("token = EOL, err_code = %d\n", err);
        }
        else if (token.type == T_EOF) {
            printf("token = EOF, err_code = %d\n", err);
        }
        else {
            printf("token = %s, err code = %d\n", str_get_str(&token.attr.id), err);
        }
    } while (token.type != T_EOF);

    str_free(&token.attr.id);
	return 0;
}
