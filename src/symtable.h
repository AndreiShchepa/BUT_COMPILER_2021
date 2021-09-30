/**
 * Project: Compiler IFJ21
 * @file symtable.h
 *
 * @brief Contain functions declaration and data types representing
 * a symbol table and working with it using hash table
 * @author Andrei Shchapaniak <xshcha00>
 */

#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <stdbool.h>
#include <stdint.h>

#define SYMTAB_SIZE 45503

typedef struct data {
    void *type;
} data_t;

typedef struct htab_item {
    char* key_id;
    data_t data;
    struct htab_item *next;
} htab_item_t;

typedef struct htab {
    int num_of_el;
    htab_item_t *arr[SYMTAB_SIZE];
} htab_t;

/*
 * @brief Sdbm algorithm for hash table
 * @return Hash of the string
 */
uint32_t symtab_hash(const char *id);

/*
 * @brief Allocate hash table and set the structure values
 * @return Initialized and allocated hash table
 */
bool symtab_init(htab_t *table);

/*
 * @brief Find the item int the table that matches the key
 * @param table - symbol table
 * @param key - key for searching item
 * @return On sucess pointer to item, otherwise NULL
 */
htab_item_t *symtab_find(const htab_t *table, const char *key);

/*
 * @brief Create new item in the table
 * @return Pointer to the created item, otherwise NULL
 */
htab_item_t *symtab_add(htab_t *table);

/*
 * @brief Table destructor
 */
void symtab_free(htab_t *table);

#endif // _SYMTABLE_H
