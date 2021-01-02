//
// Created by Lin Sinan on 2021-01-02.
//

#ifndef COMPILER_CMD_READER_H
#define COMPILER_CMD_READER_H
#include <memory>

#include "String_buffer.h"

namespace cmd_reader_ns {

class matcher_stack {
    static constexpr size_t PAREN_MAX_NUMBER = 30;
public:
    size_t size();
    bool   empty();
    void   clear();
    void   push(char c);
    bool   consume(char c);
    matcher_stack() : top(0), data() {}

private:
    char data[PAREN_MAX_NUMBER];
    size_t top;
};

void matcher_stack::push(char c) {
    if (top >= PAREN_MAX_NUMBER)
        panic_noreturn("Exceeded the maximum paren size %ud in include/Interpreter/CMD_reader.h", PAREN_MAX_NUMBER);
    data[top++] = c;
}

class cmd_reader {
    void handler_ill_input();
public:
    char*  read();
    cmd_reader();

    static std::unique_ptr<cmd_reader> make_cmd_reader();
private:
    std::unique_ptr<string_buffer> ptr_sb;
    matcher_stack                  paren_matcher;
};

// matcher_stack
size_t matcher_stack::size()   { return top; }
bool   matcher_stack::empty()  { return top == 0; }
void   matcher_stack::clear()  { top = 0; }

bool matcher_stack::consume(char c) {
    if (empty()) return false;
    if (((data[top - 1] == '(') && (c == ')')) ||
        ((data[top - 1] == '{') && (c == '}')) ) {
        -- top;
        return true;
    }

    return false;
}

// if paren_matcher is empty
//    - meet a \n -> add to sb & release to the lexer
//    - otherwise -> add to sb when meet \n or eof
// else if paren_matcher is not empty
//    - if it's open paren or open curly,  add to matcher & sb
//    - if it's close paren or close curly, compare with the top of matcher
//       - if open & close matching successfully, pop the char from matcher
//         and add close paren to sb
//    - otherwise add to sb
char* cmd_reader::read() {
    do {
        char c = getchar();

        switch (c) {
            default: break;
            case '(':
            case '{':
                paren_matcher.push(c);
                break;
            case ')':
            case '}':
                if (!paren_matcher.consume(c)) {
                    handler_ill_input();
                    return nullptr;
                }
                break;
        }
        ptr_sb->append(c);
        if ((c == '\n' || c == '\0') && paren_matcher.empty()) break;
    } while (true);

    return ptr_sb->release();
}

void cmd_reader::handler_ill_input() {
    paren_matcher.clear();
    ptr_sb->clear();
}

std::unique_ptr<cmd_reader> cmd_reader::make_cmd_reader() { return std::make_unique<cmd_reader> (); }

cmd_reader::cmd_reader()  :
        ptr_sb(string_buffer::make_buffer()),
        paren_matcher({})
{}

} // end of namespace
#endif //COMPILER_CMD_READER_H
