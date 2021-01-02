//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include "Enum.h"
#include "String_view.h"

struct Token {
    _string_view  lexeme;
    Kind          kind;
    float to_fp()  { return strtof(lexeme.content, nullptr); }
    int   to_int() { return static_cast<int> (strtol(lexeme.content, nullptr, 10)); }
};


#endif //COMPILER_TOKEN_H
