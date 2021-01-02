//
// Created by Lin Sinan on 2021-01-02.
//

#ifndef COMPILER_STRING_BUFFER_H
#define COMPILER_STRING_BUFFER_H

#include "helper.h"

class string_buffer {
    void grow();
    void shrink();
public:
    char* release();
    void  append(char c);
    void  clear();
    ~string_buffer();

    static string_buffer* make_buffer(size_t initial_capacity = 50, float grow_factor = 1.5);
private:
    size_t capacity;
    size_t length;
    float  grow_factor;
    char  *buffer;
};

#endif //COMPILER_STRING_BUFFER_H
