/******************************************************************* 
 * 
 *    @Time           @Author       @Description
 * ------------      ---------      -----------
 * 2020-12-21 19:39   Lin Sinan         None
 *
 * @File    :   AST_Printer.cpp
 * @Contact :   mynameisxiaou@gmail.com
 * @github  :   https://github.com/linsinan1995
 * @usage   :
 *
 ******************************************************************/

//
// Created by Lin Sinan on 2020-12-21.
//

#include <string>

#include "AST_visitor/AST_Printer.h"

// control the indentation by RAII
struct Indent {
    explicit Indent(int &level) : level(level) { level += INDENT_EACH_STEP; }
    std::string get_indent() { return std::string(level, ' '); }
    ~Indent() { level -= INDENT_EACH_STEP; }
    int &level;
};

void AST_Printer::visit_var(Variable_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[VAR_EXP] " << expr.name << "\n";
}

void AST_Printer::visit_str(STR_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[STRING]" << expr.val << "\n";
}

void AST_Printer::visit_fp(Float_point_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[FP_EXP] " << expr.val << "\n";
}

void AST_Printer::visit_int(Integer_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[INT_EXP] " << expr.val << "\n";
}

void AST_Printer::visit_unary(Unary_expr_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[UNARY_EXP]\n";
    expr.LHS->accept(*this);
}

void AST_Printer::visit_block(Block_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[BLOCK]\n";
    if (expr.v_expr.empty()) os << ind.get_indent() << "empty code block!\n";
    for (auto &expr_entry : expr.v_expr) {
        expr_entry->accept(*this);
    }
}

void AST_Printer::visit_binary(Binary_expr_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[LHS_EXP]\n";
    expr.LHS->accept(*this);
    os << ind.get_indent() << "[BIN_OP ] " << names_kind[expr.op] << "\n";
    os << ind.get_indent() << "[RHS_EXP]\n";
    expr.RHS->accept(*this);
}

void AST_Printer::visit_func_proto(Function_proto_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "func name: " << expr.name << "(\n";
    for (int i=0; i < expr.args.size(); i++) {
        expr.args[i] -> accept(*this);
        if (i < expr.args.size()-1) os << ind.get_indent() << ", \n";
    }
    os << ind.get_indent() << ")\n";
}

void AST_Printer::visit_func_call(Function_call_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[FUNC_CALL]\n";
    os << ind.get_indent() << "func : " << expr.name <<  " (\n";
    for (int i=0; i < expr.args.size(); i++) {
        expr.args[i] -> accept(*this);
        if (i < expr.args.size()-1) os << ind.get_indent() << ", \n";
    }
    os << ind.get_indent() << ")\n";
}

void AST_Printer::visit_func(Function_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[FUNC]\n";
    expr.args_with_func_name->accept(*this);
    os << ind.get_indent() << "func body: \n";
    expr.func_body->accept(*this);
    os << ind.get_indent() << "return: \n";
    expr.return_expr->accept(*this);
}

void AST_Printer::visit_if(If_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[IF_STMT]\n";
    os << ind.get_indent() << "if ";
    expr.cond->accept(*this);
    os << ind.get_indent() << " is not 0\n";
    expr.if_block->accept(*this);
    if (expr.else_block) {
        os << ind.get_indent() << "[ELSE]\n";
        expr.else_block->accept(*this);
    }
}

void AST_Printer::visit_while(While_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[While_STMT]\n";
    os << ind.get_indent() << "While ";
    expr.cond->accept(*this);
    os << ind.get_indent() << " is not 0\n";
    expr.while_block->accept(*this);
}

void AST_Printer::visit_def(Define_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[DEF_STMT]\n";
    expr.var->accept(*this);
    os << ind.get_indent() << "=\n";
    expr.rhs->accept(*this);
}

void AST_Printer::visit_assign(Assign_AST &expr) {
    Indent ind(cur_indent);
    os << ind.get_indent() << "[ASSIGN]\n";
    expr.var->accept(*this);
    os << ind.get_indent() << "=\n";
    expr.rhs->accept(*this);
}

void AST_Printer::evaluate(Expression_AST &expr) {
    expr.accept(*this);
}