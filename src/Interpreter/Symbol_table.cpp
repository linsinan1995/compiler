/******************************************************************* 
 * 
 *    @Time           @Author       @Description
 * ------------      ---------      -----------
 * 2020-12-28 10:11   Lin Sinan         None
 *
 * @File    :   Symbol_table.py    
 * @Contact :   mynameisxiaou@gmail.com
 * @github  :   https://github.com/linsinan1995
 * @usage   :   
 * 
 ******************************************************************/

//
// Created by Lin Sinan on 2020-12-28.
//
#include <utility>
#include <iostream>
#include "Interpreter/Symbol_table.h"
#include "helper.h"

std::shared_ptr<RT_Function> Symbol_Table::get_function(const std::string &func_name) {
    auto entry = funcs.find(func_name);
    if (entry != funcs.end()) {
        return entry->second;
    }

    return panic_nptr("Runtime Error: Function %s is not found!\n", func_name.c_str());
}

RT_Value Symbol_Table::get_variable(const std::string &var_name) {
    if (auto entry = vars.find(var_name); entry != vars.end()) {
        return entry->second;
    }

    return panic_type<RT_Value>("Runtime Error: Variable %s is not found!\n", var_name.c_str());
}

void Symbol_Table::insert(const std::string& name, RT_Value val) {
    vars[name] = std::move(val);
}

void Symbol_Table::insert(const std::string &name, std::shared_ptr<RT_Function> fun) {
    funcs[name] = std::move(fun);
}

Global_Class_Entry *Global_Class_Table::get(const std::string &class_name) {
    if (auto class_entry = class_map.find(class_name); class_entry != class_map.end()) {
        return class_entry->second.get();
    }

    return panic_nptr("Runtime Error: Class %s is not found!\n", class_name.c_str());
}

void Global_Class_Table::insert(const std::string &name, ptr_Gloabl_Class_Entry entry) {
    class_map[name] = std::move(entry);
}

void Global_Class_Entry::dump(Object *obj) {
    // variables
    for (auto & var : vars) {
        obj->member_vars[var.first] = var.second;
    }

    // function
    for (auto & func : funcs) {
        obj->member_functions[func.first] = func.second;
    }
}
