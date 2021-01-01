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
#include <type_traits>

#include "helper.h"
#include "Symbol_table.h"
#include "Interpreter/allocator.h"

//    template<bool cond, typename T = std::nullptr_t>
//    using enable_if_t = typename std::enable_if<cond, T>::type;
//
//    template<typename T>
//    inline constexpr bool is_pointer_v =  std::is_pointer<T>::value;
//
//    template<typename T>
//    inline constexpr bool is_null_pointer_v =  std::is_null_pointer<T>::value;
//
//    template<typename T, typename _T = std::remove_reference<T>>
//    using Is_poinrer_or_nullptr = enable_if_t<is_pointer_v<_T> || is_null_pointer_v<_T>>;
//
//    template<typename T, typename _T = std::remove_reference<T>>
//    using Is_not_poinrer_and_nullptr = enable_if_t<!(is_pointer_v<_T> || is_null_pointer_v<_T>)>;


namespace runtime_ns {
    class Runtime {
    public:
        using builtin_func_t = RT_Value (*) (Runtime*, std::vector<RT_Value*>);

        RT_Value* get_variable(const std::string&);

        void creat_variables(const std::vector<std::string>&, std::vector<RT_Value*>);
        void creat_variable(const std::string&, RT_Value*);

        template<typename T>
        void creat_variable(const std::string &name, T &&val) {
            RT_Value *ptr_val = allocator->alloc_var(std::forward<T>(val));

            ptr_val->occupied = true;

            // Replace & deallocate previous value
            if (auto entry = sym_table.back().vars.find(name); entry != sym_table.back().vars.end()) {
                allocator->dealloc_var(entry->second);
            }

            sym_table.back().insert(name, ptr_val);
        }

        RT_Function* get_function(const std::string&);
        void         creat_function(const std::string&, RT_Function*);

        builtin_func_t get_builtin_function(const std::string&);

        void creat_call_frame();
        void destroy_call_frame();

        void clear();
        void register_builtin_func(const std::string& name, builtin_func_t func_ptr);
    public:
        Global_Class_Table class_table;
        std::vector<Symbol_Table> sym_table;
        std::unordered_map<std::string, builtin_func_t> builtin_func;
        std::unique_ptr<Memory_manager> allocator;
        bool has_variable(const std::string &id_name);
        bool has_function(const std::string &name);
        static std::unique_ptr<Runtime> make_runtime();
        Runtime() : allocator(std::make_unique<Memory_manager>())
        {}
    };
}

#endif //COMPILER_RUNTIME_H
