//
// Created by Lin Sinan on 2020-12-16.
//
#include <cstdio>
#include <array>

#include "AST_visitor/AST_Printer.h"
#include "Interpreter/Builtin_function.hpp"
#include "Interpreter/AST_Interpreter.h"
#include "Interpreter/Cmd_reader.h"
#include "Parser.h"

using namespace parser_ns;
using namespace register_ns;
using namespace cmd_reader_ns;

using main_loop_type = void (*) ();

enum allowed_type : int { T_LEXER = 1, T_PARSER = 2, T_INTERPRETER = 3, _SIZE = T_INTERPRETER + 1 };

void do_lexing(std::unique_ptr<Lexer> &lex);
void do_parsing(std::unique_ptr<Parser> &parser);

// base template
template<allowed_type type> void main_loop() { exit(EXIT_SUCCESS); }

// partial specialization for lexer, parser & interpreter
template<> void main_loop<T_LEXER>() {
    auto lexer      = Lexer::make_lexer();
    auto ptr_reader = cmd_reader::make_cmd_reader();

    do {
        printf(">> ");
        char *p_read_chars = ptr_reader->read();
        if (strcmp(p_read_chars, "quit\n") == 0) break;
        lexer->load(p_read_chars);
        do_lexing(lexer);
        if (p_read_chars != nullptr)
            free(p_read_chars);
    } while (true);

}

template<> void main_loop<T_PARSER>() {
    auto parser     = Parser::make_parser();
    auto ptr_reader = cmd_reader::make_cmd_reader();

    do {
        printf(">> ");
        char *p_read_chars = ptr_reader->read();
        if (strcmp(p_read_chars, "quit\n") == 0) break;
        parser->read(p_read_chars);
        do_parsing(parser);
    } while (true);

}

template<> void main_loop<T_INTERPRETER>() {
    auto parser      = Parser::make_parser();
    auto ptr_reader  = cmd_reader::make_cmd_reader();

    AST_Interpreter interpreter {};
    // set up built in functions
    builtin_register::register_to_rt(interpreter.rt.get());

    do {
        printf(">> ");
        char *p_read_chars = ptr_reader->read();
        if (strcmp(p_read_chars, "quit\n") == 0) break;

        parser->read(p_read_chars);
        auto expr_list = parser->parse();

        if (expr_list.empty())
            continue;

        for (auto &expr : expr_list) {
            interpreter.evaluate(*expr);
            if (!interpreter.is_null()) {
                std::cout << *(interpreter.m_val) << "\n";
            }
        }
    } while (true);

    exit(EXIT_SUCCESS);
}

void do_lexing(std::unique_ptr<Lexer> &lex) {
    Kind k_token;

    do {
        auto token = lex->next();
        k_token    = token->kind;

        if (!anyone(k_token, k_EOF))
            printf("%-20s\t%.*s\n", names_kind[token->kind] , (int)token->lexeme.len, token->lexeme.content);
        else
            printf("%s\n", names_kind[token->kind]);
    } while (!anyone(k_token, k_EOF));
    printf("============================================\n\n\n");
}

void do_parsing(std::unique_ptr<Parser> &parser) {
    AST_Printer printer {};
    std::vector<std::unique_ptr<Expression_AST>> v = parser->parse();
    if (v.empty()) return ;

    int line = 1;
    for (auto &&expr : v) {
        printf("=========line %d=========\n", line++);
        printer.evaluate(*expr);
    }
}

constexpr std::array<main_loop_type, _SIZE> look_up_table {
    main_loop<_SIZE>,
    main_loop<T_LEXER>,
    main_loop<T_PARSER>,
    main_loop<T_INTERPRETER>
};

main_loop_type constexpr get_tool(unsigned int flag) { return flag > _SIZE ? look_up_table[0] : look_up_table[flag]; }

int main() {
    unsigned int flag;
    cmd_color_yellow();
    printf("Enter 0 => exit\n"
           "Enter 1 => lexer\n"
           "Enter 2 => parser\n"
           "Enter 3 => interpreter\n\n>> ");
    cmd_color_reset();
    // assignment-supression %*c to eat extra white space
    scanf("%u%*c", &flag);
    get_tool(flag)();
}