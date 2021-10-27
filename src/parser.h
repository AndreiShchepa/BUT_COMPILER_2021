#ifndef _PARSER_H
#define _PARSER_H

#include <stdbool.h>

bool prog();

bool prolog();

bool type();

bool statement();

bool work_var();

bool expression();

bool def_var();

bool init_assign();

bool vars();

bool type_expr();

bool other_exp();

bool type_returns();

bool other_types();

bool params();

bool other_params();

bool type_params();

bool args();

bool other_args();

int parser();

#endif // _PARSER_H
