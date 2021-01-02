/******************************************************************* 
 * 
 *    @Time           @Author       @Description
 * ------------      ---------      -----------
 * 2021-01-02 20:14  Lin Sinan         None
 *
 * @File    :   String_buffer.cpp
 * @Contact :   mynameisxiaou@gmail.com
 * @github  :   https://github.com/linsinan1995
 * @usage   :
 *
 ******************************************************************/

//
// Created by Lin Sinan on 2021-01-02.
//

#include "String_buffer.h"

string_buffer* string_buffer::make_buffer(size_t initial_capacity, float grow_factor) {
    auto *ptr_buffer = static_cast<string_buffer*> (malloc(sizeof(string_buffer)));
    if (ptr_buffer == nullptr)
        panic_noreturn("Error: Fail to allocate memory for string_buffer!\n");

    ptr_buffer->capacity    = initial_capacity;
    ptr_buffer->length      = 1;
    ptr_buffer->buffer      = static_cast<char*> (malloc(initial_capacity));
    ptr_buffer->grow_factor = grow_factor;

    if (ptr_buffer->buffer == nullptr)
        panic_noreturn("Error: Fail to allocate memory for string_buffer.buffer!\n");

    ptr_buffer->buffer[0] = 0;
    return ptr_buffer;
}

void string_buffer::grow() {
    auto new_capacity = static_cast<size_t>(grow_factor * capacity);
    auto new_buffer   = static_cast<char*> (realloc(buffer, new_capacity));

    if (new_buffer == nullptr)
        panic_noreturn("Error: Fail to allocate memory for string_buffer!\n");

    capacity = new_capacity;
    buffer   = new_buffer;
}

void string_buffer::shrink() {
    auto new_buffer = static_cast<char*> (realloc(buffer, length));
    if (new_buffer == nullptr)
        panic_noreturn("Error: Fail to allocate memory for string_buffer!\n");
    buffer = new_buffer;
}

char* string_buffer::release() {
    shrink();
    char *string = buffer;
    buffer    = static_cast<char*> (malloc(length));
    capacity  = length;
    length    = 1;
    buffer[0] = 0;
    return string;
}

void string_buffer::append(char c) {
    if (capacity == length)
        grow();

    buffer[length-1] = c;
    buffer[length++] = 0;
}

void string_buffer::clear() {
    length    = 1;
    buffer[0] = 0;
}

string_buffer::~string_buffer() {
    free(buffer);
}