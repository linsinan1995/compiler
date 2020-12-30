//
// Created by Lin Sinan on 2020-12-21.
//

#ifndef COMPILER_AST_INTERPRETER_H
#define COMPILER_AST_INTERPRETER_H

#include "Interpreter/Runtime.h"
#include "Ast.h"

class AST_Interpreter : public AST_Visitor {
public:
    std::shared_ptr<runtime_ns::Runtime> rt;
    bool is_object_call = false;
    Object *m_ptr_obj = nullptr;
    RT_Value val;
    void reset();
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
    void visit_str(STR_AST                   &) override ;
    void visit_mat(Matrix_AST                &) override ;
    void visit_class(Class_AST               &) override ;
    void visit_class_decl(Class_Decl_AST     &) override ;
    void visit_class_var(Class_Var_AST       &) override ;
    void visit_class_call(Class_Call_AST     &) override ;

    void mat_helper(Matrix_AST &, std::vector<float>&, std::vector<int>&);
    bool is_null() { return val.is_type<VOID>(); }

    AST_Interpreter() : rt(runtime_ns::Runtime::make_runtime()),
                        val(RT_Value())
    {}
};

#endif //COMPILER_AST_INTERPRETER_H
