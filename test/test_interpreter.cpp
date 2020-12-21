//
// Created by Lin Sinan on 2020-12-20.
//
#include <iostream>
#include <string>
#include <iomanip>

#include "AST_visitor/AST_Interpreter.h"
#include "Runtime.h"
#include "Parser.h"

using namespace parser_ns;
using namespace runtime_ns;
static int TEST_COUNT = 1;
#define TEST_NAME(X) printf("==============TEST %d: %-10s==============\n", TEST_COUNT++, X);

const char *code =
        "var x = 23\n"
        "x = x / 4\n"
        "x";

const char *code2 =
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
        "f(1,2)\n"
        "# f(a, b)\n"
        "# f(x)";

const char *code3 =
        "func max(x, z) {\n"
        "    if (x > z) {"
        "       a = x\n"
        "       x = z\n"
        "       z = a\n"
        "    }\n"
        "    return z\n"
        "}\n"
        "\n"
        "max(1,3)\n"
        "max(-231,-253)\n"
        "max(max(1,4),3)\n";

const char *code4 =
        "func to_equal(x, z) {\n"
        "    while (x > z) {"
        "       x = x - 1\n"
        "    }\n"
        "    return x\n"
        "}\n"
        "to_equal(10,4)\n"
        "to_equal(to_equal(10,4),3)\n";

const char *code5 =
        "func x(a, b) {\n"
        "    return a * b\n"
        "}\n"
        "\n"
        "x(1,2)\n"
        "x(x(1,2),3)";

const char *code6 =
        "helloworld()\n"
        "println(123,42,52)";

std::ostream& operator<<(std::ostream &os, RT_Value val) {
    switch (val.type) {
        default:
            return os;
        case FP:
            os << std::to_string(val.data.fp);
            return os;
        case INT:
            os << std::to_string(val.data._int);
            return os;
        case BOOL:
            os << std::boolalpha << val.data._bool;
            return os;
    }
}

struct builtin_register {
    std::vector<std::pair<std::string, Runtime::buildin_func_t>> funcs;

    void _register(Runtime *rt) const {
        for (auto & [name, func] : funcs)
            rt->register_builtin_func(name, func);
    }
};

void driver(std::unique_ptr<Parser> &parser, AST_Interpreter& visitor) {
    std::vector<std::unique_ptr<Expression_AST>> v = parser->parse();
    if (v.empty()) return ;

    for (auto &&expr : v) {
        visitor.evaluate(*expr);
        if (!visitor.is_null())
            std::cout << visitor.val << "\n";
    }
    visitor.rt->clear();
}

static std::string rt_value_to_string(RT_Value val) {
    if (val.is_type<VOID>()) return "null";
    if (val.is_type<INT>()) return std::to_string(val.data._int);
    if (val.is_type<BOOL>()) return std::to_string(val.data._bool);
    if (val.is_type<FP>()) return std::to_string(val.data.fp);
    return {};
}

RT_Value builtin_helloworld(Runtime* rt, std::vector<RT_Value> args) {
    std::cout << "Hello world!\n";
    return RT_Value();
}

RT_Value builtin_println(Runtime* rt, std::vector<RT_Value> args) {
    for (auto arg : args) {
        std::cout << rt_value_to_string(arg) << "\n";
    }
    if (args.empty()) std::cout << "\n";
    return RT_Value();
}

int main() {
    std::unique_ptr<Parser> parser = Parser::make_parser(code);
    AST_Interpreter interpreter {};

    TEST_NAME("simple def")
    driver(parser, interpreter);

    TEST_NAME("func def & call")
    parser = Parser::make_parser(code2);
    driver(parser, interpreter);

    TEST_NAME("if block")
    parser = Parser::make_parser(code3);
    driver(parser, interpreter);

    TEST_NAME("while block")
    parser = Parser::make_parser(code4);
    driver(parser, interpreter);

    TEST_NAME("Read me")
    parser = Parser::make_parser(code5);
    driver(parser, interpreter);

    TEST_NAME("Register a built-in function");
    builtin_register reg;
    reg.funcs.emplace_back(std::make_pair("helloworld", builtin_helloworld));
    reg.funcs.emplace_back(std::make_pair("println", builtin_println));

    reg._register(interpreter.rt.get());
    parser = Parser::make_parser(code6);
    driver(parser, interpreter);
}