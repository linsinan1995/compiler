//
// Created by Lin Sinan on 2020-12-21.
//

#ifndef COMPILER_AST_VISITOR_H
#define COMPILER_AST_VISITOR_H

#include <memory>

struct Expression_AST;
struct Float_point_AST;
struct STR_AST;
struct Variable_AST;
struct Binary_expr_AST;
struct Define_AST;
struct Function_call_AST;
struct If_AST;
struct Block_AST;
struct Assign_AST;
struct Function_AST;
struct While_AST;
struct Function_proto_AST;
struct Unary_expr_AST;
struct Integer_AST;
struct Matrix_AST;

class AST_Visitor {
public:
    virtual void evaluate(Expression_AST &) = 0;

    virtual void visit_mat(Matrix_AST                &) = 0;
    virtual void visit_def(Define_AST                &) = 0;
    virtual void visit_func_proto(Function_proto_AST &) = 0;
    virtual void visit_func_call(Function_call_AST   &) = 0;
    virtual void visit_func(Function_AST             &) = 0;
    virtual void visit_var(Variable_AST              &) = 0;
    virtual void visit_str(STR_AST                   &) = 0;
    virtual void visit_binary(Binary_expr_AST        &) = 0;
    virtual void visit_int(Integer_AST               &) = 0;
    virtual void visit_fp(Float_point_AST            &) = 0;
    virtual void visit_block(Block_AST               &) = 0;
    virtual void visit_assign(Assign_AST             &) = 0;
    virtual void visit_if(If_AST                     &) = 0;
    virtual void visit_while(While_AST               &) = 0;
    virtual void visit_unary(Unary_expr_AST          &) = 0;
};


#endif //COMPILER_AST_VISITOR_H
