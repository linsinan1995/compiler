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

inline
Lexer::Lexer()
{
}

std::unique_ptr<Token> Lexer::next() {
    auto token = std::make_unique<Token>();

    eat_white_space();
    char ne = peek();

    if (is_comment(ne))                                         comment(token.get());
    else if(is_left_quote(ne))                                  string(token.get());
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
    check_costumized_class_decl(token);
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
    return code ? std::make_unique<Lexer>(code) : std::make_unique<Lexer> ();
}

void Lexer::load(const char *m_code) {
    src_code = m_code;
}

void Lexer::string(Token *token) {
    // eat left quote
    char ne = eat();

    token->lexeme.content = src_code;
    token->lexeme.len = 0;
    token->kind = k_string;

    while (!anyone(ne, '\"', '\0')) {
        ne = eat();
        token->lexeme.len++;
    }

    // eat right quote
    if (ne == '\"')
        eat();
    else {
        token->kind = k_unexpected;
        // add left quote
        -- (token->lexeme.content);
        ++ (token->lexeme.len);
    }
}

void Lexer::check_costumized_class_decl(Token *tok) {
    if (registered_class.find(tok->lexeme.to_string()) !=
        registered_class.end()) {
        tok->kind = k_class_decl;
    }
}

void Lexer::register_class(Token *tok) {
    registered_class.insert(tok->lexeme.to_string());
}

bool Lexer::is_uni_operator(Token *token, char op) {
    switch (op) {
        case '.': token->kind = k_dot;           return true;
        case '(': token->kind = k_open_paren;    return true;
        case ')': token->kind = k_close_paren;   return true;
        case '+': token->kind = op_add;          return true;
        case '{': token->kind = k_open_curly;    return true;
        case '}': token->kind = k_close_curly;   return true;
        case '/': token->kind = op_div;          return true;
        case '*': token->kind = op_mul;          return true;
        case '%': token->kind = op_mod;          return true;
        case ';': token->kind = k_semi;          return true;
        case '^': token->kind = op_pow;          return true;
        case ',': token->kind = k_comma;         return true;
        case '-': token->kind = op_sub;          return true;
        case '[': token->kind = k_open_bracket;  return true;
        case ']': token->kind = k_close_bracket; return true;
        default: return false;
    }
}

bool Lexer::is_ambigious_operator(Token *token, char c) {
    switch (c) {
        case '>': token->kind = op_gt;     return true;
        case '<': token->kind = op_lt;     return true;
        case '=': token->kind = op_assign; return true;
        default: return false;
    }
}

inline
bool Lexer::is_white_space(char c) {
    return anyone(c, ' ', '\t', '\r', '\n');
}

inline
bool Lexer::is_identifier_prefix(char c) {
    return isalpha(c) || c == '_';
}

void Lexer::check_keyword(Token *tok) {
    if (tok->lexeme.len == 1) {
        if (tok->lexeme.content[0] == '=')
            tok->kind = op_assign;
        else return;
    } else if (tok->lexeme.len == 2) {
        if (tok->lexeme.content[0] == '=' &&
            tok->lexeme.content[1] == '=') tok->kind = op_equal;
        else if (tok->lexeme.content[0] == 'i' &&
                 tok->lexeme.content[1] == 'f') tok->kind = kw_if;
    } else if (tok->lexeme.len == 3) {
        if (tok->lexeme.content[0] == 'v' &&
            tok->lexeme.content[1] == 'a' &&
            tok->lexeme.content[2] == 'r') tok->kind = kw_def;
    } else if (tok->lexeme.len == 4) {
        if (tok->lexeme.content[0] == 'f' &&
            tok->lexeme.content[1] == 'u' &&
            tok->lexeme.content[2] == 'n' &&
            tok->lexeme.content[3] == 'c') tok->kind = kw_func;
        else if (tok->lexeme.content[0] == 'e' &&
                 tok->lexeme.content[1] == 'l' &&
                 tok->lexeme.content[2] == 's' &&
                 tok->lexeme.content[3] == 'e') tok->kind = kw_else;
    } else if (tok->lexeme.len == 5) {
        if (tok->lexeme.content[0] == 'w' &&
            tok->lexeme.content[1] == 'h' &&
            tok->lexeme.content[2] == 'i' &&
            tok->lexeme.content[3] == 'l' &&
            tok->lexeme.content[4] == 'e') tok->kind = kw_while;
        else if (tok->lexeme.content[0] == 'c' &&
                 tok->lexeme.content[1] == 'l' &&
                 tok->lexeme.content[2] == 'a' &&
                 tok->lexeme.content[3] == 's' &&
                 tok->lexeme.content[4] == 's') tok->kind = kw_class;
    } else if (tok->lexeme.len == 6) {
        if (tok->lexeme.content[0] == 'r' &&
            tok->lexeme.content[1] == 'e' &&
            tok->lexeme.content[2] == 't' &&
            tok->lexeme.content[3] == 'u' &&
            tok->lexeme.content[4] == 'r' &&
            tok->lexeme.content[5] == 'n') tok->kind = kw_return;
    }
}

inline
bool Lexer::is_left_quote(char c) {
    return c == '\"';
}

inline
bool Lexer::is_comment(char c) {
    return c == '#';
}

inline
bool Lexer::is_valid_identifier(char c) {
    return isalnum(c) || c == '_';
}

inline
bool Lexer::is_eof(char c) {
    return c == '\0';
}

inline
bool Lexer::is_digit(char c) {
    return isdigit(c);
}

bool Lexer::is_op(Kind k) {
    return anyone(k, op_add, op_sub, op_mul, op_div, op_lt, op_equal,
                  op_le, op_ge, op_gt, op_mod, op_pow);
}
