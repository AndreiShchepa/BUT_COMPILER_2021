/**
 * Project: Compiler IFJ21
 *
 * @file compiler.c
 * @author Andrei Shchapaniak <xshcha00>
 *
 */

#include <stdio.h>
#include "parser.h"
#include "error.h"
#include "str.h"

int main() {
    int ret;

    ret = parser();

    if (ret == NO_ERR) {
        printf("All good\n");
    }
    else {
        if (ret == INTERNAL_ERR) {
            printf("Internal err\n");
        }
        else if (ret == SCANNER_ERR) {
            printf("Scanner err\n");
        }
        else {
            printf("Parser err\n");
        }
    }
	return 0;
}
