#ifndef _DEBUG_H
#define _DEBUG_H

#include <assert.h>
#include <stdio.h>

#define debug_msg(...) debug_print(stdout, __VA_ARGS__)
#define debug_msg_stderr(...) debug_print(stderr, __VA_ARGS__)

#define debug_todo(...) do {   \
    fprintf(stderr, "TODO: "); \
    debug_msg(__VA_ARGS__);    \
} while(0)

#define debug_print(_dst, ...) do {                                           \
    fprintf((_dst), "- %s:%d in %s: ", __FILE__, __LINE__ - 1, __FUNCTION__); \
    fprintf((_dst), __VA_ARGS__);                                             \
} while(0)

#define debug_assert(cond) assert((cond))

#endif // _DEBUG_H
