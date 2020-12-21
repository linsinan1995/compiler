//
// Created by Lin Sinan on 2020-12-21.
//

#ifndef COMPILER_AST_INTERPRETER_H
#define COMPILER_AST_INTERPRETER_H

#include "Runtime.h"
#include "Ast.h"

using namespace runtime_ns;

class AST_Interpreter : public AST_Visitor {
public:
    std::shared_ptr<Runtime> rt;
    RT_Value val;

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

    bool is_null() { return val.is_type<VOID>(); }
    AST_Interpreter() : rt(Runtime::make_runtime()),
                        val(RT_Value())
    {}
};

#endif //COMPILER_AST_INTERPRETER_H
