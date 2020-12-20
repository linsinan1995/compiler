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

static std::string raw_to_string(raw_string &raw_string) {
    return std::string(raw_string.content, raw_string.len);
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
        default:
            return false;
    }
}

static
bool is_uni_operator(Token* token, char op) {
    switch (op) {
        case '(':
            token->kind = k_open_paren;
            return true;
        case ')':
            token->kind = k_close_paren;
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
        case '-':
            token->kind = op_sub;
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
bool allocate_check(void *ptr, const char *c) {
    if (ptr == nullptr) {
        printf("Fail to allocate memory to %s!\n", c);
        return true;
    }
    return false;
}

static float get_val_tok_fp(Token *tok) {
    return strtof(tok->lexeme.content, nullptr);
}

static void* f_malloc(size_t t) {
    return malloc(t);
}

static bool is_op(Kind k) {
    return anyone(k, op_add, op_sub, op_mul, op_div, op_lt, op_equal,
                     op_le, op_ge, op_gt, op_mod, op_pow);
}

static std::nullptr_t LogError(std::string str) {
    fprintf(stderr, "LogError: %s\n", str.c_str());
    return nullptr;
}

[[noreturn]] static void panic(char const* const format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}
#endif //COMPILER_HELP_H
