//
// Created by Lin Sinan on 2020-12-19.
//

#ifndef COMPILER_RUNTIME_H
#define COMPILER_RUNTIME_H

#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <string>
#include "helper.h"
#include "Symbol_table.h"

namespace runtime_ns {
    class Context {
    public:
        bool         has_variable(const std::string&);
        RT_Value     get_variable(const std::string&);
        void         creat_variable(const std::string&, RT_Value);
        void         creat_variables(const std::vector<std::string>&, std::vector<RT_Value>);

        bool         has_function(const std::string& name);
        void         creat_function(const std::string& name, std::shared_ptr<RT_Function> f);
        std::shared_ptr<RT_Function> get_function(const std::string& name);

    public:
        // store variables & functions
        Symbol_Table sym_table {};
    };

    class Runtime {
    public:
        using builtin_func_t = RT_Value (*) (Runtime*, std::vector<RT_Value>);

        RT_Value       get_variable(const std::string&);
        void           creat_variable(const std::string&, RT_Value);
        void           creat_variables(const std::vector<std::string>&, std::vector<RT_Value>);

        std::shared_ptr<RT_Function> get_function(const std::string& name);
        void           creat_function(const std::string& name, std::shared_ptr<RT_Function> f);

        builtin_func_t get_builtin_function(const std::string&);

        void creat_context();
        void ruin_context();

        void clear();
        static std::shared_ptr<Runtime> make_runtime();
        void register_builtin_func(const std::string& name, builtin_func_t func_ptr);
    public:
        Global_Class_Table class_table;
        std::vector<std::unique_ptr<Context>> contexts;
        std::unordered_map<std::string, builtin_func_t> builtin_func;
    };
}

#endif //COMPILER_RUNTIME_H
