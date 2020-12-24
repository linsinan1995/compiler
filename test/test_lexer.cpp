//
// Created by Lin Sinan on 2020-12-16.
//

#include <cstdio>
#include "Lexer.h"

static int TEST_COUNT = 1;

#define TEST_NAME(X) printf("==============TEST: %-10s==============\n", X);

const char *code =
        "func mat(a,b) { return a*b }\n"
        "x = 24\n"
        "# This is a comment.\n"
        "x = -3\n"
        "y = 21\n"
        "while(x>=0) {\n"
        "   x = x-1\n"
        "}\n"
        "res = (x + y) ^ x\n"
        "if (x+y == 0) {\n"
        "  return 1\n"
        "} else {\n"
        "  return 0\n"
        "}";

const char *code2 =
        "func x(x, z) {\n"
        "   return x*z\n"
        "}\n"
        "print(x(1,2))\n";

const char *code3 =
        "func x(x$ z) {\n";

const char *code4 =
        "var x = 5\n"
        "var y = 12 + x * 3 + 3\n"
        "func f(a,b) {\n"
        "   x = b - 1\n"
        "   return a*b+x\n"
        "}\n"
        "f(1, y)\n"
        "f(1, x)\n"
        "x = f(f(x,1),f(2, f(1,y)))\n"
        "f(1,x)\n"
        "f(1,2)\n";

const char *code5 =
        "func x(a, b) {\n"
        "    return a * b\n"
        "}\n"
        "\n"
        "x(1,2)\n"
        "x(x(1,2),3)";

const char *code6 =
        "mat = [[1,2,3], [2,3,4], [5,6,7]]";

void main_loop(std::unique_ptr<Lexer>& lex) {
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


int main() {
    auto lex  = Lexer::make_lexer(code);

    TEST_NAME("cond stmt")
    main_loop(lex);

    TEST_NAME("func block")
    lex->load(code2);
    main_loop(lex);

    TEST_NAME("err input")
    lex->load(code3);
    main_loop(lex);

    TEST_NAME("def keyword")
    lex->load(code4);
    main_loop(lex);

    TEST_NAME("readme")
    lex->load(code5);
    main_loop(lex);

    TEST_NAME("matrix")
    lex->load(code6);
    main_loop(lex);
}