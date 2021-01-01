//
// Created by Lin Sinan on 2020-12-23.
//

#ifndef COMPILER_BUILTIN_FUNCTION_H
#define COMPILER_BUILTIN_FUNCTION_H

#include <iostream>

#include "Interpreter/Runtime.h"


namespace built_in_function {
    using namespace runtime_ns;

    // built in function
    RT_Value builtin_println(Runtime* rt, std::vector<RT_Value*> args) {
        for (const auto& arg : args) {
            std::cout << *arg << "\n";
        }
        if (args.empty()) std::cout << "\n";
        return RT_Value();
    }

    RT_Value builtin_print_func_args(Runtime* rt, std::vector<RT_Value*> args) {
        if (args.size() != 1) {
            return panic_type<RT_Value>("Runtime Error: Wrong argument number in "
                                        "builtin_print_func_args! It required 1 but got %d\n",
                                        args.size());
        }

        auto func_name = args[0];

        if (func_name->is_not_type<STRING>()) {
            return panic_type<RT_Value>("Runtime Error: Wrong argument type in "
                                        "builtin_print_func_args! It required string but got %s "
                                        "arguments\n", names_rt_val_kind[func_name->type]);
        }

        auto func = rt->get_function(args[0]->data._str);

        if (!func) {
            return panic_type<RT_Value>("Runtime Error: Wrong function name in "
                                        "builtin_print_func_args! Use info() to check"
                                        "all defined functions\n");
        }

        std::cout << "[func]: " << *func_name << "\n";
        std::cout << "  [args]: ";
        for (auto &arg : func->params_name) {
            std::cout << arg << " ";
        }
        std::cout<<"\n";
        return {};
    }

    RT_Value builtin_print_statue(Runtime* rt, std::vector<RT_Value*> args) {
        if (!args.empty()) {
            return panic_type<RT_Value>("Runtime Error: Wrong argument number in "
                                        "builtin_print_statue! It required 0 but got %d "
                                        "arguments\n", args.size());
        }

        std::cout << "\n==========STATUE==========\n";
        std::cout << "[context level]: " << rt->sym_table.size() << "\n";
        std::cout << "[local vars]: \n";
        for (auto &[var, val] : rt->sym_table.back().vars) {
            std::cout << var << " : " << val << "\n";
        }

        for (auto &[name, func] : rt->sym_table.back().funcs) {
            auto rt_str_name = RT_Value(name);
            builtin_print_func_args(rt, { &rt_str_name });
        }
        std::cout << "==========================\n";
        return {};
    }
}

namespace register_ns {

    using namespace runtime_ns;

    class builtin_register {
        using entry = std::pair<std::string, Runtime::builtin_func_t>;
        std::vector<entry> funcs;
    public:
        // as terminator
        template <typename STR>
        void push_back(STR&& func, Runtime::builtin_func_t func_ptr) {
            funcs.emplace_back(std::forward<STR>(func), func_ptr);
        }

        template <typename STR, typename... Entries>
        void push_back(STR&& func, Runtime::builtin_func_t func_ptr, Entries && ...args) {
            funcs.emplace_back(std::forward<STR>(func), func_ptr);
            push_back(std::forward<Entries> (args)...);
        }

        void _register(Runtime *rt) const {
            for (auto & [name, func] : funcs)
                rt->register_builtin_func(name, func);
        }

        static
        void register_to_rt(Runtime *rt) {
            using namespace built_in_function;
            auto reg = std::make_unique<builtin_register> ();
            reg->push_back("println" ,  builtin_println,
                           "func_info", builtin_print_func_args,
                           "info",      builtin_print_statue);
            reg->_register(rt);
        }
    };
}

#endif //COMPILER_BUILTIN_FUNCTION_H
