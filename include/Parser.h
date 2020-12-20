//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Ast.h"
#include "Lexer.h"

namespace parser_ns {

class Parser;

using v_expr_ptr = std::vector<std::unique_ptr<Expression_AST>>;
using ptr_expr = std::unique_ptr<Expression_AST>;
using expr = Expression_AST;
using ptr_assign_expr = std::unique_ptr<Assign_AST>;

class Parser {
    void next();
    char peek();
    Parser() = delete;

public:
    std::unique_ptr<Token> cur_token;
    std::unique_ptr<Token> prev_tok;
    std::unique_ptr<Lexer> lexer;
public:
    v_expr_ptr  parse();
    ptr_expr    read_one_statement();
public:
    ptr_expr            handle_def_statement();
    ptr_expr            handle_def_func_statement();
    ptr_expr            handle_if_statement();
    ptr_expr            handle_statement();

    ptr_expr                parse_atom();
    ptr_expr                parse_expr(int prev_prec = 0);
    ptr_expr                parse_unary_expr();
    ptr_Variable_AST        parse_id_expr();
    ptr_Integer_AST         parse_int_expr();
    ptr_expr                parse_paren_expr();
    ptr_Block_AST           parse_block();
    ptr_assign_expr         parse_assign_expr();
    ptr_While_AST           handle_while_statement();
    v_expr_ptr              parse_func_call_expr();
    ptr_Function_proto_AST  parse_func_proto();
    ptr_Function_AST        parse_def_func_expr(ptr_Function_proto_AST);
public:
    void read_RT(const char *code);
    Parser(std::unique_ptr<Lexer>);
    static std::unique_ptr<Parser> make_parser(const char *code);
};


}
#endif //COMPILER_PARSER_H
