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
    f->occupied = true;

    if (auto entry = sym_table.back().funcs.find(name); entry != sym_table.back().funcs.end()) {
        allocator->dealloc_func(entry->second);
    }

    sym_table.back().insert(name, f);
}

Runtime::builtin_func_t Runtime::get_builtin_function(const std::string& name) {
    return builtin_func[name];
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

//template<typename T>
//void Runtime::creat_variable(const std::string& name, T val,
//                    typename std::enable_if<!(std::is_pointer<T>::value ||
//                                              std::is_null_pointer<T>::value)>::type) {
//    if (!val) val = allocator->alloc_var();
//    else      val = allocator->alloc_var(*val);
//
//    val->occupied = true;
//
//    // Replace & deallocate previous value
//    if (auto entry = sym_table.back().vars.find(name); entry != sym_table.back().vars.end()) {
//        allocator->dealloc_var(entry->second);
//    }
//
//    sym_table.back().insert(name, val);
//}
//void Runtime::creat_variable(const std::string& name, RT_Value *val) {
//    if (!val) val = allocator->alloc_var();
//    else      val = allocator->alloc_var(*val);
//
//    val->occupied = true;
//
//    // Replace & deallocate previous value
//    if (auto entry = sym_table.back().vars.find(name); entry != sym_table.back().vars.end()) {
//        allocator->dealloc_var(entry->second);
//    }
//
//    sym_table.back().insert(name, val);
//}

RT_Value* Runtime::get_variable(const std::string &name) {
    return sym_table.back().get_variable(name);
}

void Runtime::creat_call_frame() {
    sym_table.push_back({});
}

void Runtime::destroy_call_frame() {
    sym_table.pop_back();
}

void Runtime::creat_variables(const std::vector<std::string> &id_names, std::vector<RT_Value*> vals) {
    for (int i = 0; i < id_names.size(); i++) {
        // copy to instead of move
        // allocator->alloc_var(vals[i]) will copy a pointer to invoke RT_Value(bool)
        creat_variable(id_names[i], allocator->alloc_var(*vals[i]));
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

