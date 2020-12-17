//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_HELP_H
#define COMPILER_HELP_H

#include <memory>
#include "Token.h"

template <typename _DesireType, typename... _ArgumentType>
inline bool anyone(_DesireType k, _ArgumentType... args) {
    return ((args == k) || ...);
}


static
bool is_ambigious_operator(Token* token, char c) {
    switch (c) {
        case '>':
            token->kind = op_gt;
            return true;
        case '<':
            token->kind = op_lt;
            return true;
        case '=':
            token->kind = op_assign;
            return true;
        case '-':
            token->kind = op_sub;
            return true;
        default:
            return false;
    }
}

static
bool is_uni_operator(Token* token, char op) {
    switch (op) {
        case '(':
            token->kind = k_open_parent;
            return true;
        case ')':
            token->kind = k_close_parent;
            return true;
        case '+':
            token->kind = op_add;
            return true;
        case '{':
            token->kind = k_open_curly;
            return true;
        case '}':
            token->kind = k_close_curly;
            return true;
        case '/':
            token->kind = op_div;
            return true;
        case '*':
            token->kind = op_mul;
            return true;
        case '%':
            token->kind = op_mod;
            return true;
        case ';':
            token->kind = k_semi;
            return true;
        case '^':
            token->kind = op_pow;
            return true;
        case ',':
            token->kind = k_comma;
            return true;
        default:
            return false;
    }
}

static
bool is_white_space(char c) {
    return anyone(c, ' ', '\t', '\r', '\n');
}

static
bool is_identifier_prefix(char c) {
    return isalpha(c) || c == '_';
}

static
void check_keyword(Token *tok) {
    if (tok->lexeme.len == 1) {
        if (tok->lexeme.content[0] == '=')
            tok->kind = op_assign;
        else return;
    }

    if (tok->lexeme.len <= 6 && tok->lexeme.len > 1) {
        if (tok->lexeme.content[0] == '=' && tok->lexeme.len == 2) {
            if (tok->lexeme.content[1] == '=') {
                tok->kind = op_equal;
            }
        } else if (tok->lexeme.content[0] == 'i') {
            if (tok->lexeme.len == 2 &&
                tok->lexeme.content[1] == 'f') {
                tok->kind = kw_if;
            }
        } else if (tok->lexeme.content[0] == 'r') {
            if (tok->lexeme.len == 6 &&
                tok->lexeme.content[1] == 'e' &&
                tok->lexeme.content[2] == 't' &&
                tok->lexeme.content[3] == 'u' &&
                tok->lexeme.content[4] == 'r' &&
                tok->lexeme.content[5] == 'n') {
                tok->kind = kw_return;
            }
        } else if (tok->lexeme.content[0] == 'f') {
            if (tok->lexeme.len == 4 &&
                tok->lexeme.content[1] == 'u' &&
                tok->lexeme.content[2] == 'n' &&
                tok->lexeme.content[3] == 'c') {
                tok->kind = kw_func;
            }
        } else if (tok->lexeme.content[0] == 'e') {
            if (tok->lexeme.len == 4 &&
                tok->lexeme.content[1] == 'l' &&
                tok->lexeme.content[2] == 's' &&
                tok->lexeme.content[3] == 'e') {
                tok->kind = kw_else;
            }
        } else if (tok->lexeme.content[0] == 'w') {
            if (tok->lexeme.len == 5 &&
                tok->lexeme.content[1] == 'h' &&
                tok->lexeme.content[2] == 'i' &&
                tok->lexeme.content[3] == 'l' &&
                tok->lexeme.content[4] == 'e') {
                tok->kind = kw_while;
            }
        } else if (tok->lexeme.content[0] == 'v') {
            if (tok->lexeme.len == 3 &&
                tok->lexeme.content[1] == 'a' &&
                tok->lexeme.content[2] == 'r') {
                tok->kind = kw_def;
            }
        }
    }
}

static bool is_comment(char c) { return c == '#'; }
static bool is_valid_identifier(char c) { return isalnum(c) || c == '_'; }
static bool is_eof(char c) { return c == '\0'; }
// unify the function name
static bool is_digit(char c) { return isdigit(c); }
static bool is_alnum(char c) { return isalnum(c); }

static
void panic(const char *tok, const char *loc) {
    printf("Parsing Error: can't find the %s in %s!\n", tok, loc);
}

static
void panic3(const char *target_tok, const char *tok, const char *loc) {
    printf("Parsing Error: can't find the %s (instead %s is found) in %s!\n",
           target_tok, tok, loc);
}


static
bool allocate_check(void *ptr, const char *c) {
    if (ptr == nullptr) {
        printf("Fail to allocate memory to %s!\n", c);
        return true;
    }
    return false;
}

//static int precedence(Kind op) {
//    switch (op) {
//        default:
//            return 0;
//        case op_equal:
//        case op_gt:
//        case op_lt:
//        case op_ge:
//        case op_le:
//            return 3;
//        case op_add:
//        case op_sub:
//            return 4;
//        case op_mul:
//        case op_div:
//        case op_mod:
//            return 5;
//        case op_pow:
//            return 6;
//    }
//}

static int get_val_tok_int(Token *tok) {
    return (int) strtol(tok->lexeme.content, nullptr, 10);
}

static void* f_malloc(size_t t) {
    return malloc(t);
}


#endif //COMPILER_HELP_H
