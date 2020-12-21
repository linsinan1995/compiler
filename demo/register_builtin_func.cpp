//
// Created by Lin Sinan on 2020-12-21.
//
#include <iostream>

#include "AST_visitor/AST_Interpreter.h"
#include "Parser.h"

using namespace parser_ns;
using namespace runtime_ns;

static std::string rt_value_to_string(RT_Value val) {
    if (val.is_type<VOID>()) return "null";
    if (val.is_type<INT>()) return std::to_string(val.data._int);
    if (val.is_type<BOOL>()) return std::to_string(val.data._bool);
    if (val.is_type<FP>()) return std::to_string(val.data.fp);
    return {};
}

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

// functions will be registered
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

const char *code =
        "# helloworld & println are functions ported from C++ interface\n"
        "helloworld()\n"
        "println(123,42,52)";

int main () {
    builtin_register reg;
    AST_Interpreter interpreter {};

    reg.funcs.emplace_back(std::make_pair("helloworld", builtin_helloworld));
    reg.funcs.emplace_back(std::make_pair("println", builtin_println));
    // dump buildin functions into the global scope of runtime.
    reg._register(interpreter.rt.get());

    auto parser = Parser::make_parser(code);

    std::vector<std::unique_ptr<Expression_AST>> v = parser->parse();

    for (auto &&expr : v) {
        interpreter.evaluate(*expr);
        if (!interpreter.is_null())
            std::cout << interpreter.val << "\n";
    }
    interpreter.rt->clear();
}