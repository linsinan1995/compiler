//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_ENUM_H
#define COMPILER_ENUM_H

#include "Kind.def"

#define X(entry) entry
typedef enum { ENUM_DATA_TYPE_KIND_ENTRY }  Kind;
typedef enum { ENUM_STATEMENT_KIND_ENTRY }  Kind_statement;
typedef enum { ENUM_EXPRESSION_KIND_ENTRY } Kind_expression;
#undef X

#define X(entry) #entry
static const char* const names_kind[]      = { ENUM_DATA_TYPE_KIND_ENTRY };
static const char* const names_stmt_kind[] = { ENUM_STATEMENT_KIND_ENTRY };
static const char* const names_exp_kind[]  = { ENUM_EXPRESSION_KIND_ENTRY };
#undef X


#endif //COMPILER_ENUM_H
