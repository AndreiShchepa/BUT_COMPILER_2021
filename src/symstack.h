/**
 * Project: Compiler IFJ21
 *
 * @file symstack.h
 *
 * @brief Contain functions declaration and data types representing
 *        a stack of symtables.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */

#ifndef _SYMSTACK_H
#define _SYMSTACK_H

#include <stdbool.h>
#include <stdint.h>
#include "scanner.h"
#include "str.h"

void free_symtbs(arr_symtbs_t *symtbs);

/*
 * @brief Deallocate and delete last symtable
 *        from array of local symtables
 */
void delete_last_symtab(arr_symtbs_t *symtbs);

/*
 * @brief Allocate and add new local symtable
 * @return On success true, otherwise false
 */
bool add_symtab(arr_symtbs_t *symtbs);

/*
 * @brief Search ID in last and pervious local tables
 * @param symtbs - pointer ro the array of local symtables
 * @param key - ID for searching
 * @return On success pointer to item, otherwise NULL
 */
htab_item_t *find_id_symtbs(arr_symtbs_t *symtbs, const char *key);
#endif // _SYMSTACK_H
