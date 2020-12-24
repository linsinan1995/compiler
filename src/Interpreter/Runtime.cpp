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

void Runtime::register_builtin_func(const std::string& name, builtin_func_t func_ptr) {
    builtin_func[name] = func_ptr;
}

void Runtime::clear() {
    builtin_func.clear();
    contexts.clear();
    creat_context();
}
