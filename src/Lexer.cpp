/******************************************************************* 
 * 
 *    @Time           @Author       @Description
 * ------------      ---------      -----------
 * 2020-12-16 15:29   Lin Sinan         None
 *
 * @File    :   Lexer.cpp
 * @Contact :   mynameisxiaou@gmail.com
 * @github  :   https://github.com/linsinan1995
 * @usage   :   
 * 
 ******************************************************************/

//
// Created by Lin Sinan on 2020-12-16.
//
// all scalar is float for convenience
#define ALL_FLOAT_POINT
#include "Lexer.h"

inline
Lexer::Lexer(const char * code) : src_code(code)
    {
    }

std::unique_ptr<Token> Lexer::next() {
    auto token = std::make_unique<Token>();

    eat_white_space();
    char ne = peek();

    if (is_comment(ne))                                         comment(token.get());
    else if(is_identifier_prefix(ne))                           identifier(token.get());
    else if(is_ambigious_operator(token.get(), ne))             ambigious_operator(token.get());
    else if(is_digit(ne))                                       number(token.get());
    else if(is_uni_operator(token.get(), ne))                   uni_operator(token.get());
    else if(is_eof(ne))                                         token->kind = k_EOF;
    else                                                        token->kind = k_unexpected;

    return token;
}

inline
char Lexer::peek() {
    return *src_code;
}

inline
char Lexer::eat() {
    return *(++src_code);
}

void Lexer::eat_white_space() {
    char ne = peek();
    while (is_white_space(ne)) {
        ne = eat();
    }
}

void Lexer::comment(Token* token) {
    token->kind = k_comment;

    token->lexeme.content = src_code;
    token->lexeme.len = 0;

    // eat # + ' '
    char ne = eat();

    while (!anyone(ne, '\n', '\0')) {
        ne = eat();
        token->lexeme.len++;
    }
}

void Lexer::identifier(Token* token) {
    token->kind = k_var;

    token->lexeme.content = src_code;
    token->lexeme.len = 1;

    char ne = eat();
    while (is_valid_identifier(ne)) {
        ne = eat();
        token->lexeme.len++;
    }

    check_keyword(token);
}

void Lexer::number(Token* token) {
    token->lexeme.content = src_code;
    token->lexeme.len = 1;
    bool is_fp = false;

    char ne = eat();

    while (is_digit(ne) || (!is_fp && ne == '.')) {
        if (ne == '.') {
            is_fp = true;
            ne = eat();
            token->lexeme.len++;
        }
        ne = eat();
        token->lexeme.len++;
    }
#ifdef ALL_FLOAT_POINT
    token->kind = k_fp;
#else
    token->kind = is_fp ? k_fp : k_int;
#endif
}

inline
void Lexer::uni_operator(Token* token) {
    token->lexeme.content = src_code;
    token->lexeme.len = 1;
    // eat op
    eat();
}

void Lexer::ambigious_operator(Token* token) {
    token->lexeme.content = src_code;
    eat();
    token->lexeme.len = 1;
    if (peek() == '=') {
        switch (token->kind)
        {
            default :
                return;
            case op_assign :
                token->kind = op_equal;
                break;
            case op_gt :
                token->kind = op_ge;
                break;
            case op_lt :
                token->kind = op_le;
                break;
        }

        eat();
        token->lexeme.len++;
    }

    check_keyword(token);
}

std::unique_ptr<Lexer> Lexer::make_lexer(const char *code) {
    return std::make_unique<Lexer>(code);
}

void Lexer::load(const char *m_code) {
    src_code = m_code;
}



