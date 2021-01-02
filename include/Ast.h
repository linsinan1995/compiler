//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_EXPRESSION_H
#define COMPILER_EXPRESSION_H

#include <memory>
#include <string>
#include <vector>
#include <any>
#include <unordered_map>
#include <queue>

// todo
//      AoT codegen
//#include "llvm/ADT/APFloat.h"
//#include "llvm/ADT/STLExtras.h"
//#include "llvm/IR/BasicBlock.h"
//#include "llvm/IR/Constants.h"
//#include "llvm/IR/DerivedTypes.h"
//#include "llvm/IR/Function.h"
//#include "llvm/IR/IRBuilder.h"
//#include "llvm/IR/LLVMContext.h"
//#include "llvm/IR/Module.h"
//#include "llvm/IR/Type.h"
//#include "llvm/IR/Verifier.h"

#include "Interpreter/Runtime.h"
#include "AST_visitor.h"

using ptr_Define_AST         = std::unique_ptr<Define_AST>;
using ptr_Function_proto_AST = std::unique_ptr<Function_proto_AST>;
using ptr_Function_call_AST  = std::unique_ptr<Function_call_AST>;
using ptr_Function_AST       = std::unique_ptr<Function_AST>;
using ptr_Variable_AST       = std::unique_ptr<Variable_AST>;
using ptr_Expression_AST     = std::unique_ptr<Expression_AST>;
using ptr_Binary_expr_AST    = std::unique_ptr<Binary_expr_AST>;
using ptr_Integer_AST        = std::unique_ptr<Integer_AST>;
using ptr_Float_point_AST    = std::unique_ptr<Float_point_AST>;
using ptr_Block_AST          = std::unique_ptr<Block_AST>;
using ptr_Assign_AST         = std::unique_ptr<Assign_AST>;
using ptr_If_AST             = std::unique_ptr<If_AST>;
using ptr_While_AST          = std::unique_ptr<While_AST>;
using ptr_Unary_expr_AST     = std::unique_ptr<Unary_expr_AST>;
using ptr_STR_AST            = std::unique_ptr<STR_AST>;
using ptr_Class_AST          = std::unique_ptr<Class_AST>;
using ptr_Class_Call_AST     = std::unique_ptr<Class_Call_AST>;
using ptr_Class_Var_AST      = std::unique_ptr<Class_Var_AST>;

struct Expression_AST {
    virtual ~Expression_AST() = default;
    virtual void accept(AST_Visitor &visitor) = 0;
};

struct Integer_AST : public Expression_AST {
public:
    int val;
    explicit Integer_AST(int m_val) : val(m_val) {}
    void accept(AST_Visitor &visitor) override;
};

struct STR_AST : public Expression_AST {
public:
    std::string val;
    explicit STR_AST(_string_view m_val) : val(m_val.to_string()) {}
    void accept(AST_Visitor &visitor) override;
};

struct Float_point_AST : public Expression_AST {
public:
    float val;
    explicit Float_point_AST(float m_val) : val(m_val) {}
    void accept(AST_Visitor &visitor) override;
};

struct Matrix_AST : public Expression_AST {
public:
    std::vector<ptr_Expression_AST > values;
    std::vector<int> dim;
    void accept(AST_Visitor &visitor) override;
};

struct Variable_AST : public Expression_AST {
    std::string name;
    explicit Variable_AST(_string_view& m_name) : name(m_name.to_string())
    {}
    void accept(AST_Visitor &visitor) override;
};

struct Block_AST : public Expression_AST {
    std::vector<ptr_Expression_AST> v_expr;

    void accept(AST_Visitor &visitor) override;
};

struct Function_proto_AST : public Expression_AST {
    std::string                   name;
    std::vector<ptr_Variable_AST> args;

    void accept(AST_Visitor &visitor) override;
};

struct Function_AST : public Expression_AST {
    std::unique_ptr<Block_AST>           func_body;
    std::unique_ptr<Expression_AST>      return_expr;
    std::unique_ptr<Function_proto_AST>  args_with_func_name;

    void accept(AST_Visitor &visitor) override;
};

struct Function_call_AST : public Expression_AST {
    std::string name;
    std::vector<ptr_Expression_AST> args;

    void accept(AST_Visitor &visitor) override;
};

struct Unary_expr_AST : public Expression_AST {
    ptr_Expression_AST LHS;
    void accept(AST_Visitor &visitor) override;

    explicit Unary_expr_AST(ptr_Expression_AST lhs) :
        LHS(std::move(lhs))
    {}
};

struct Binary_expr_AST : public Unary_expr_AST {
    ptr_Expression_AST RHS;
    Kind op;
    void accept(AST_Visitor &visitor) override;

    explicit Binary_expr_AST(ptr_Expression_AST lhs, Kind op, ptr_Expression_AST rhs) :
        Unary_expr_AST(std::move(lhs)), op(op), RHS(std::move(rhs))
    {}
};

struct Assign_AST : public Expression_AST {
    ptr_Variable_AST   var;
    ptr_Expression_AST rhs;

    void accept(AST_Visitor &visitor) override;
};

struct Define_AST : public Assign_AST {
    void accept(AST_Visitor &visitor) override;
};

struct If_AST : public Expression_AST {
    ptr_Expression_AST cond;
    ptr_Block_AST      if_block;
    ptr_Block_AST      else_block;

    void accept(AST_Visitor &visitor) override;
};

struct While_AST: public Expression_AST  {
    ptr_Expression_AST cond;
    ptr_Block_AST      while_block;

    void accept(AST_Visitor &visitor) override;
};

struct Class_AST: public Expression_AST  {
    std::string type_name;
    std::vector<ptr_Assign_AST>   vars;
    std::vector<ptr_Function_AST> funcs;

    void accept(AST_Visitor &visitor) override;
};

struct Class_Decl_AST: public Expression_AST  {
    std::string class_name;
    std::string var_name;

    void accept(AST_Visitor &visitor) override;
};

struct Class_Call_AST: public Expression_AST  {
    std::string                     obj_name;
    std::string                     func_name;
    std::vector<ptr_Expression_AST> args;

    void accept(AST_Visitor &visitor) override;
};

struct Class_Var_AST: public Expression_AST {
    std::string  obj_name;
    std::string  var_name;
    void accept(AST_Visitor &visitor) override;
};
#endif //COMPILER_EXPRESSION_H
