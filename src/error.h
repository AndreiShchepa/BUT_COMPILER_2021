/**
 * Project: Compiler IFJ21
 *
 * @file error.h
 *
 * @author Andrei Shchapaniak <xshcha00>
 */

#ifndef _ERROR_H
#define _ERROR_H

#define NO_ERR 0
#define SCANNER_ERR 1
#define PARSER_ERR 2
#define SEM_DEF_ERR 3
#define SEM_TYPE_COMPAT_ERR 4
#define SEM_FUNC_ERR 5
#define SEM_ARITHM_REL_ERR 6
#define SEM_OTHER_ERR 7
#define NIL_ERR 8
#define DIV_0_ERR 9
#define INTERNAL_ERR 99

#endif // _ERROR_H
