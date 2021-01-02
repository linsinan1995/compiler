//
// Created by Lin Sinan on 2021-01-02.
//

#ifndef COMPILER_STRING_VIEW_H
#define COMPILER_STRING_VIEW_H

#include <string>

struct _string_view {
    size_t       len;
    const char  *content;
    std::string to_string() { return std::string(content, len); };
};

#endif //COMPILER_STRING_VIEW_H
