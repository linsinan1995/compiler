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
    std::vector<int>   dim;
};

struct Object {
    std::string type_name;
    RT_Function* get_function(const std::string&);
    RT_Value*    get_variable(const std::string&);

    std::unordered_map<std::string, RT_Value*>     member_vars {};
    std::unordered_map<std::string, RT_Function*>  member_functions {};
    void update_variable(const std::string&, RT_Value*);
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
        explicit VALUE_Data(int val)         :  _int(val)           { }
        explicit VALUE_Data(bool val)        : _bool(val)           { }
        explicit VALUE_Data(float val)       : fp(val)              { }
        explicit VALUE_Data(std::string val) : _str(std::move(val)) { }
        explicit VALUE_Data(Object val)      : obj(std::move(val))  { }
    };
public:
    RT_Value() : type(VOID) {};
    explicit RT_Value(float);
    explicit RT_Value(bool);
    explicit RT_Value(int);
    explicit RT_Value(Mat);
    explicit RT_Value(std::string);
    explicit RT_Value(Object);

    RT_Value(const RT_Value&);
    RT_Value(RT_Value&&) noexcept ;
    RT_Value &operator=(RT_Value);

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

    bool to_bool();
    template <int _Value_Type> bool is_type()     { return this->type == _Value_Type; }
    template <int _Value_Type> bool is_not_type() { return this->type != _Value_Type; }
public:
    Value_Type  type;
    VALUE_Data  data;
    bool        occupied = false;
};

struct RT_Function {
    std::vector<std::string>        params_name;
    std::shared_ptr<Block_AST>      block;
    std::shared_ptr<Expression_AST> ret;
};

#endif //COMPILER_VALUE_H
