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

// can be changed, but why...
uint32_t symtab_hash(const char *id) {
	uint32_t h = 0;
	const unsigned char *p;
	for (p = (const unsigned char*)id; *p != '\0'; p++) {
		h = 65599 * h + *p;
	}

	return h;
}

htab_t *symtab_init() {
    htab_t *table = malloc(sizeof(htab_t) + SYMTAB_SIZE * sizeof(htab_item_t*));
	if (!table) {
		return false;
	}

	table->num_of_el = 0;
	for (int i = 0; i < SYMTAB_SIZE; i++) {
        table->arr[i] = NULL;
	}

	return table;
}

htab_item_t *symtab_find(const htab_t *table, const char *key) {
	htab_item_t *item = table->arr[symtab_hash(key) % SYMTAB_SIZE];

	while (item) {
		if (!strcmp(item->key_id, key)) {
			return item;
		}
		item = item->next;
	}

	return NULL;
}

htab_item_t *htab_item_init(const string_t *s) {
    struct htab_item *item = calloc(1, sizeof(htab_item_t));

    if (!item) {
        return NULL; // INTERNAL ERR
    }

    item->key_id = calloc(s->length + 1, sizeof(char));

    if (!item) {
        return NULL; // INTERNAL ERR
    }

    strcpy(item->key_id, s->str);
    item->next = NULL;

    return item;
}

htab_item_t *symtab_add(htab_t *table, const string_t *s) {
    if (!table) {
        return NULL; // INTERNAL ERR
    }

    uint32_t index = symtab_hash(s->str) % SYMTAB_SIZE;
    htab_item_t *item = table->arr[index];

    if (!item) {
        item = htab_item_init(s);

        if (!item) {
            return NULL; // INTERNAL ERR
        }

        (table->num_of_el)++;
        table->arr[index] = item;
        return item;
    }

    return NULL; // semantic err, set err to SEMANTIC_ERR (redefinition)
}

bool symtab_add_params(htab_t *table, token_t *token, bool value) {
    (void)table;
    (void)token;
    (void)value;
    return true;
}

void symtab_free(htab_t *table) {
    htab_item_t *item;
    htab_item_t *item_free;

    for (int i = 0; i < SYMTAB_SIZE; i++) {
        item = table->arr[i];

        while (item != NULL) {
            item_free = item;
            item = item->next;
            free(item_free->key_id);
            free(item_free);
        }
    }
    free(table);
}
