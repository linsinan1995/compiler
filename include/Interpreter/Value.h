#include <utility>

//
// Created by Lin Sinan on 2020-12-28.
//

#ifndef COMPILER_VALUE_H
#define COMPILER_VALUE_H

#include <vector>
#include "Enum.h"

struct Block_AST;
struct Expression_AST;
struct RT_Value;
struct Object;
struct RT_Function;

using Value_Type = int;
using ptr_obj = std::shared_ptr<Object>;

struct Mat {
    std::vector<float> data;
    std::vector<int> dim;
};

struct Object {
    std::string type_name;
    std::shared_ptr<RT_Function> get_function(const std::string &);
    RT_Value get_variable(const std::string &);

    std::unordered_map<std::string, RT_Value>     member_vars {};
    std::unordered_map<std::string, std::shared_ptr<RT_Function>>  member_functions {}; // weak_ptr
};

class RT_Value {
    union VALUE_Data {
        float       fp;
        int         _int;
        bool        _bool;
        std::string _str;
        Mat         matrix;
        Object      obj;
        VALUE_Data() {}
        ~VALUE_Data() {}
        explicit VALUE_Data(int val) { _int = val; };
        explicit VALUE_Data(bool val) { _bool = val; };
        explicit VALUE_Data(float val) { fp = val; }
        explicit VALUE_Data(std::string val) { _str = std::move(val); };
        explicit VALUE_Data(Object val) { obj = std::move(val); };
    };
public:
    RT_Value() : type(VOID) {};
    explicit RT_Value(float val);
    explicit RT_Value(bool val);
    explicit RT_Value(int val);
    explicit RT_Value(Mat val);
    explicit RT_Value(std::string val);
    explicit RT_Value(Object val);
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

    template <int _Value_Type>
    inline bool is_type() {
        return this->type == _Value_Type;
    }

    template <int _Value_Type>
    inline bool is_not_type() {
        return this->type != _Value_Type;
    }
};

struct RT_Function {
    std::vector<std::string>        params_name;
    std::vector<RT_Value>           params;
    std::shared_ptr<Block_AST>      block;
    std::shared_ptr<Expression_AST> ret;
};

#endif //COMPILER_VALUE_H
