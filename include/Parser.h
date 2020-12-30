//
// Created by Lin Sinan on 2020-12-16.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Ast.h"
#include "Lexer.h"

namespace parser_ns {

class Parser;

using v_expr_ptr = std::vector<std::shared_ptr<Expression_AST>>;
using ptr_expr = std::shared_ptr<Expression_AST>;
using expr = Expression_AST;
using ptr_assign_expr = std::shared_ptr<Assign_AST>;

class Parser {
    void consume(Kind, const std::string &);
    void next();
    char peek();

    std::unique_ptr<Token> cur_token;
    std::unique_ptr<Token> prev_tok;
    std::unique_ptr<Lexer> lexer;

    ptr_expr            read_one_statement();

    ptr_Define_AST      handle_def_statement();
    ptr_Function_AST    handle_def_func_statement();
    ptr_expr            handle_if_statement();
    ptr_expr            handle_general_statement();
    ptr_expr            handle_class_statement();
    ptr_expr            handle_class_decl();

    ptr_expr                parse_atom();
    ptr_expr                parse_expr(int prev_prec = 0);
    ptr_expr                parse_unary_expr();
    ptr_Variable_AST        parse_id_expr();
    ptr_Float_point_AST     parse_fp_expr();
    ptr_Integer_AST         parse_int_expr();
    ptr_STR_AST             parse_string_expr();
    ptr_expr                parse_paren_expr();
    ptr_Block_AST           parse_block();
    ptr_assign_expr         parse_assign_expr();
    ptr_While_AST           handle_while_statement();
    v_expr_ptr              parse_func_call_expr();
    ptr_Function_proto_AST  parse_func_proto();
    ptr_Function_AST        parse_def_func_expr(ptr_Function_proto_AST);
    ptr_expr                parse_neg_number_expr();
    ptr_expr                parse_matrix_expr();
public:
    v_expr_ptr  parse();
    void read(const char *code);
    explicit Parser(std::unique_ptr<Lexer>);
    static std::unique_ptr<Parser> make_parser(const char*);
};


}
#endif //COMPILER_PARSER_H
