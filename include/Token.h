//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include "Enum.h"

typedef struct {
    size_t       len;
    const char  *content;
} raw_string;

typedef struct {
    raw_string    lexeme;
    Kind          kind;
} Token;

#endif //COMPILER_TOKEN_H
