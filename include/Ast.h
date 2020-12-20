//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_EXPRESSION_H
#define COMPILER_EXPRESSION_H
#define PARSER_TEST
#include <memory>
#include <string>
#include <vector>
#include <any>
#include <unordered_map>
#include <queue>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include "Runtime.h"

struct Expression_AST;
struct Float_point_AST;
struct Variable_AST;
struct Binary_expr_AST;
struct Define_AST;
struct Function_call_AST;
struct If_AST;
struct Else_AST;
struct Block_AST;
struct Assign_AST;
struct Function_AST;
struct While_AST;
struct Function_proto_AST;
struct Unary_expr_AST;

using ptr_Define_AST         = std::unique_ptr<Define_AST>;
using ptr_Function_proto_AST = std::unique_ptr<Function_proto_AST>;
using ptr_Function_call_AST  = std::unique_ptr<Function_call_AST>;
using ptr_Function_AST       = std::unique_ptr<Function_AST>;
using ptr_Variable_AST       = std::unique_ptr<Variable_AST>;
using ptr_Expression_AST     = std::unique_ptr<Expression_AST>;
using ptr_BinaryExprAST      = std::unique_ptr<Binary_expr_AST>;
using ptr_Integer_AST        = std::unique_ptr<Float_point_AST>;
using ptr_Block_AST          = std::unique_ptr<Block_AST>;
using ptr_Else_AST           = std::unique_ptr<Else_AST>;
using ptr_Assign_AST         = std::unique_ptr<Assign_AST>;
using ptr_If_AST             = std::unique_ptr<If_AST>;
using ptr_While_AST          = std::unique_ptr<While_AST>;
using ptr_Unary_expr_AST     = std::unique_ptr<Unary_expr_AST>;

struct Expression_AST {
    virtual ~Expression_AST() = default;

    virtual runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &) = 0;
    virtual llvm::Value *codegen() = 0;
    virtual void print() = 0;
};


struct Integer_AST : public Expression_AST {
public:
    int val;
    explicit Integer_AST(int m_val) : val(m_val) {}

    void print() override;
    llvm::Value* codegen() override;
};


struct Float_point_AST : public Expression_AST {
public:
    float val;
    explicit Float_point_AST(float m_val) : val(m_val) {}

    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &rt) override;
    void print() override;
    llvm::Value* codegen() override;
};

struct Variable_AST : public Expression_AST {
    raw_string name;
    explicit Variable_AST(raw_string m_name) : name(m_name) {}

    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &rt) override;
    void print() override;
    llvm::Value* codegen() override;
};

struct Block_AST : public Expression_AST {
    std::vector<ptr_Expression_AST> v_expr;

    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &rt) override;
    void print() override;
    llvm::Value* codegen() override;
};

struct Function_proto_AST : public Expression_AST {
    raw_string                    name;
    std::vector<ptr_Variable_AST> args;

    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &rt) override { return runtime_ns::RT_Value(); }
    void print() override;
    llvm::Function * codegen() override;
};

struct Function_AST : public Expression_AST {
    ptr_Block_AST           func_body;
    ptr_Expression_AST      return_expr;
    ptr_Function_proto_AST  args_with_func_name;

    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &rt) override;
    void print() override;
    llvm::Value* codegen() override;
};

struct Function_call_AST : public Expression_AST {
    raw_string name;
    std::vector<ptr_Expression_AST> args;

    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &rt) override;
    void print() override;
    llvm::Value* codegen() override;
};

struct Unary_expr_AST : public Expression_AST {
    ptr_Expression_AST LHS;

    Unary_expr_AST() = default;
    Unary_expr_AST(Unary_expr_AST&& expr) = default;
    explicit Unary_expr_AST(ptr_Expression_AST lhs) :
            LHS(std::move(lhs))
    {}
    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &rt) override;
    void print() override ;
    llvm::Value* codegen() override;
};

struct Binary_expr_AST : public Unary_expr_AST {
    ptr_Expression_AST RHS;
    Kind op;

    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &rt) override;
    void print() override;
    llvm::Value* codegen() override;
};

struct Assign_AST : public Expression_AST {
    ptr_Variable_AST   var;
    ptr_Expression_AST rhs;

    void print() override;
    llvm::Value *codegen() override;
    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &) override;
};

struct Define_AST : public Assign_AST {
    llvm::Value* codegen() override;
    void print() override;
    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &) override;

};

struct If_AST : public Expression_AST {
    ptr_Expression_AST cond;
    ptr_Block_AST      if_block;
    ptr_Block_AST      else_block;

    llvm::Value* codegen() override;
    void print() override;
    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &) override;
};

struct While_AST: public Expression_AST  {
    ptr_Expression_AST cond;
    ptr_Block_AST      while_block;

    runtime_ns::RT_Value eval(const std::shared_ptr<runtime_ns::Runtime> &) override ;
    llvm::Value* codegen() override;
    void print() override;
};

#endif //COMPILER_EXPRESSION_H
