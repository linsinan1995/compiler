#include <utility>

#include <utility>

#include <utility>

/******************************************************************* 
 * 
 *    @Time           @Author       @Description
 * ------------      ---------      -----------
 * 2020-12-19 16:22   Lin Sinan         None
 *
 * @File    :   Runtime.cpp
 * @Contact :   mynameisxiaou@gmail.com
 * @github  :   https://github.com/linsinan1995
 * @usage   :
 *
 ******************************************************************/

//
// Created by Lin Sinan on 2020-12-19.
//

#include "Interpreter/Runtime.h"
using namespace runtime_ns;

using ptr_value = std::unique_ptr<RT_Value>;

void Context::creat_variable(const std::string& id_name, RT_Value val) {
    vars[id_name] = std::move(val);
}

void Context::creat_variables(std::vector<std::string> id_names, std::vector<RT_Value> vals) {
    for (int i = 0; i < id_names.size(); i++) {
        vars.emplace(std::move(id_names[i]), std::move(vals[i]));
    }
}

RT_Value Context::get_variable(const std::string &id_name) {
    if (auto res = vars.find(id_name); res != vars.end()) {
        return res->second;
    }

    panic("Variable %s not found!\n", id_name.c_str());
    return RT_Value();
}

bool Context::has_variable(const std::string& id_name) {
    return vars.find(id_name) != vars.end();
}

void Context::creat_function(const std::string& name, std::shared_ptr<RT_Function> f) {
    funcs.insert({name, f});
}

bool Context::has_function(const std::string& name) {
    return funcs.find(name) != funcs.end();
}

std::shared_ptr<RT_Function> Context::get_function(const std::string& name) {
    if (auto res = funcs.find(name); res != funcs.end()) {
        return res->second;
    }
    return nullptr;
}

template <int _Value_Type>
inline bool RT_Value::is_type() {
    return this->type == _Value_Type;
}

template <int _Value_Type>
inline bool RT_Value::is_not_type() {
    return this->type != _Value_Type;
}

template <> bool RT_Value::is_not_type<STRING>() { return this->type != STRING; }
template <> bool RT_Value::is_type<STRING>() { return this->type == STRING; }

RT_Value RT_Value::operator+(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp + rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int + rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator-(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp - rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int - rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator*(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp * rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int * rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator/(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp / rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int / rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator%(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int / rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator>(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp > rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int > rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator<(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp < rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int < rhs.data._int);
    }

    return RT_Value();
}
RT_Value RT_Value::operator==(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp == rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int == rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator>=(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp >= rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int >= rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator<=(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp <= rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int <= rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator^(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (!(rhs.is_type<INT>())) {
        panic("Runtime Error : the second operand of ^ operator should be integer!\n");
    }

    if (!(rhs.is_type<INT>() || rhs.is_type<FP>())) {
        panic("Runtime Error : the first operand of ^ operator should be integer or float!\n");
    }

    for (int i = 0; i < rhs.data._int; i++) {
        rhs = rhs * rhs;
    }

    return rhs;
}

bool RT_Value::to_bool() {
    if (is_type<BOOL>()) return data._bool;
    if (is_type<VOID>()) return false;
    if (is_type<INT>()) return data._int == 0;
    if (is_type<FP>()) return data.fp == 0.;

    panic("Runtime Error : Unexpected RT_Value type!\n");
    return false;
}

RT_Value &RT_Value::operator=(RT_Value val) {
        if(&val == this)
            return *this;

        type = val.type;
        switch (type) {
            default:
                break;
            case FP:
                data.fp = val.data.fp;
                break;
            case INT:
                data._int = val.data._int;
                break;
            case BOOL:
                data._bool = val.data._bool;
                break;
            case STRING:
                data._str = std::move(val.data._str);
                break;
        }
        return *this;
}

RT_Value::RT_Value(const RT_Value &val) {
    type = val.type;
    switch (val.type) {
        default:
            break;
        case FP:
            data.fp = val.data.fp;
            break;
        case INT:
            data._int = val.data._int;
            break;
        case BOOL:
            data._bool = val.data._bool;
            break;
        case STRING:
            data._str = val.data._str;
            break;
    }
}

RT_Value::RT_Value(RT_Value &&val) noexcept {
    type = val.type;
    switch (val.type) {
        default:
            break;
        case FP:
            data.fp = val.data.fp;
            break;
        case INT:
            data._int = val.data._int;
            break;
        case BOOL:
            data._bool = val.data._bool;
            break;
        case STRING:
            data._str = std::move(val.data._str);
            break;
    }
}

std::ostream &runtime_ns::operator<<(std::ostream &os, const RT_Value &val) {
    switch (val.type) {
        default:
            return os;
        case STRING:
            os << val.data._str;
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

void Runtime::creat_function(const std::string& name, std::shared_ptr<RT_Function> f) {
    contexts.back()->creat_function(name, f);
}

Runtime::buildin_func_t Runtime::get_builtin_function(const std::string& name) {
    return builtin_func[name];
}

std::shared_ptr<RT_Function> Runtime::get_function(const std::string &name) {
    if (contexts.empty()) {
        return nullptr;
    }

    return contexts.back()->get_function(name);
}

void Runtime::creat_variable(const std::string& name, RT_Value rt) {
    contexts.back()->creat_variable(name, std::move(rt));
}

RT_Value Runtime::get_variable(const std::string &name) {
    return contexts[contexts.size()-1]->get_variable(name);
}

void Runtime::creat_context() {
    contexts.emplace_back(std::make_unique<Context> ());
}

void Runtime::ruin_context() {
    contexts.pop_back();
}

void Runtime::creat_variables(std::vector<std::string> id_names, std::vector<RT_Value> vals) {
    contexts.back()->creat_variables(std::move(id_names), std::move(vals));
}

std::shared_ptr<Runtime> Runtime::make_runtime() {
    auto rt = std::make_shared<Runtime>();
    rt->creat_context();
    return rt;
}

void Runtime::register_builtin_func(const std::string& name, buildin_func_t func_ptr) {
    builtin_func[name] = func_ptr;
}

void Runtime::clear() {
    builtin_func.clear();
    contexts.clear();
    creat_context();
}

