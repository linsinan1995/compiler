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

inline void Integer_AST::accept(AST_Visitor &visitor) { visitor.visit_int(*this); }
inline void Float_point_AST::accept(AST_Visitor &visitor) { visitor.visit_fp(*this); }
inline void Variable_AST::accept(AST_Visitor &visitor) { visitor.visit_var(*this); }
inline void Function_proto_AST::accept(AST_Visitor &visitor) { visitor.visit_func_proto(*this);}
inline void Block_AST::accept(AST_Visitor &visitor) { visitor.visit_block(*this); }
inline void Unary_expr_AST::accept(AST_Visitor &visitor) { visitor.visit_unary(*this); }
inline void Binary_expr_AST::accept(AST_Visitor &visitor) { visitor.visit_binary(*this); }
inline void Assign_AST::accept(AST_Visitor &visitor) { visitor.visit_assign(*this); }
inline void Define_AST::accept(AST_Visitor &visitor) { visitor.visit_def(*this); }
inline void If_AST::accept(AST_Visitor &visitor) { visitor.visit_if(*this); }
inline void While_AST::accept(AST_Visitor &visitor) { visitor.visit_while(*this); }
inline void Function_call_AST::accept(AST_Visitor &visitor) { visitor.visit_func_call(*this); }
inline void Function_AST::accept(AST_Visitor &visitor) { visitor.visit_func(*this); }