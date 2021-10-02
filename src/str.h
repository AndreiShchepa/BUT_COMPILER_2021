/**
 * Project: Compiler IFJ21
 *
 * @file str.h
 *
 * @brief Contain functions declaration and data types representing
 *        a dynamic string and working with it.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */

#ifndef _STR_H
#define _STR_H

#include <stdbool.h>

typedef struct string {
	char* str;
	int length;
	int alloc_size;
} string_t;

/*
 * @param s - dynamic string
 * @param init_size - size for string initializing
 * @brief Initialize a dynamic string of size init_size
 * @return On sucsess true, otherwise false
 */
bool str_init(string_t *s, const int init_size);

/*
 * @brief Free dynamic string
 */
void str_free(string_t *s);

/*
 * @brief Clear dynamic string
 */
void str_clear(string_t *s);

/*
 * @brief Add symbol to the end of the dynamic string
 * @return On success true, otherwise false
 */
bool str_add_char(string_t *s, const char c);

/*
 * @return On success true, otherwise false
 */
bool str_copy_str(string_t *s_dst, const string_t *s_src);

/*
 * @param s1 - dynamic string
 * @param s2 - other dynamic string
 * @return if      (s1 is equal to s2)     { 0 }
 *         else if (s1 is less than s2)    { <0 }
 *         else if (s1 is greater than s2) { >0 }
 */
int str_cmp_str(const string_t *s1, const string_t *s2);

/*
 * @param s1 - dynamic string
 * @param s2 - const string
 * @return if      (s1 is equal to s2)     { 0 }
 *         else if (s1 is less than s2)    { <0 }
 *         else if (s1 is greater than s2) { >0 }
 */
int str_cmp_const_str(const string_t *s1, const char *s2);

/*
 * @return Pointer to dynamic string
 */
char *str_get_str(const string_t *s);

/*
 * @return Length of dynamic string
 */
int str_get_len(const string_t *s);

#endif // _STR_H
