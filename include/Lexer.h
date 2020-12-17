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
    const char *src_code;
public:
    void load(const char *m_code) { src_code = m_code;}
    static std::unique_ptr<Lexer> make_lexer(const char *);
    Lexer(const char * code) ;
    std::unique_ptr<Token> next ();
    char peek ();
    char eat  ();
    void eat_white_space ();

    void comment            (Token*);
    void identifier         (Token*);
    void number             (Token*);
    void uni_operator       (Token*);
    void ambigious_operator (Token*);
};



#endif //COMPILER_LEXER_H
