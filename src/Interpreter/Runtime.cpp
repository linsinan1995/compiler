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

void Runtime::creat_function(const std::string& name, RT_Function* f) {
    if (auto entry = sym_table.back().funcs.find(name); entry != sym_table.back().funcs.end()) {
        allocator->dealloc_func(entry->second);
    }

    sym_table.back().insert(name, f);
}

Runtime::builtin_func_t Runtime::get_builtin_function(const std::string& name) {
    if (auto entry = builtin_func.find(name); entry != builtin_func.end()) {
        return entry->second;
    }
    return nullptr;
}

bool Runtime::has_variable(const std::string& id_name) {
    return sym_table.back().get_variable(id_name)->is_type<VOID>();
}

bool Runtime::has_function(const std::string& name) {
    return sym_table.back().get_function(name) == nullptr;
}

RT_Function* Runtime::get_function(const std::string &name) {
    return sym_table.back().get_function(name);
}

RT_Value* Runtime::get_variable(const std::string &name) {
    return sym_table.back().get_variable(name);
}

void Runtime::creat_call_frame() {
    sym_table.push_back({});
}

void Runtime::destroy_call_frame() {
    // todo:
    //      garbage collections & memory check
    sym_table.pop_back();
}

void Runtime::creat_variables(const std::vector<std::string> &id_names, std::vector<RT_Value*> vals) {
    for (int i = 0; i < id_names.size(); i++) {
        // copy to instead of move
        // allocator->alloc_var(vals[i]) will copy a pointer to invoke RT_Value(bool)
        creat_variable(id_names[i], vals[i]);
    }
}

std::unique_ptr<Runtime> Runtime::make_runtime() {
    auto rt = std::make_unique<Runtime>();
    rt->creat_call_frame();
    return rt;
}

void Runtime::register_builtin_func(const std::string& name, builtin_func_t func_ptr) {
    builtin_func[name] = func_ptr;
}

void Runtime::clear() {
    builtin_func.clear();
}

void Runtime::creat_variable(const std::string &name, RT_Value *ptr_val) {
    if (!ptr_val) ptr_val = allocator->alloc_var();
    else          ptr_val = allocator->alloc_var(*ptr_val);

    ptr_val->occupied = true;

    // Replace & deallocate previous value
    if (auto entry = sym_table.back().vars.find(name); entry != sym_table.back().vars.end()) {
        allocator->dealloc_var(entry->second);
    }

    sym_table.back().insert(name, ptr_val);
}

