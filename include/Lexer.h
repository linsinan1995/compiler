//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <memory>
#include <string>
#include "Token.h"
#include "helper.h"

class Lexer {
    using ptr_token = std::unique_ptr<Token>;
    using ptr_lexer = std::unique_ptr<Lexer>;
    const char *src_code;
public:
    ptr_token next ();
    void load(const char *m_code);

    char peek ();
    char eat  ();
    void eat_white_space ();

    void comment            (Token*);
    void identifier         (Token*);
    void number             (Token*);
    void uni_operator       (Token*);
    void ambigious_operator (Token*);

public:
    explicit Lexer(const char * code) ;
    static ptr_lexer make_lexer(const char *);
};



#endif //COMPILER_LEXER_H
