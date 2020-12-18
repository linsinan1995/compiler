//
// Created by Lin Sinan on 2020-12-16.
//
#include <iostream>
#include "Parser.h"

void driver(std::unique_ptr<Parser> &parser) {
    std::vector<std::unique_ptr<Expression_AST>> v = parser->parse();
    if (v.empty()) return ;

    int line = 1;
    for (auto &&expr : v) {
        printf("=========line %d=========\n", line++);
        expr->print();
    }
}

void main_loop(std::unique_ptr<Parser> &parser) {
    std::string code;
    do {
        std::cout << ">> ";
        std::getline(std::cin >> std::ws, code);
        if (code == "QUIT") break;
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
        lexer->load(code.c_str());
        do_lexing(lexer);
    } while (1);
}

int main() {
    int flag;
    std::cout << "Enter 1     => parser\n"
                 "Enter other => lexer\n>> ";
    std::cin >> flag;
    if (flag == 1) {
        std::unique_ptr<Parser> parser = Parser::make_parser("");
        main_loop(parser);
    } else {
        std::unique_ptr<Lexer> lexer = Lexer::make_lexer("");
        main_loop(lexer);
    }

}