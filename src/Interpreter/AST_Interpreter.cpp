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

using namespace runtime_ns;

void AST_Interpreter::visit_var(Variable_AST &expr) {
    val = rt->get_variable(expr.name);
}

void AST_Interpreter::visit_fp(Float_point_AST &expr) {
    val = RT_Value(expr.val);
}

void AST_Interpreter::visit_int(Integer_AST &expr) {
    val = RT_Value(expr.val);
}

void AST_Interpreter::visit_str(STR_AST &expr) {
    val = RT_Value(expr.val);
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
    val = RT_Value(mat);
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
            panic_nptr("Runtime Error: Unexpected operator in visit_binary! Token type is %s\n",
                       names_kind[expr.op]);
            reset();
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

    reset();
}


void AST_Interpreter::visit_func_proto(Function_proto_AST &expr) {
    reset();
}

void AST_Interpreter::visit_func_call(Function_call_AST &expr) {
    std::shared_ptr<RT_Function> func = rt->get_function(expr.name);

    std::vector<RT_Value> v_args;
    for (auto &arg : expr.args) {
        arg->accept(*this);
        v_args.push_back(val);
    }

    if (!func) {
        auto f = rt->get_builtin_function(expr.name);

        if (!f) {
            panic("Runtime Error : function %s has not been defined!\n",
                  expr.name.c_str());
            reset();
            return;
        }

        val = f(rt.get(), v_args);
        return;
    }

    if (func->params_name.size() != expr.args.size()) {
        panic("Runtime Error : Wrong argument number! func %s required %d, but provided %d\n",
              expr.name.c_str(), func->params_name.size(), expr.args.size());
        val = RT_Value();
        return;
    }

    rt->creat_context();
    rt->creat_variables(func->params_name, v_args);
    if (this->is_object_call && m_ptr_obj) {
        for (auto &[name, mem_var] : m_ptr_obj->member_vars) {
            rt->creat_variable(name, mem_var);
        }
        for (auto &[name, mem_func] : m_ptr_obj->member_functions) {
            rt->creat_function(name, mem_func);
        }
    }
    func->block->accept(*this);
    if (func->ret)
        func->ret->accept(*this);
    else reset();
    rt->ruin_context();
}

void AST_Interpreter::visit_func(Function_AST &expr) {
    auto func = std::make_shared<RT_Function>() ;
    for (auto & arg : expr.args_with_func_name->args) {
        func->params_name.push_back(arg->name);
    }

    func->block = expr.func_body;
    func->ret = expr.return_expr;

    rt->creat_function(expr.args_with_func_name->name, func);
    reset();
}

void AST_Interpreter::visit_if(If_AST &expr) {
    expr.cond->accept(*this);
    RT_Value cond_res = val;
    if (cond_res.to_bool() && expr.if_block){
        expr.if_block->accept(*this);
    } else {
        if (expr.else_block) expr.else_block->accept(*this);
    }

    reset();
}

void AST_Interpreter::visit_while(While_AST &expr) {
    expr.cond->accept(*this);

    while (val.to_bool()){
        expr.while_block->accept(*this);
    }
    reset();
}

void AST_Interpreter::visit_def(Define_AST &expr) {
    expr.rhs->accept(*this);
    rt->creat_variable(expr.var->name, val);
    reset();
}

void AST_Interpreter::visit_assign(Assign_AST &expr) {
    expr.rhs->accept(*this); // it also assigns value to rt->val
    rt->creat_variable(expr.var->name, val);
    reset();
}

//struct Object {
//    std::string type_name;
//    std::unordered_map<std::string, RT_Value>     member_vars;
//    std::unordered_map<std::string, RT_Function*> member_functions;
//};
void AST_Interpreter::visit_class(Class_AST &expr) {
    auto entry = std::make_unique<Global_Class_Entry>();

    // functions
    for (auto & f : expr.funcs) {
        auto mem_f = std::make_unique<RT_Function> ();
        for (auto & arg : f->args_with_func_name->args) {
            mem_f->params_name.push_back(arg->name);
        }
        mem_f->block = std::move(f->func_body);
        mem_f->ret = std::move(f->return_expr);
        entry->funcs.emplace_back(std::move(f->args_with_func_name->name), std::move(mem_f));
    }

    // variables
    for (auto & var : expr.vars) {
        var->rhs->accept(*this);
        auto mem_var = std::make_unique<RT_Value> (val);
        entry->vars.emplace_back(std::move(var->var->name), std::move(val));
    }

    rt->class_table.insert(expr.type_name, std::move(entry));
    reset();
}

void AST_Interpreter::visit_class_decl(Class_Decl_AST &expr) {
    Global_Class_Entry *entry = rt->class_table.get(expr.class_name);

    Object obj;
    obj.type_name = expr.var_name;

    entry->dump(&obj);

    rt->creat_variable(expr.var_name, RT_Value(std::move(obj)));
    reset();
}

void AST_Interpreter::visit_class_var(Class_Var_AST &expr) {
    val = rt->get_variable(expr.obj_name);
    if (val.is_type<VOID>()) {
        panic("Runtime Error : Object %s has not been defined!\n",
              expr.obj_name.c_str());
        reset();
        return;
    }

    if (auto res = val.data.obj.get_variable(expr.var_name); !res.is_type<VOID>()) {
        val = res;
    } else {
        panic("Runtime Error : member variable %s in %s has not been defined!\n",
              expr.var_name.c_str(), expr.obj_name.c_str());
        reset();
    }
}

void AST_Interpreter::visit_class_call(Class_Call_AST &expr) {
    auto obj = rt->get_variable(expr.obj_name);
    this->m_ptr_obj = &obj.data.obj;
    if (obj.is_not_type<OBJECT>()) {
        panic("Runtime Error : Object %s has not been defined!\n",
              expr.obj_name.c_str());
        reset();
        return;
    }

    auto func = obj.data.obj.get_function(expr.func_name);

    if (!func) {
        panic("Runtime Error : function %s in object %s has not been defined!\n",
              expr.func_name.c_str(), expr.obj_name.c_str());
        reset();
        return;
    }

    std::vector<RT_Value> v_args, obj_args;


    for (auto &arg : expr.args) {
        arg->accept(*this);
        v_args.push_back(val);
    }

    if (func->params_name.size() != expr.args.size()) {
        panic("Runtime Error : Wrong argument number! function %s required %d, but provided %d\n",
              expr.func_name.c_str(), func->params_name.size(), expr.args.size());
        reset();
        return;
    }

    rt->creat_context();
    rt->creat_variables(func->params_name, v_args);
    this->is_object_call = true;
    for (auto &[name, mem_var] : m_ptr_obj->member_vars) {
        rt->creat_variable(name, mem_var);
    }
    for (auto &[name, mem_func] : m_ptr_obj->member_functions) {
        rt->creat_function(name, mem_func);
    }
    func->block->accept(*this);
    if (func->ret)
        func->ret->accept(*this);
    else {
        reset();
    }
    this->is_object_call = false;
    this->m_ptr_obj = nullptr;
    rt->ruin_context();
}

void AST_Interpreter::evaluate(Expression_AST &expr) {
    reset();
    expr.accept(*this);
}

void AST_Interpreter::reset() {
    val.type = VOID;
}