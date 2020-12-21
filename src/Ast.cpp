/******************************************************************* 
 * 
 *    @Time           @Author       @Description
 * ------------      ---------      -----------
 * 2020-12-17 17:28   Lin Sinan         None
 *
 * @File    :   Ast.cpp
 * @Contact :   mynameisxiaou@gmail.com
 * @github  :   https://github.com/linsinan1995
 * @usage   :   
 * 
 ******************************************************************/

//
// Created by Lin Sinan on 2020-12-17.
//

#include "Ast.h"

void Integer_AST::accept(AST_Visitor &visitor) { visitor.visit_int(*this); }
void Float_point_AST::accept(AST_Visitor &visitor) { visitor.visit_fp(*this); }
void Variable_AST::accept(AST_Visitor &visitor) { visitor.visit_var(*this); }
void Function_proto_AST::accept(AST_Visitor &visitor) { visitor.visit_func_proto(*this);}
void Block_AST::accept(AST_Visitor &visitor) { visitor.visit_block(*this); }
void Unary_expr_AST::accept(AST_Visitor &visitor) { visitor.visit_unary(*this); }
void Binary_expr_AST::accept(AST_Visitor &visitor) { visitor.visit_binary(*this); }
void Assign_AST::accept(AST_Visitor &visitor) { visitor.visit_assign(*this); }
void Define_AST::accept(AST_Visitor &visitor) { visitor.visit_def(*this); }
void If_AST::accept(AST_Visitor &visitor) { visitor.visit_if(*this); }
void While_AST::accept(AST_Visitor &visitor) { visitor.visit_while(*this); }
void Function_call_AST::accept(AST_Visitor &visitor) { visitor.visit_func_call(*this); }
void Function_AST::accept(AST_Visitor &visitor) { visitor.visit_func(*this); }