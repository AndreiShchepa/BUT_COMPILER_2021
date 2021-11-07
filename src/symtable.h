/**
 * Project: Compiler IFJ21
 *
 * @file symtable.h
 *
 * @brief Contain functions declaration and data types representing
 *        a symbol table and working with it using hash table.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */

#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <stdbool.h>
#include <stdint.h>
#include "scanner.h"

#define MAX_HT_SIZE 101

typedef struct data {
    bool def_func;
    bool decl_func;
    bool def_var;
} data_t;

typedef struct htab_item {
    char* key_id;
    data_t data;
    struct htab_item *next;
} htab_item_t;

typedef htab_item_t *htable_t[MAX_HT_SIZE];

typedef struct {
    int size;
    htable_t *htab;
} arr_symtbs_t;

/*
 *
 */
bool init_symtbs(arr_symtbs_t *symtbs);

/*
 *
 */
void free_symtbs(arr_symtbs_t *symtbs);

/*
 *
 */
void delete_last_symtab(arr_symtbs_t *symtbs);

/*
 *
 */
bool add_symtab(arr_symtbs_t *symtbs);

/*
 * @brief Sdbm algorithm for hash table
 * @return Hash of the string
 */
uint32_t symtab_hash(const char *id);

/*
 * @brief Allocate hash table and set the structure values
 */
void symtab_init(htable_t *table);

/*
 * @brief Find the item int the table that matches the key
 * @param table - symbol table
 * @param key - key for searching item
 * @return On success pointer to item, otherwise NULL
 */
htab_item_t *symtab_find(const htable_t *table, const char *key);

/*
 * @brief Create new item in the table
 * @return Pointer to the created item, otherwise NULL
 */
htab_item_t *symtab_add(htable_t *table, const string_t *s);

bool symtab_add_params(htable_t *table, token_t *token, bool value);

/*
 * @brief Table destructor
 */
void symtab_free(htable_t *table);

#endif // _SYMTABLE_H
