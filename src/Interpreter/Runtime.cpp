#include <utility>

#include <utility>

#include <utility>

#include <utility>

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
    sym_table.insert(id_name, std::move(val));
}

void Context::creat_variables(const std::vector<std::string> &id_names, std::vector<RT_Value> vals) {
    for (int i = 0; i < id_names.size(); i++) {
        sym_table.insert(id_names[i], vals[i]);
    }
}

RT_Value Context::get_variable(const std::string &id_name) {
    return sym_table.get_variable(id_name);
}

bool Context::has_variable(const std::string& id_name) {
    return sym_table.get_variable(id_name).is_type<VOID>();
}

void Context::creat_function(const std::string& name, std::shared_ptr<RT_Function> f) {
    sym_table.insert(name, std::move(f));
}

bool Context::has_function(const std::string& name) {
    return sym_table.get_function(name) == nullptr;
}

std::shared_ptr<RT_Function> Context::get_function(const std::string& name) {
    return sym_table.get_function(name);
}

void Runtime::creat_function(const std::string& name, std::shared_ptr<RT_Function> f) {
    contexts.back()->creat_function(name, std::move(f));
}


Runtime::builtin_func_t Runtime::get_builtin_function(const std::string& name) {
    return builtin_func[name];
}

std::shared_ptr<RT_Function> Runtime::get_function(const std::string &name) {
    if (contexts.empty()) {
        return nullptr;
    }

    auto f = contexts.back()->get_function(name);
    return f == nullptr ? panic_nptr("Function %s not found!\n", name.c_str()) :
                          f;
}

void Runtime::creat_variable(const std::string& name, RT_Value val) {
    contexts.back()->creat_variable(name, std::move(val));
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

void Runtime::creat_variables(const std::vector<std::string> &id_names, std::vector<RT_Value> vals) {
    contexts.back()->creat_variables(id_names, std::move(vals));
}

std::shared_ptr<Runtime> Runtime::make_runtime() {
    auto rt = std::make_shared<Runtime>();
    rt->creat_context();
    return rt;
}

void Runtime::register_builtin_func(const std::string& name, builtin_func_t func_ptr) {
    builtin_func[name] = func_ptr;
}

void Runtime::clear() {
    builtin_func.clear();
    contexts.clear();
    creat_context();
}
