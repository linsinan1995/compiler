//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Ast.h"
#include "Lexer.h"

class Parser {
    using v_expr_ptr = std::vector<std::unique_ptr<Expression_AST>>;
    using ptr_expr = std::unique_ptr<Expression_AST>;
    using expr = Expression_AST;
    using ptr_assign_expr = std::unique_ptr<Assign_AST>;
    std::unique_ptr<Lexer> lexer;
    void next();
    char peek();
public:

    Parser() = delete;
    explicit Parser(std::unique_ptr<Lexer>);
    std::unique_ptr<Token> cur_token;
    v_expr_ptr parse();
    ptr_expr parse_one_statement();
    ptr_assign_expr parse_assign_expr();
    ptr_While_AST parse_while_expr();
    std::vector<ptr_Expression_AST>  parse_func_call_expr();
    ptr_Function_AST parse_def_func_expr(ptr_Variable_AST, ptr_Function_proto_AST);
    ptr_Function_proto_AST parse_func_proto();
    ptr_expr handle_def_statement();
    ptr_expr handle_def_func_statement();
    ptr_expr handle_if_statement();
    ptr_expr handle_statement();
    ptr_Block_AST parse_block();
    ptr_expr parse_expr(int prev_prec = 0);
    ptr_expr parse_unary_expr();
    ptr_Variable_AST parse_id_expr();
    ptr_Integer_AST parse_int_expr();
    ptr_expr parse_paren_expr();

    static std::unique_ptr<Parser> make_parser(const char *code);

};



#endif //COMPILER_PARSER_H
