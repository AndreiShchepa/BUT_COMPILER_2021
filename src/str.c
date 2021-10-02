/**
 * Project: Compiler IFJ21
 *
 * @file str.c
 *
 * @brief Implement functions representing a dynamic string.
 *
 * @author Andrei Shchapaniak <xshcha00>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"

#define STR_LEN_INC 50

bool str_init(string_t *s, const int init_size) {
	s->str = calloc(init_size, sizeof(char));
	if (!s->str) {
		return false;
	}
	s->length = 0;
	s->alloc_size = init_size;

	return true;
}

void str_free(string_t *s) {
	if (s->str) {
		free(s->str);
		s->str = NULL;
	}
}

void str_clear(string_t *s) {
	if (s) {
		memset(s->str, 0, s->length);
		s->length = 0;
	}
}

bool str_add_char(string_t *s, const char c) {
	if (!s || !s->str) {
		return false;
	}

	if (s->length + 1 >= s->alloc_size) {
		char *tmp;
		tmp = realloc(s->str, s->alloc_size + STR_LEN_INC);
		if (!tmp) {
			return false;
		}

		s->str = tmp;
		s->alloc_size += STR_LEN_INC;
	}

	s->str[s->length++] = c;
	s->str[s->length] = '\0';

	return true;
}

bool str_copy_str(string_t *s_dst, const string_t *s_src) {
	if (!s_src || !s_dst || !s_src->str || !s_dst->str) {
		return false;
	}

	if (s_src->length > s_dst->alloc_size) {
		char *tmp;
		tmp = realloc(s_dst->str, s_src->length + 1);
		if (!tmp) {
			return false;
		}

		s_dst->str = tmp;
		s_dst->alloc_size = s_src->alloc_size;
	}

	s_dst->length = s_src->length;
	strcpy(s_dst->str, s_src->str);

	return true;
}

bool str_concat_str(
		string_t *s_dest,
		const string_t *s1,
		const string_t *s2)
{
	if (!s1 || !s2 || !s1->str || !s2->str) {
		return false;
	}

	if (!str_init(s_dest, s1->length + s2->length + 1)) {
		return false;
	}

	s_dest->length = s1->length + s2->length;
	memcpy(s_dest->str, s1->str, s1->length);
	memcpy(s_dest->str + s1->length, s2->str, s2->length + 1);

	return true;
}

int str_cmp_str(const string_t *s1, const string_t *s2) {
	return strcmp(s1->str, s2->str);
}

int str_cmp_const_str(const string_t *s1, const char *s2) {
	return strcmp(s1->str, s2);
}

char *str_get_str(const string_t *s) {
	return s->str;
}

int str_get_len(const string_t *s) {
	return s->length;
}
