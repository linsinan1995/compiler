/******************************************************************* 
 * 
 *    @Time           @Author       @Description
 * ------------      ---------      -----------
 * 2020-12-21 16:39   Lin Sinan         None
 *
 * @File    :   AST_Interpreter.cpp
 * @Contact :   mynameisxiaou@gmail.com
 * @github  :   https://github.com/linsinan1995
 * @usage   :   
 * 
 ******************************************************************/

//
// Created by Lin Sinan on 2020-12-21.
//
#include "Interpreter/AST_Interpreter.h"
#define DEBUG_RT_VAR() do { \
                          for (auto [n,v] : rt->sym_table.back().vars)  \
                              std::cout << n << ": " << *v << std::endl; \
                       } while(0);

using namespace runtime_ns;

void AST_Interpreter::visit_var(Variable_AST &expr) {
    reset();
    if (m_object) val = m_object->get_variable(expr.name);

    if (!val)
        val = rt->get_variable(expr.name);
}

void AST_Interpreter::visit_fp(Float_point_AST &expr) {
    reset();
    val = rt->allocator->alloc_var(expr.val);
}

void AST_Interpreter::visit_int(Integer_AST &expr) {
    reset();
    val = rt->allocator->alloc_var(expr.val);
}

void AST_Interpreter::visit_str(STR_AST &expr) {
    reset();
    val = rt->allocator->alloc_var(expr.val);
}

void AST_Interpreter::mat_helper(Matrix_AST &expr, std::vector<float> &data, std::vector<int> &dim) {
    for (int i = 0; i < expr.dim[0]; i++) {
        if (auto inner = dynamic_cast<Float_point_AST*> (expr.values[i].get())) {
            data.push_back(inner->val);
        } else {
            Expression_AST &inner_mat = *(expr.values[i]);
            mat_helper(dynamic_cast<Matrix_AST&> (inner_mat), data, dim);
        }
    }
}

void AST_Interpreter::visit_mat(Matrix_AST &expr) {
    auto dims = std::vector<int> (expr.dim);
    auto data = std::vector<float> {};

    for (int i = 0; i < expr.dim[0]; i++) {
        if (auto inner = dynamic_cast<Float_point_AST*> (expr.values[i].get())) {
            data.push_back(inner->val);
        } else {
            Expression_AST &inner_mat = *(expr.values[i]);
            mat_helper(dynamic_cast<Matrix_AST&> (inner_mat), data, dims);
        }
    }

    Mat mat;
    mat.dim = dims;
    mat.data = data;
    reset();
    val = rt->allocator->alloc_var(std::move(mat));
}

void AST_Interpreter::visit_unary(Unary_expr_AST &expr) {
    expr.LHS->accept(*this);
}

void AST_Interpreter::visit_binary(Binary_expr_AST &expr) {
    reset();
    expr.LHS->accept(*this);
    RT_Value *l_val = val;
    val = nullptr;

    expr.RHS->accept(*this);
    RT_Value *r_val = val;
    val = rt->allocator->alloc_var();

    switch(expr.op) {
        default:
            panic_nptr("Runtime Error: Unexpected operator in visit_binary! Token type is %s\n",
                       names_kind[expr.op]);
            reset();
            break;
        case op_add:
            *val = *l_val + *r_val;
            break;
        case op_sub:
            *val = *l_val - *r_val;
            break;
        case op_mul:
            *val = *l_val * *r_val;
            break;
        case op_div:
            *val = *l_val / *r_val;
            break;
        case op_gt:
            *val = *l_val > *r_val;
            break;
        case op_ge:
            *val = *l_val >= *r_val;
            break;
        case op_lt:
            *val = *l_val < *r_val;
            break;
        case op_le:
            *val = *l_val <= *r_val;
            break;
        case op_pow:
            *val = *l_val * *r_val;
            break;
        case op_equal:
            *val = *l_val == *r_val;
            break;
    }
    // addr of r_val == addr of val
    if (l_val && !l_val->occupied) rt->allocator->dealloc_var(l_val);
    if (r_val && !r_val->occupied) rt->allocator->dealloc_var(r_val);
}

void AST_Interpreter::visit_block(Block_AST &expr) {
    for (auto &expr_entry : expr.v_expr) {
        expr_entry->accept(*this);
    }

    reset();
}


void AST_Interpreter::visit_func_proto(Function_proto_AST &expr) {
    reset();
}

void AST_Interpreter::visit_func_call(Function_call_AST &expr) {
    RT_Function *func;

    if (m_object) func = m_object->get_function(expr.name);
    else          func = rt->get_function(expr.name);

    std::vector<RT_Value*> v_args;
    for (auto &arg : expr.args) {
        arg->accept(*this);
        v_args.push_back(val);
        val = nullptr;
    }

    if (!func) {
        auto f = rt->get_builtin_function(expr.name);

        if (!f) {
            panic("Runtime Error : function %s has not been defined!\n",
                  expr.name.c_str());
            reset();
            return;
        }

        reset();
        val = f(rt.get(), v_args);
        return;
    }

    if (func->params_name.size() != expr.args.size()) {
        panic("Runtime Error : Wrong argument number! func %s required %d, but provided %d\n",
              expr.name.c_str(), func->params_name.size(), expr.args.size());
        reset();
        return;
    }

    rt->creat_call_frame();
    rt->creat_variables(func->params_name, v_args);
    func->block->accept(*this);
    if (func->ret) {
        func->ret->accept(*this);
    } else reset();
    rt->destroy_call_frame();
}

