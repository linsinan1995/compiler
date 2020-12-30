//
// Created by Lin Sinan on 2020-12-28.
//

#ifndef COMPILER_SYMBOL_TABLE_H
#define COMPILER_SYMBOL_TABLE_H

#include <unordered_map>
#include <string>
#include "Value.h"
#include "Enum.h"

//struct Symbol_Entry_Type {
//    Kind_rt_value type;
//    Symbol_Entry_Type(Kind_rt_value entry_type) : type(entry_type)
//    {}
//};
//
//struct Builtin_Type : public Symbol_Entry_Type {
//    RT_Value *val;
//    Builtin_Type(Kind_rt_value entry_type, RT_Value *entry_val) :
//        Symbol_Entry_Type(entry_type),
//        val(entry_val)
//    {}
//};
//
//struct Customized_Type : public Symbol_Entry_Type {
//    std::string type_name;
//    std::unordered_map<std::string, RT_Value*>    vars;
//    std::unordered_map<std::string, RT_Function*> funcs;
//    Customized_Type(Kind_rt_value entry_type, std::string entry_type_name) :
//        Symbol_Entry_Type(entry_type),
//        type_name(std::move(entry_type_name))
//    {}
//};
//
////Kind_rt_value
//struct Symbol_Entry {
//    std::string        name;
//    Symbol_Entry_Type *var_type;
//    Kind_rt_value get_type() { return var_type->type; }
//
//    bool is_class() { return var_type->type == CLASS; }
//};

struct Symbol_Table {
//    void remove(const std::string &);
    void insert(const std::string &, RT_Value);
    void insert(const std::string &, std::shared_ptr<RT_Function>);
    std::shared_ptr<RT_Function> get_function(const std::string &);
    RT_Value get_variable(const std::string &);

    std::unordered_map<std::string, RT_Value> vars {};
    std::unordered_map<std::string, std::shared_ptr<RT_Function>> funcs {};
};

using str_value_list    = std::vector<std::pair<std::string, RT_Value>>;
using str_ptr_func_list = std::vector<std::pair<std::string, std::shared_ptr<RT_Function>>>;

struct Global_Class_Entry {
    str_value_list    vars;
    str_ptr_func_list funcs;
    void dump(Object*);
};

using ptr_Gloabl_Class_Entry = std::unique_ptr<Global_Class_Entry>;

struct Global_Class_Table {
    void insert(const std::string&, ptr_Gloabl_Class_Entry entry);
    Global_Class_Entry* get(const std::string&);
    std::unordered_map<std::string, ptr_Gloabl_Class_Entry> class_map;
};
#endif //COMPILER_SYMBOL_TABLE_H
