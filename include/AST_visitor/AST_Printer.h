//
// Created by Lin Sinan on 2020-12-21.
//

#ifndef COMPILER_AST_PRINTER_H
#define COMPILER_AST_PRINTER_H
#define INDENT_EACH_STEP 2

#include "Ast.h"

class AST_Printer : public AST_Visitor {
    int cur_indent = -INDENT_EACH_STEP;
public:

    void evaluate(Expression_AST &) override ;

    void visit_def(Define_AST                &) override ;
    void visit_func_proto(Function_proto_AST &) override ;
    void visit_func_call(Function_call_AST   &) override ;
    void visit_func(Function_AST             &) override ;
    void visit_var(Variable_AST              &) override ;
    void visit_binary(Binary_expr_AST        &) override ;
    void visit_int(Integer_AST               &) override ;
    void visit_fp(Float_point_AST            &) override ;
    void visit_block(Block_AST               &) override ;
    void visit_assign(Assign_AST             &) override ;
    void visit_if(If_AST                     &) override ;
    void visit_while(While_AST               &) override ;
    void visit_unary(Unary_expr_AST          &) override ;
};

#endif //COMPILER_AST_PRINTER_H
