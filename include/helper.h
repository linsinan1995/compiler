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

static bool allocate_check(void *ptr, const char *c) {
    if (ptr == nullptr) {
        printf("Fail to allocate memory to %s!\n", c);
        return true;
    }
    return false;
}

static void* f_malloc(size_t t) {
    return malloc(t);
}

static void cmd_color_red()    { printf("\033[1;31m"); }
static void cmd_color_yellow() { printf("\033[1;33m"); }
static void cmd_color_reset()  { printf("\033[0m"); }

static void panic(char const* const format, ...) {
    cmd_color_red();
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    cmd_color_reset();
}

template <typename Ret_Type>
static Ret_Type panic_type(char const* const format, ...) {
    cmd_color_red();
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    cmd_color_reset();
    return {};
}

[[noreturn]] static void panic_noreturn(char const* const format, ...) {
    cmd_color_red();
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    cmd_color_reset();
    exit(EXIT_FAILURE);
}

static std::nullptr_t panic_nptr(char const* const format, ...) {
    cmd_color_red();
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    cmd_color_reset();
    return nullptr;
}
#endif //COMPILER_HELP_H
