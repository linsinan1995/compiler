//
// Created by Lin Sinan on 2020-12-16.
//
#include <iomanip>
#include <iostream>
#include "AST_visitor/AST_Printer.h"
#include "src/Interpreter/Builtin_function.hpp"
#include "Interpreter/AST_Interpreter.h"
#include "Parser.h"

using namespace parser_ns;
using namespace register_ns;

void driver(std::unique_ptr<Parser> &parser) {
    AST_Printer printer {};
    std::vector<std::shared_ptr<Expression_AST>> v = parser->parse();
    if (v.empty()) return ;

    int line = 1;
    for (auto &&expr : v) {
        printf("=========line %d=========\n", line++);
        printer.evaluate(*expr);
    }
}

void main_loop(std::unique_ptr<Parser> &parser) {
    std::string code;
    do {
        std::cout << ">> ";
        std::getline(std::cin >> std::ws, code);
        if (code == "QUIT") return ;
        parser->read_RT(code.c_str());
        driver(parser);
    } while (1);
}

void do_lexing(std::unique_ptr<Lexer> &lex) {
    std::unique_ptr<Token> token = nullptr;
    do {
        token = lex->next();
        if (token->kind != k_unexpected &&
            token->kind != k_EOF)
            printf("%-20s\t%.*s\n", names_kind[token->kind] , (int)token->lexeme.len, token->lexeme.content);
        else
            printf("%s\n", names_kind[token->kind]);
    } while (token->kind != k_unexpected &&
             token->kind != k_EOF);
    printf("============================================\n\n\n");
}

void main_loop(std::unique_ptr<Lexer> &lexer) {
    std::string code;
    do {
        std::cout << ">> ";
        if (!std::getline(std::cin >> std::ws, code)) break;
        if (code == "QUIT") return ;
        lexer->load(code.c_str());
        do_lexing(lexer);
    } while (1);
}

void main_loop_interpreter(std::unique_ptr<Parser> &parser) {
    std::string code;
    auto interpreter = AST_Interpreter();
    // set up built in functions
    builtin_register::register_to_rt(interpreter.rt.get());

    do {
        std::cout << ">> ";
        std::getline(std::cin >> std::ws, code);
        parser->read_RT(code.c_str());
        std::vector<std::shared_ptr<Expression_AST>> v = parser->parse();

        if (v.empty()) return ;

        for (auto &expr : v) {
            interpreter.evaluate(*expr);
            if (!interpreter.is_null()) {
                std::cout << interpreter.val << "\n";
            }
        }
    } while (1);
}

int main() {
    int flag = -1;
    while (flag != 0) {
        std::cout << "Enter 1 => lexer\n"
                     "Enter 2 => parser\n"
                     "Enter 3 => interpreter\n"
                     "Enter 0 => quit\n>> ";
        std::cin >> flag;
        if (flag == 1) {
            std::unique_ptr<Lexer> lexer = Lexer::make_lexer("");
            main_loop(lexer);
        } else if (flag == 2) {
            std::unique_ptr<Parser> parser = Parser::make_parser("");
            main_loop(parser);
        } else if (flag == 3) {
            std::unique_ptr<Parser> parser = Parser::make_parser("");
            main_loop_interpreter(parser);
        }
    }
}