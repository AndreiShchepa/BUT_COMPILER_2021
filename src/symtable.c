/**
 * Project: Compiler IFJ21
 *
 * @file symtab.c
 *
 * @brief Implement functions representing a symbol table using hash table
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

bool symtab_init(htab_t *table) {
	table = calloc(1, sizeof(htab_t *));
	if (!table) {
		return false;
	}

	table->num_of_el = 0;
	for (int i = 0; i < SYMTAB_SIZE; i++) {
		table->arr[i] = NULL;
	}

	return true;
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

htab_item_t *symtab_add(htab_t *table) {
	(void)table;
	// must be completed

    return NULL;
}

void symtab_free(htab_t *table) {
	(void)table;
	// must be completed
}
