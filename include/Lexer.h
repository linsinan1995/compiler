//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <unordered_set>
#include <memory>
#include <string>
#include "Token.h"
#include "helper.h"

class Lexer {
    using ptr_token = std::unique_ptr<Token>;
    using ptr_lexer = std::unique_ptr<Lexer>;
    const char *src_code;
    std::unordered_set<std::string> registered_class {};

    void string                     (Token*);
    void comment                    (Token*);
    void identifier                 (Token*);
    void number                     (Token*);
    void uni_operator               (Token*);
    void ambigious_operator         (Token*);
    void check_costumized_class_decl(Token*);
    bool is_uni_operator(Token* token, char op);
    bool is_ambigious_operator(Token* token, char c);
    bool is_white_space(char c);
    bool is_identifier_prefix(char c);
    bool is_left_quote(char c);
    bool is_comment(char c);
    void check_keyword(Token *tok);
    bool is_valid_identifier(char c);
    bool is_eof(char c);
    bool is_digit(char c);
public:
    ptr_token next ();
    void load(const char *m_code);
    char peek ();
    char eat  ();
    void eat_white_space ();
    void register_class(Token*);
    bool is_op(Kind k);
public:
    explicit Lexer(const char * code) ;
    static ptr_lexer make_lexer(const char *);
};
#endif //COMPILER_LEXER_H