void AST_Interpreter::visit_func(Function_AST &expr) {
    auto func = rt->allocator->alloc_func();
    for (auto & arg : expr.args_with_func_name->args) {
        func->params_name.emplace_back(std::move(arg->name));
    }

    func->block = std::move(expr.func_body);
    func->ret = std::move(expr.return_expr);
//    std::exchange(expr.func_body, nullptr);
//    std::exchange(expr.return_expr, nullptr);
    expr.return_expr = nullptr;
    expr.func_body = nullptr;

    rt->creat_function(expr.args_with_func_name->name, func);
    expr.args_with_func_name = nullptr;
    reset();
}

void AST_Interpreter::visit_if(If_AST &expr) {
    expr.cond->accept(*this);
    RT_Value *cond_res = val;

    if (cond_res->to_bool() && expr.if_block){
        reset();
        expr.if_block->accept(*this);
    } else {
        reset();
        if (expr.else_block) expr.else_block->accept(*this);
    }

    reset();
}

void AST_Interpreter::visit_while(While_AST &expr) {
    expr.cond->accept(*this);

    while (val->to_bool()){
        reset();
        expr.while_block->accept(*this);
        expr.cond->accept(*this);
    }

    reset();
}

void AST_Interpreter::visit_def(Define_AST &expr) {
    expr.rhs->accept(*this);
    val->occupied = true;
    rt->creat_variable(expr.var->name, val);
    val = nullptr;
}

void AST_Interpreter::visit_assign(Assign_AST &expr) {
    expr.rhs->accept(*this); // it also assigns value to rt->val
    if (m_object) {
        auto m_obj_inner_val = m_object->get_variable(expr.var->name);
        if (m_obj_inner_val) {
            m_object->update_variable(expr.var->name, val);
            // deallocate previous value pointer
            // todo:
            //      deallocation strategies for 1.local variable 2.object variable
            //      scope -> from bool to int or bit-mask
            // rt->allocator->dealloc_var(val);
            reset();
            return;
        }
    }

    rt->creat_variable(expr.var->name, val);
    reset();
}

void AST_Interpreter::visit_class(Class_AST &expr) {
    auto entry = std::make_unique<Global_Class_Entry>();

    // functions
    for (auto & f : expr.funcs) {
        RT_Function *mem_f = rt->allocator->alloc_func();
        for (auto & arg : f->args_with_func_name->args) {
            mem_f->params_name.push_back(arg->name);
        }
        mem_f->block = std::move(f->func_body);
        mem_f->ret = std::move(f->return_expr);
        entry->funcs.emplace_back(std::move(f->args_with_func_name->name), mem_f);
    }

    // variables
    for (auto & var : expr.vars) {
        var->rhs->accept(*this);
        val->occupied = true;
        entry->vars.emplace_back(std::move(var->var->name), val);
        val = nullptr;
    }

    rt->class_table.insert(expr.type_name, std::move(entry));
    reset();
}

void AST_Interpreter::visit_class_decl(Class_Decl_AST &expr) {
    Global_Class_Entry *entry = rt->class_table.get(expr.class_name);

    Object obj {};
    obj.type_name = expr.var_name;
    // dump functions of class to the object
    entry->dump_function(&obj);

    // copy variable of class to the object
    for (auto &[name, p_member_var] : entry->vars) {
        RT_Value *member_var = rt->allocator->alloc_var(*p_member_var);
        member_var->occupied = true;
        obj.member_vars[name] = member_var;
    }

    rt->creat_variable(expr.var_name, std::move(obj));

    auto vv = rt->get_variable(expr.var_name);

    reset();
}

void AST_Interpreter::visit_class_var(Class_Var_AST &expr) {
    reset();
    val = rt->get_variable(expr.obj_name);
    if (val->is_type<VOID>()) {
        panic("Runtime Error : Object %s has not been defined!\n",
              expr.obj_name.c_str());
        reset();
        return;
    }

    if (auto res = val->data.obj.get_variable(expr.var_name); !res->is_type<VOID>()) {
        reset();
        val = res;
    } else {
        panic("Runtime Error : member variable %s in %s has not been defined!\n",
              expr.var_name.c_str(), expr.obj_name.c_str());
        reset();
    }
}

void AST_Interpreter::visit_class_call(Class_Call_AST &expr) {
    auto obj = rt->get_variable(expr.obj_name);

    if (obj->is_not_type<OBJECT>()) {
        panic("Runtime Error : Object %s has not been defined!\n",
              expr.obj_name.c_str());
        reset();
        return;
    }

    auto func = obj->data.obj.get_function(expr.func_name);

    if (!func) {
        panic("Runtime Error : function %s in object %s has not been defined!\n",
              expr.func_name.c_str(), expr.obj_name.c_str());
        reset();
        return;
    }

    std::vector<RT_Value*> v_args;

    for (auto &arg : expr.args) {
        arg->accept(*this);
        v_args.push_back(val);
        val = nullptr;
    }

    if (func->params_name.size() != expr.args.size()) {
        panic("Runtime Error : Wrong argument number! function %s required %d, but provided %d\n",
              expr.func_name.c_str(), func->params_name.size(), expr.args.size());
        reset();
        return;
    }

    rt->creat_call_frame();
    rt->creat_variables(func->params_name, v_args);
    this->m_object = &obj->data.obj;

    func->block->accept(*this);
    if (func->ret)
        func->ret->accept(*this);
    else reset();

    this->m_object = nullptr;
    rt->destroy_call_frame();
}

void AST_Interpreter::evaluate(Expression_AST &expr) {
    reset();
    expr.accept(*this);
}

void AST_Interpreter::reset() {
    if (val && !val->occupied) rt->allocator->dealloc_var(val);
    val = nullptr;
}