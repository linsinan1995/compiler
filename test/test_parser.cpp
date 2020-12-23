//
// Created by Lin Sinan on 2020-12-21.
//

//
// Created by Lin Sinan on 2020-12-16.
//
#include <fstream>
#include "AST_visitor/AST_Printer.h"
#include "Ast.h"
#include "Parser.h"

using namespace parser_ns;
static int TEST_COUNT = 1;
#define TEST_NAME(X) printf("==============TEST %d: %-10s==============\n", TEST_COUNT++, X);

void print_expression(Expression_AST *exp);

const char *code3 =
        "var x = (2 + 3) * 4\n"
        "x = 2 + 3 * 4\n"
        "func pow(x, y) {\n"
        "   return x^y\n"
        "}\n"
        "# test !\n"
        "x = ((12+4) * 8) \n"
        "if (1+2 >= 3) {\n\n"
        "   # test !\n"
        "   var z = 12 + x * 3\n"
        "   x = z + 26 % -21\n"
        "} else {"
        "   x = (12 ^ 2) * pow(2,3) \n"
        "   x = fun(x) \n"
        "}";

const char *code2 =
        "var y = 12 + x * 3 + 3\n"
        "if (z + 23 * (23 - 4) * 4 / (1+2^4)) {}";

const char *code =
        "var x = 5\n"
        "var y = 12 + x * 3 + 3\n"
        "func f(a,b) {\n"
        "   # x = b - 1\n"
        "   return a*b+x\n"
        "}\n"
        "f(1, y)\n"
        "f(1, x)\n"
        "x = f(f(x,1),f(2, f(1,y)))\n"
        "f(1,x)\n"
        "f(1,2)\n"
        "# f(a, b)\n"
        "# f(x)";

const char *code4 =
        "func max(x, z) {\n"
        "    if (x > z) {"
        "#       a = x\n"
        "       x = z\n"
        "       z = x\n"
        "    }\n"
        "    return z\n"
        "}\n"
        "\n"
        "max(1,3)\n"
        "max(max(1,4),3)\n";

const char *code5 =
        "func x(a, b) {\n"
        "    return a * b\n"
        "}\n"
        "\n"
        "x(1,2)\n"
        "x(x(1,2),3)";

const char *code6 =
        "x = 5\n"
        "y=6\n"
        "x>y\n"
        "x>5";

void driver(std::unique_ptr<Parser> &parser, AST_Printer& visitor) {
    std::vector<std::shared_ptr<Expression_AST>> v = parser->parse();
    if (v.empty()) return ;

    int line = 1;
    for (auto &&expr : v) {
        printf("=========line %d=========\n", line++);
        visitor.evaluate(*expr);
    }
}

void driver_to_file(std::unique_ptr<Parser> &parser, const char *file_name) {
    std::ofstream out(file_name);
    if (out.bad()) {
        panic_noreturn("Fail to open/create file %s\n", file_name);
    }

    AST_Printer visitor (out);
    std::vector<std::shared_ptr<Expression_AST>> v = parser->parse();
    if (v.empty()) return ;

    int line = 1;
    for (auto &&expr : v) {
        out << "=========line " << line++ << "=========\n";
        visitor.evaluate(*expr);
    }
}

int main() {
    std::unique_ptr<Parser> parser = Parser::make_parser(code);
    TEST_NAME("simple def")

    AST_Printer printer {};
    driver(parser, printer);

    TEST_NAME("precedence")
    parser = Parser::make_parser(code2);
    driver(parser, printer);

    TEST_NAME("func block")
    parser = Parser::make_parser(code3);
    driver(parser, printer);

    TEST_NAME("if block")
    parser = Parser::make_parser(code4);
    driver(parser, printer);

    TEST_NAME("readme")
    parser = Parser::make_parser(code5);
    driver(parser, printer);

    TEST_NAME("write to file")
    parser = Parser::make_parser(code5);
    driver_to_file(parser, "out.txt");

    TEST_NAME("debug operator >")
    parser = Parser::make_parser(code6);
    driver(parser, printer);
}