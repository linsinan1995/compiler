//
// Created by Lin Sinan on 2020-12-28.
//

#ifndef COMPILER_SYMBOL_TABLE_H
#define COMPILER_SYMBOL_TABLE_H

#include <unordered_map>
#include <string>
#include <stack>
#include "Value.h"
#include "Enum.h"

using str_value_list    = std::vector<std::pair<std::string, RT_Value*>>;
using str_ptr_func_list = std::vector<std::pair<std::string, RT_Function*>>;

struct Symbol_Table {
    void insert(const std::string &, RT_Value*);
    void insert(const std::string &, RT_Function*);
    RT_Function* get_function(const std::string &);
    RT_Value* get_variable(const std::string &);

    std::unordered_map<std::string, RT_Value*> vars {};
    std::unordered_map<std::string, RT_Function*> funcs {};
};

struct Global_Class_Entry {
    str_value_list    vars;
    str_ptr_func_list funcs;
    void dump_function(Object*);
};

using ptr_Gloabl_Class_Entry = std::unique_ptr<Global_Class_Entry>;

struct Global_Class_Table {
    void insert(const std::string&, ptr_Gloabl_Class_Entry entry);
    Global_Class_Entry* get(const std::string&);
    std::unordered_map<std::string, ptr_Gloabl_Class_Entry> class_map;
};
#endif //COMPILER_SYMBOL_TABLE_H
