/**
 * Project: Compiler IFJ21
 *
 * @file symstack.c
 *
 * @brief Implement functions representing for working with
 * local and global symtable.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "symtable.h"
#include "error.h"
#include "symstack.h"

extern int err;

void free_symtbs(arr_symtbs_t *symtbs) {
    int const_size = symtbs->size;

    for (int i = 0; i < const_size; i++) {
        delete_last_symtab(symtbs);
    }

    free(symtbs->htab);
}

void delete_last_symtab(arr_symtbs_t* symtbs) {
    int index = symtbs->size - 1;

    symtab_free(&symtbs->htab[index]);
    (symtbs->size)--;
}

bool add_symtab(arr_symtbs_t *symtbs) {
    int const_size = symtbs->size;

    htable_t *tmp = NULL;
    tmp = realloc(symtbs->htab, (const_size + 1) * sizeof(htable_t));
    if (!tmp) {
        err = INTERNAL_ERR;
        return false;
    }

    symtbs->htab = tmp;
    symtab_init(&symtbs->htab[const_size]);
    (symtbs->size)++;

    return true;
}

htab_item_t *find_id_symtbs(arr_symtbs_t *symtbs, const char *key) {
    htab_item_t *item;
    for (int i = symtbs->size - 1; i >= 0; i--) {
        item = symtab_find(&symtbs->htab[i], key);
        if (item) {
            return item;
        }
    }

    return NULL;
}
