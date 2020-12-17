//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_EXPRESSION_H
#define COMPILER_EXPRESSION_H
#define PARSER_TEST
#include <memory>
#include <string>
#include <vector>
#include "helper.h"
#include "Parser.h"

class Expression_AST;
class Integer_AST;
class Variable_AST;
class BinaryExprAST;
struct Define_AST;
struct Function_call_AST;
struct If_AST;
struct Else_AST;
struct Block_AST;
struct Assign_AST;
struct Function_AST;
struct While_AST;
struct Function_proto_AST;

using ptr_Function_proto_AST = std::unique_ptr<Function_proto_AST>;
using ptr_Function_call_AST = std::unique_ptr<Function_call_AST>;
using ptr_Function_AST = std::unique_ptr<Function_AST>;
using ptr_Variable_AST = std::unique_ptr<Variable_AST>;
using ptr_Expression_AST = std::unique_ptr<Expression_AST>;
using ptr_BinaryExprAST = std::unique_ptr<BinaryExprAST>;
using ptr_Integer_AST = std::unique_ptr<Integer_AST>;
using ptr_Block_AST = std::unique_ptr<Block_AST>;
using ptr_Else_AST = std::unique_ptr<Else_AST>;
using ptr_Assign_AST = std::unique_ptr<Assign_AST>;
using ptr_If_AST = std::unique_ptr<If_AST>;
using ptr_While_AST = std::unique_ptr<While_AST>;

struct Expression_AST {
    virtual ~Expression_AST() = default;
#ifdef PARSER_TEST
    virtual void print() = 0;
#endif
};

class Integer_AST : public Expression_AST {
public:
    int val;
    Integer_AST(int m_val) : val(m_val) {}
#ifdef PARSER_TEST
    void print() override { printf("[INT_EXP] %d\n", val); }
#endif
};

struct Variable_AST : public Expression_AST {
    raw_string name;
    Variable_AST(raw_string m_name) : name(m_name) {}
#ifdef PARSER_TEST
    void print() override { printf("[VAR_EXP] %.*s\n", (int)name.len, name.content); }
#endif
};

struct Block_AST : public Expression_AST {
    std::vector<ptr_Expression_AST> v_expr;
#ifdef PARSER_TEST
    void print() override {
        printf("[BLOCK]\n");
        if (v_expr.empty()) printf("empty code block!\n");
        for (auto &&expr : v_expr) {
            expr->print();
        }
    }
#endif
};

struct Function_proto_AST : public Expression_AST {
    std::vector<ptr_Variable_AST> args;
#ifdef PARSER_TEST
    void print() override {
        printf("[FUNC_PROTO]\n");
        printf("(\n");
        for (int i=0; i < args.size(); i++) {
            args[i] -> print();
            if (i < args.size()-1) printf(", \n");
        }
        printf(")\n");
    }
#endif
};

struct Function_AST : public Expression_AST {
    ptr_Variable_AST name;
    ptr_Block_AST func_body;
    ptr_Expression_AST return_expr;
    ptr_Function_proto_AST args;
#ifdef PARSER_TEST
    void print() override {
        printf("[FUNC]\n");
        printf("func name: \n");
        name->print();
        args->print();
        printf("func body: \n");
        func_body->print();
        printf("return: \n");
        return_expr->print();
    }
#endif
};

struct Function_call_AST : public Expression_AST {
    raw_string name;
    std::vector<ptr_Expression_AST> args;
#ifdef PARSER_TEST
    void print() override {
        printf("[FUNC_CALL]\n");
        printf("func : %.*s (\n", (int)name.len, name.content);
        for (int i=0; i < args.size(); i++) {
            args[i] -> print();
            if (i < args.size()-1) printf(", \n");
        }
        printf(")\n");
    }
#endif
};



struct UnaryExprAST : public Expression_AST {
    ptr_Expression_AST LHS;
    UnaryExprAST() = default;
    UnaryExprAST(UnaryExprAST&& expr) = default;
    UnaryExprAST(ptr_Expression_AST lhs) :
            LHS(std::move(lhs))
    {}
#ifdef PARSER_TEST
    void print() override {
        printf("[UNARY_EXP]\n");
        LHS->print();
    }
#endif
};

struct BinaryExprAST : public UnaryExprAST {
    ptr_Expression_AST RHS;
    Kind op;
#ifdef PARSER_TEST
    void print() override {
        printf("[LHS_EXP]\n");
        LHS->print();
        printf("[BIN_OP ] %s\n", names_kind[op]);
        printf("[RHS_EXP]\n");
        RHS->print();
    }
#endif
};



struct Define_AST : public Expression_AST {
    ptr_Variable_AST   var;
    ptr_Expression_AST rhs;

    void set_var(raw_string m_name) {
        var = std::make_unique<Variable_AST>(m_name);
    }

    Define_AST() = default;

    Define_AST(ptr_Variable_AST m_var, ptr_Expression_AST m_rhs) :
        var(std::move(m_var)), rhs(std::move(m_rhs))
    {}

#ifdef PARSER_TEST
    void print() override {
        printf("[DEF_STMT]\n");
        var->print();
        printf("=\n");
        rhs->print();
    }
#endif
};

struct Assign_AST : public Define_AST {
#ifdef PARSER_TEST
    void print() override {
        printf("[ASSIGN]\n");
        var->print();
        printf("=\n");
        rhs->print();
    }
#endif
};



struct If_AST : public Expression_AST {
    ptr_Expression_AST cond;
    ptr_Block_AST      if_block;

#ifdef PARSER_TEST
    void print() override {
        printf("[IF_STMT]\n");
        printf("if ");
        cond->print();
        printf(" is equal to 0\n");
        if_block->print();
    }

#endif

    If_AST() = default;

    If_AST(ptr_Expression_AST m_cond, ptr_Block_AST m_if_block) :
            cond(std::move(m_cond)), if_block(std::move(m_if_block))
    {}
};

struct While_AST: public If_AST  {
#ifdef PARSER_TEST
    void print() override {
        printf("[While_STMT]\n");
        printf("While ");
        cond->print();
        printf(" is equal to 0\n");
        if_block->print();
    }
#endif
};

struct Else_AST: public If_AST {
    ptr_Block_AST else_block;

    Else_AST() = default;
    Else_AST(Else_AST&& expr) = default;
    Else_AST(ptr_Expression_AST m_cond, ptr_Block_AST m_if_block, ptr_Block_AST m_else_block) :
            If_AST(std::move(m_cond), std::move(m_if_block)), else_block(std::move(m_else_block))
    {}
#ifdef PARSER_TEST
    void print() override {
        printf("[IF_STMT]\n");
        printf("if ");
        cond->print();
        printf(" is less than 0\n");
        printf("[ELSE]\n");
        else_block->print();
    }
#endif
};



#endif //COMPILER_EXPRESSION_H
