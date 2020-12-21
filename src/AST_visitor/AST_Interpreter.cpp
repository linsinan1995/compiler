/******************************************************************* 
 * 
 *    @Time           @Author       @Description
 * ------------      ---------      -----------
 * 2020-12-21 16:39   Lin Sinan         None
 *
 * @File    :   AST_Interpreter.py    
 * @Contact :   mynameisxiaou@gmail.com
 * @github  :   https://github.com/linsinan1995
 * @usage   :   
 * 
 ******************************************************************/

//
// Created by Lin Sinan on 2020-12-21.
//

#include "AST_visitor/AST_Interpreter.h"

using namespace runtime_ns;

void AST_Interpreter::visit_var(Variable_AST &expr) {
    val = rt->get_variable(raw_to_string(expr.name));
}

void AST_Interpreter::visit_fp(Float_point_AST &expr) {
    val = RT_Value(FP, expr.val);
}

void AST_Interpreter::visit_int(Integer_AST &expr) {
    val = RT_Value(INT, expr.val);
}

void AST_Interpreter::visit_unary(Unary_expr_AST &expr) {
    expr.LHS->accept(*this);
}

void AST_Interpreter::visit_binary(Binary_expr_AST &expr) {
    expr.LHS->accept(*this);
    RT_Value l_val = val;

    expr.RHS->accept(*this);
    RT_Value r_val = val;

    switch(expr.op) {
        default:
            LogError(std::string("Unexpected operator " + std::string(names_kind[expr.op])));
            val = RT_Value();
            break;
        case op_add:
            val = l_val + r_val;
            break;
        case op_sub:
            val = l_val - r_val;
            break;
        case op_mul:
            val = l_val * r_val;
            break;
        case op_div:
            val = l_val / r_val;
            break;
        case op_gt:
            val = l_val > r_val;
            break;
        case op_ge:
            val = l_val >= r_val;
            break;
        case op_lt:
            val = l_val < r_val;
            break;
        case op_le:
            val = l_val <= r_val;
            break;
        case op_pow:
            val = l_val * r_val;
            break;
        case op_equal:
            val = l_val == r_val;
            break;
    }

}

void AST_Interpreter::visit_block(Block_AST &expr) {
    for (auto &expr_entry : expr.v_expr) {
        expr_entry->accept(*this);
    }

    val = RT_Value();
}


void AST_Interpreter::visit_func_proto(Function_proto_AST &expr) {
    val = RT_Value();
}

void AST_Interpreter::visit_func_call(Function_call_AST &expr) {
    std::shared_ptr<RT_Function> func = rt->get_function(raw_to_string(expr.name));

    std::vector<RT_Value> v_args;

    for (auto &arg : expr.args) {
        arg->accept(*this);
        v_args.emplace_back(val);
    }

    if (!func) {
        auto f = rt->get_builtin_function(raw_to_string(expr.name));

        if (!f) {
            panic("Runtime Error : function %s has not been define!\n",
                  raw_to_string(expr.name).c_str());
            val = RT_Value();
            return;
        }

        val = f(rt.get(), std::move(v_args));
        return;
    }

    if (func->params_name.size() != expr.args.size()) {
        panic("Runtime Error : wrong argument number! func %s required %d, but provided %d\n",
              raw_to_string(expr.name).c_str(), func->params_name.size(), expr.args.size());
        val = RT_Value();
        return;
    }

    rt->creat_context();
    rt->creat_variables(func->params_name, std::move(v_args));
    func->block->accept(*this);
    func->ret->accept(*this);
    rt->ruin_context();
}


void AST_Interpreter::visit_func(Function_AST &expr) {
    auto func = std::make_shared<RT_Function>() ;

    for (auto & arg : expr.args_with_func_name->args) {
        func->params_name.push_back(raw_to_string(arg->name));
    }

    func->block = std::move(expr.func_body);
    func->ret = std::move(expr.return_expr);

    rt->creat_function(raw_to_string(expr.args_with_func_name->name), func);
    val = RT_Value();
}

void AST_Interpreter::visit_if(If_AST &expr) {
    expr.cond->accept(*this);
    RT_Value cond_res = val;
    if (cond_res.to_bool() && expr.if_block){
        expr.if_block->accept(*this);
    } else {
        if (expr.else_block) expr.else_block->accept(*this);
    }

    val = RT_Value();
}

void AST_Interpreter::visit_while(While_AST &expr) {
    expr.cond->accept(*this);

    while (val.to_bool()){
        expr.while_block->accept(*this);
    }
    val = RT_Value();
}


void AST_Interpreter::visit_def(Define_AST &expr) {
    expr.rhs->accept(*this);
    rt->creat_variable(raw_to_string(expr.var->name), val);
}

void AST_Interpreter::visit_assign(Assign_AST &expr) {
    expr.rhs->accept(*this);
    rt->creat_variable(raw_to_string(expr.var->name), val);
}

void AST_Interpreter::evaluate(Expression_AST &expr) {
    expr.accept(*this);
}

