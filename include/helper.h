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
    return raw_string.content? std::string(raw_string.content, raw_string.len) :
                               "";
}

static bool allocate_check(void *ptr, const char *c) {
    if (ptr == nullptr) {
        printf("Fail to allocate memory to %s!\n", c);
        return true;
    }
    return false;
}

static float get_val_tok_fp(Token *tok) {
    return strtof(tok->lexeme.content, nullptr);
}

static int get_val_tok_int(Token *tok) {
    return static_cast<int> (strtol(tok->lexeme.content, nullptr, 10));
}

static void* f_malloc(size_t t) {
    return malloc(t);
}


static void panic(char const* const format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

template <typename Ret_Type>
static Ret_Type panic_type(char const* const format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    return {};
}

[[noreturn]] static void panic_noreturn(char const* const format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

static std::nullptr_t panic_nptr(char const* const format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    return nullptr;
}
#endif //COMPILER_HELP_H
