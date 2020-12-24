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

struct Block_AST;
struct Expression_AST;

namespace runtime_ns {
    using Value_Type = int;

    struct Mat {
        std::vector<float> data;
        std::vector<int> dim;
    };

    class RT_Value {
        union VALUE_Data {
            float       fp{};
            int         _int;
            bool        _bool;
            std::string _str;
            Mat         matrix;
            VALUE_Data() {}
            ~VALUE_Data() {}
            explicit VALUE_Data(int val) { _int = val; };
            explicit VALUE_Data(bool val) { _bool = val; };
            explicit VALUE_Data(float val) { fp = val; }
            explicit VALUE_Data(std::string val) { _str = std::move(val); };
        };
    public:
        RT_Value() : type(VOID) {};
        explicit RT_Value(float val) : type(FP), data(val) {};
        explicit RT_Value(bool val) : type(BOOL), data(val) {};
        explicit RT_Value(int val) : type(INT), data(val) {};
        explicit RT_Value(Mat val) : type(MATRIX) { new (&data.matrix) Mat(std::move(val)); };
        // placement new & tagged union for storing string in union
        explicit RT_Value(std::string val) : type(STRING), data(std::move(val)) {};
        RT_Value(const RT_Value& val);
        RT_Value(RT_Value&& val) noexcept ;

        RT_Value &operator=(RT_Value val);
        RT_Value operator+(RT_Value rhs);
        RT_Value operator-(RT_Value rhs);
        RT_Value operator*(RT_Value rhs);
        RT_Value operator/(RT_Value rhs);
        RT_Value operator%(RT_Value rhs);
        RT_Value operator>(RT_Value rhs);
        RT_Value operator<(RT_Value rhs);
        RT_Value operator==(RT_Value rhs);
        RT_Value operator>=(RT_Value rhs);
        RT_Value operator<=(RT_Value rhs);
        RT_Value operator^(RT_Value rhs);

        friend std::ostream& operator<<(std::ostream &os, const RT_Value &val);
    public:
        Value_Type  type;
        VALUE_Data  data;
        bool to_bool();
        template <int _Value_Type> bool is_type();
        template <int _Value_Type> bool is_not_type();
    };

    struct RT_Function {
        std::vector<std::string>        params_name;
        std::vector<RT_Value>           params;
        std::shared_ptr<Block_AST>      block;
        std::shared_ptr<Expression_AST> ret;
    };


    class Context {
    public:
        bool         has_variable(const std::string&);
        RT_Value     get_variable(const std::string&);
        void         creat_variable(const std::string&, RT_Value);
        void         creat_variables(std::vector<std::string>, std::vector<RT_Value>);

        bool         has_function(const std::string& name);
        void         creat_function(const std::string& name, std::shared_ptr<RT_Function> f);
        std::shared_ptr<RT_Function> get_function(const std::string& name);

    public:
        // store variables & functions
        std::unordered_map<std::string, RT_Value>     vars;
        std::unordered_map<std::string, std::shared_ptr<RT_Function> > funcs;
    };

    class Runtime {
    public:
        using builtin_func_t = RT_Value (*)(Runtime*, std::vector<RT_Value>);

        RT_Value       get_variable(const std::string&);
        void           creat_variable(const std::string&, RT_Value);
        void           creat_variables(std::vector<std::string>, std::vector<RT_Value>);

        std::shared_ptr<RT_Function> get_function(const std::string& name);
        void           creat_function(const std::string& name, std::shared_ptr<RT_Function> f);

        builtin_func_t get_builtin_function(const std::string&);

        void creat_context();
        void ruin_context();

        void clear();
        static std::shared_ptr<Runtime> make_runtime();
        void register_builtin_func(const std::string& name, builtin_func_t func_ptr);
    public:
        std::vector<std::unique_ptr<Context>> contexts;
        std::unordered_map<std::string, builtin_func_t> builtin_func;
    };
}

#endif //COMPILER_RUNTIME_H
