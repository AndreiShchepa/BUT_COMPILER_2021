/**
 * Project: Compiler IFJ21
 *
 * @file symtab.c
 *
 * @brief Implement functions representing a symbol table using hash table.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include "error.h"

extern int err;

uint32_t symtab_hash(const char *id) {
	uint32_t h = 0;
	const unsigned char *p;
	for (p = (const unsigned char*)id; *p != '\0'; p++) {
		h = 65599 * h + *p;
	}

	return h;
}

void symtab_init(htable_t *table) {
    if (!table) {
        return;
    }

    for (int i = 0; i < MAX_HT_SIZE; i++) {
        (*table)[i] = NULL;
    }
}

htab_item_t *symtab_find(const htable_t *table, const char *key) {
    if (!table) {
        err = INTERNAL_ERR;
        return NULL;
    }

	htab_item_t *item = (*table)[symtab_hash(key) % MAX_HT_SIZE];

	while (item) {
		if (!strcmp(item->key_id, key)) {
			return item;
		}

		item = item->next;
	}

	return NULL;
}

htab_item_t *htab_item_init(const string_t *s) {
    htab_item_t *item = calloc(1, sizeof(htab_item_t));

    if (!item) {
        err = INTERNAL_ERR;
        return NULL;
    }

    item->key_id = calloc(s->length + 1, sizeof(char));

    if (!item->key_id) {
        free(item);
        err = INTERNAL_ERR;
        return NULL;
    }

    strcpy(item->key_id, s->str);
    item->next = NULL;

    return item;
}

htab_item_t *symtab_add(htable_t *table, const string_t *s) {
    if (!table) {
        err = INTERNAL_ERR;
        return NULL;
    }

    uint32_t index = symtab_hash(s->str) % MAX_HT_SIZE;
    htab_item_t *item = symtab_find(table, s->str);

    if (item) {
        err = SEM_DEF_ERR;
        return NULL;
    }
    else {
        htab_item_t *new_item = htab_item_init(s);

        if (!new_item) {
            err = INTERNAL_ERR;
            return NULL;
        }

        new_item->next = (*table)[index];
        (*table)[index] = new_item;
        return new_item;
    }
}

void symtab_free(htable_t *table) {
    if (!table) {
        return;
    }

    htab_item_t *item;
    htab_item_t *item_free;

    for (int i = 0; i < MAX_HT_SIZE; i++) {
        item = (*table)[i];

        while (item) {
            item_free = item;
            item = item->next;
            if (item_free->type == FUNC && item_free->data.func) {
                if (item_free->data.func->def == true) {
                    str_free(&item_free->data.func->def_attr.rets);
                    str_free(&item_free->data.func->def_attr.argv);
                }
                if (item_free->data.func->decl == true) {
                    str_free(&item_free->data.func->decl_attr.rets);
                    str_free(&item_free->data.func->decl_attr.argv);
                }
                free(item_free->data.func);
            }
            else if (item_free->type == VAR && item_free->data.var) {
                str_free(&item_free->data.var->type);
                free(item_free->data.var);
            }

            free(item_free->key_id);
            free(item_free);
        }

        (*table)[i] = NULL;
    }
}

