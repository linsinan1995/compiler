//
// Created by Lin Sinan on 2020-12-21.
//
#include <iostream>
#include <tuple>
#include "Interpreter/AST_Interpreter.h"
#include "Parser.h"

using namespace parser_ns;
using namespace runtime_ns;

const char *code =
        "# helloworld & println are functions ported from C++ interface\n"
        "helloworld()\n"
        "println(123,42,52)\n"
        "var x = 5\n"
        "var y = 12 + x * 3 + 3\n"
        "func f(a,b) {\n"
        "   x = b - 1\n"
        "   return a*b+x\n"
        "}\n"
        "f(1, y)\n"
        "func_info(\"f\")\n"
        "info()";

struct builtin_register {
    using entry = std::pair<std::string, Runtime::builtin_func_t>;
    std::vector<entry> funcs;

    // as terminator
    void push_back() {};
    template <typename STR, typename... Entries>
    void push_back(STR&& func, Runtime::builtin_func_t func_ptr, Entries && ...args) {
        funcs.emplace_back(std::forward<STR>(func), func_ptr);
        push_back(std::forward<Entries> (args)...);
    }

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
    for (const auto& arg : args) {
        std::cout << rt_value_to_string(arg) << "\n";
    }
    if (args.empty()) std::cout << "\n";
    return RT_Value();
}

RT_Value builtin_print_func_args(Runtime* rt, std::vector<RT_Value> args) {
    if (args.size() != 1) {
        return panic_type<RT_Value>("Runtime Error: Wrong argument number in "
                                    "builtin_print_func_args! It required 1 but got %d",
                                    args.size());
    }

    auto func_name = args[0];

    if (func_name.is_not_type<STRING>()) {
        return panic_type<RT_Value>("Runtime Error: Wrong argument type in "
                                    "builtin_print_func_args! It required string but got %s "
                                    "arguments", names_rt_val_kind[func_name.type]);
    }

    auto func = rt->get_function(args[0].data._str);

    std::cout << "[func]: " << func_name << "\n";
    std::cout << "  [args]: ";
    for (auto &arg : func->params_name) {
        std::cout << arg << " ";
    }
    std::cout<<"\n";
    return {};
}

RT_Value builtin_print_statue(Runtime* rt, std::vector<RT_Value> args) {
    if (!args.empty()) {
        return panic_type<RT_Value>("Runtime Error: Wrong argument number in "
                                    "builtin_print_statue! It required 0 but got %d "
                                    "arguments", args.size());
    }

    std::cout << "\n==========STATUE==========\n";
    std::cout << "[context level]: " << rt->contexts.size() << "\n";
    std::cout << "[local vars]: \n";
    for (auto &[var, val] : rt->contexts.back()->vars) {
        std::cout << var << " : " << val << "\n";
    }

    for (auto &[name, func] : rt->contexts.back()->funcs) {
        builtin_print_func_args(rt, { RT_Value(name) });
    }
    std::cout << "==========================\n";
    return {};
}

int main () {
    builtin_register reg;
    AST_Interpreter interpreter {};

    std::string helloworld = "helloworld";
    std::string println = "println";

    reg.push_back(helloworld, builtin_helloworld,
                  std::move(println), builtin_println,
                  "func_info", builtin_print_func_args,
                  "info", builtin_print_statue);

    // dump buildin functions into the global scope of runtime.
    reg._register(interpreter.rt.get());

    auto parser = Parser::make_parser(code);

    std::vector<std::shared_ptr<Expression_AST>> v = parser->parse();

    for (auto &&expr : v) {
        interpreter.evaluate(*expr);
        if (!interpreter.is_null())
            std::cout << interpreter.val << "\n";
    }
    interpreter.rt->clear();
}