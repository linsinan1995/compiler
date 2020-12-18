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

int main() {
    std::unique_ptr<Parser> parser = Parser::make_parser("");
    main_loop(parser);
}