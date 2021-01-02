//
// Created by Lin Sinan on 2020-12-20.
//
#include <iostream>
#include <string>
#include <iomanip>

#include "Interpreter/AST_Interpreter.h"
#include "Interpreter/Runtime.h"
#include "Parser.h"

using namespace parser_ns;
using namespace runtime_ns;
static int TEST_COUNT = 1;
#define TEST_NAME(X) printf("==============TEST %d: %-10s==============\n", TEST_COUNT++, X);

const char *code =
        "var x = 23.0\n"
        "x = x / 4.0\n"
        "x";  // 5.75

const char *code2 =
        "var x = 5\n"
        "var y = 12 + x * 3 + 3\n"
        "func f(a,b) {\n"
        "   x = b - 1\n"
        "   return a*b+x\n"
        "}\n"
        "f(1, y)\n"  // 59
        "f(1, x)\n"  // 9
        "x = f(f(x,1),f(2, f(1,y)))\n" //
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
        "max(1,3)\n"         // 3
        "max(-231,-253)\n"   // -231
        "max(max(1,4),3)\n"; // 4

const char *code4 =
        "func to_equal(x, z) {\n"
        "    while (x > z) {"
        "       x = x - 1\n"
        "    }\n"
        "    return x\n"
        "}\n"
        "to_equal(10,4)\n"   // 4
        "to_equal(to_equal(10,4),3)\n";  // 3

const char *code5 =
        "func x(a, b) {\n"
        "    return a * b\n"
        "}\n"
        "\n"
        "x(1,2)\n"   // 2
        "x(x(1,2),3)"; // 6

const char *code6 =
        "5\n" // 5
        "f(1,2)\n"
        "func f(a, b) { return a + b }\n"
        "f(-1,2)\n" // 1
        "if (1+2 >= 3) { z = 3 } else { z = 2 } z\n"  // 3
        "func f2(a, b) { if (a > b) { c = a+b } else { c = 0 } return c }\n"
        "f2(1,2)\n" // 0
        "f2(f(1,2),1)\n" // 4
        "f2(2,1) <= f(1,2)" // true
        "f2(2,1) < f(1,2)"; // false

const char *code7 =
        "a = [4,5,6]\n"
        "b = [1,2,3]\n"
        "a*[1,1]\n" // error
        "c = a + b\n"
        "c\n"   // 5,7,9
        "c = a * b\n"
        "c\n"   // 32
        "a * [1,1,1]" // 15
        "a = [[1,2], [3, 4], [5,6]]\n"
        "b = [[1,2,3], [4,5,6]]\n"
        "b*a\n" // 22 28 / 49 64
        "a*b\n" // 9 12 15 / 19 26 33 / 29 40 51
        "[1,1,1]*a\n" // 9 12
        "a*[1,1]\n"   // 3 7 11
        "func mat(a,b) { "
        "    c = a*b \n"
        "    return c+1 \n"
        "}\n"
        "var a = [[1,2,3] ,[4,5,6], [7,8,9]]\n"
        "var b = [[1,2],[3,4],[5,6]]\n"
        "c = mat(a,b)"
        "c"; // 23 29 / 50 65 / 77 101

const char *code8 =
        "class MyVar {\n"
        "    inc = 1\n"
        "    m_val = 10\n"
        "\n"
        "    func inc() {\n"
        "        m_val = m_val + 1\n"
        "    }\n"
        "\n"
        "    func get_val() {\n"
        "        return m_val\n"
        "    }\n"
        "\n"
        "    func add_val_if_gt_inc(x) {\n"
        "        if (x > m_val) {\n"
        "            x = x + m_val\n"
        "        } else {\n"
        "            m_val = m_val + inc\n"
        "            inc()    \n"
        "        }\n"
        "        return x\n"
        "    }\n"
        "}\n"
        "\n"
        "MyVar my_var\n"
        "my_var.get_inc()\n"
        "\n"
        "x = 10\n"
        "\n"
        "my_var.add_val_if_gt_inc(x)\n"
        "x = x + 5\n"
        "my_var.add_val_if_gt_inc(x)\n"
        "my_var.get_val()\n"
//        "my_var.m_val = 50\n"
        "my_var.inc()\n"
        "my_var.get_val()\n"
        "my_var.inc()\n"
        "my_var.get_val()\n"
        "MyVar my_var_2\n" //
        "my_var_2.get_val()\n"
        "my_var.get_val()\n"
        "my_var_2.m_val\n"
        "my_var.m_val\n"
        "";


struct builtin_register {
    std::vector<std::pair<std::string, Runtime::builtin_func_t>> funcs;

    void _register(Runtime *rt) const {
        for (auto & [name, func] : funcs)
            rt->register_builtin_func(name, func);
    }
};

void driver(std::unique_ptr<Parser> &parser, AST_Interpreter& visitor) {
    std::vector<std::unique_ptr<Expression_AST>> v = parser->parse();
    if (v.empty()) return ;
    for (auto &expr : v) {
        visitor.evaluate(*expr);
        if (!visitor.is_null())
            std::cout << *visitor.m_val << "\n";
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

RT_Value builtin_helloworld(Runtime* rt, const std::vector<RT_Value>& args) {
    std::cout << "Hello world!\n";
    return RT_Value();
}

RT_Value builtin_println(Runtime* rt, const std::vector<RT_Value>& args) {
    for (const auto& arg : args) {
        std::cout << rt_value_to_string(arg) << "\n";
    }
    if (args.empty()) std::cout << "\n";
    return RT_Value();
}

int main() {
    std::unique_ptr<Parser> parser = Parser::make_parser(code);
    AST_Interpreter interpreter {};
    std::cout << "sizeof(RT_Value): " << sizeof(RT_Value) <<std::endl;

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

    TEST_NAME("Bug in operator>")
    parser = Parser::make_parser(code6);
    driver(parser, interpreter);

    TEST_NAME("matrix")
    parser = Parser::make_parser(code7);
    driver(parser, interpreter);

    TEST_NAME("Class")
    parser = Parser::make_parser(code8);
    driver(parser, interpreter);
}

