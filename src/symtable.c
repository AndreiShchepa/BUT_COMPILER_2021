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

bool find_id_symtbs(arr_symtbs_t *symtbs, const char *key) {
    for (int i = symtbs->size - 1; i >= 0; i--) {
        if (symtab_find(&symtbs->htab[i], key)) {
            return true;
        }
    }

    return false;
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

bool symtab_add_params(htable_t *table, token_t *token, bool value) {
    (void)table;
    (void)token;
    (void)value;
    return true;
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
            free(item_free->key_id);
            free(item_free);
        }

        (*table)[i] = NULL;
    }
}
