/******************************************************************* 
 * 
 *    @Time           @Author       @Description
 * ------------      ---------      -----------
 * 2020-12-16 17:11   Lin Sinan         None
 *
 * @File    :   Parser.cpp
 * @Contact :   mynameisxiaou@gmail.com
 * @github  :   https://github.com/linsinan1995
 * @usage   :   
 * 
 ******************************************************************/

//
// Created by Lin Sinan on 2020-12-16.
//
#include <map>
#include "Parser.h"

using namespace parser_ns;

static std::map<int, int> precedence {
        {op_equal, 30}, {op_gt, 30}, {op_lt, 30}, {op_le, 30}, {op_ge, 30},
        {op_add, 40}, {op_sub, 40},
        {op_mul, 50}, {op_div, 50}, {op_mod, 50},
        {op_pow, 60}
};

ptr_While_AST Parser::handle_while_statement() {
    ptr_While_AST while_expr = std::make_shared<While_AST> ();

    // eat while/if keyword
    next();
    if (cur_token->kind != k_open_paren) return panic_nptr("Parsing Error: Missing an open paren in handle_while_statement! "
                                                           "The parser expects open paren, but get %s\n",
                                                           names_kind[cur_token->kind]);
    // eat open paren
    next();

    while_expr->cond = parse_expr();

    if (cur_token->kind != k_close_paren) return panic_nptr("Parsing Error: Missing an close paren in handle_while_statement! "
                                                            "The parser expects open paren, but get %s\n",
                                                            names_kind[cur_token->kind]);
    // eat close paren
    next();

    if (!(while_expr->while_block = std::move(parse_block())))
        return panic_nptr("Parsing Error: if block is empty!\n");
    return while_expr;
}

// var =  expr
// var == expr
// func(3) op expr ...
// var other op expr
//    return parse_assign_expr();
ptr_expr Parser::handle_statement() {
    if (cur_token->kind != k_var) return parse_expr();

    prev_tok = std::move(cur_token);
    next();

    // assign
    if (cur_token->kind == op_assign) {
        auto assign_stmt = std::make_shared<Assign_AST> ();
        assign_stmt->var = std::make_shared<Variable_AST>(prev_tok->lexeme); // copy
        prev_tok = nullptr;
        // eat =
        next();

        assign_stmt->rhs = std::move(parse_expr());
        return assign_stmt;
    }
    return parse_expr();
}

v_expr_ptr Parser::parse() {
    v_expr_ptr parsed_vec_stmt {};
    ptr_expr   stmt;

    // get the first token
    next();

    while((stmt = read_one_statement())) {
        parsed_vec_stmt.emplace_back(std::move(stmt));
    }

    return parsed_vec_stmt;
}

inline
Parser::Parser(std::unique_ptr<Lexer> lexer) : lexer(std::move(lexer))
    {
    }

// block -> { expr_1 \n expr_2 \n ... expr_3 \n }
ptr_Block_AST Parser::parse_block() {
    auto block = std::make_shared<Block_AST> ();

    if (cur_token->kind != k_open_curly)
        return panic_nptr("Parsing Error: Missing an open curly in parse_block! "
                          "The parser expects open curly, but get %s\n",
                          names_kind[cur_token->kind]);

    // eat open curly
    next();

    while (cur_token->kind != k_close_curly) {
        block->v_expr.emplace_back(read_one_statement());

        if (cur_token->kind == k_EOF)
            return panic_nptr("Parsing Error: Missing an close curly in parse_block! "
                              "The parser expects close curly, but get %s\n",
                              names_kind[cur_token->kind]);
    }

    // eat close curly
    next();
    return block;
}

ptr_Function_proto_AST Parser::parse_func_proto() {
    auto proto = std::make_shared<Function_proto_AST> ();

    proto->name = raw_to_string(cur_token->lexeme);
    // eat func name
    next();

    if (cur_token->kind != k_open_paren)
        return panic_nptr("Parsing Error: Missing open paren in parse_func_proto! "
                          "The parser expects open paren, but get %s\n",
                          names_kind[cur_token->kind]);

    // eat open paren
    next();

    while (!anyone(cur_token->kind, k_close_paren, k_EOF)) {
        proto->args.emplace_back(parse_id_expr());
        next(); // eat id
        if (cur_token->kind == k_comma) next();
    }

    if (cur_token->kind != k_close_paren)
        return panic_nptr("Parsing Error: Missing close paren in parse_func_proto! "
                          "The parser expects close paren, but get %s\n",
                          names_kind[cur_token->kind]);

    return proto;
}

// block -> { expr_1 \n expr_2 \n ... expr_3 \n return xxx \n }
ptr_Function_AST Parser::parse_def_func_expr(ptr_Function_proto_AST proto) {
    auto func = std::make_shared<Function_AST> ();

    func->args_with_func_name = std::move(proto);
    func->func_body = std::make_shared<Block_AST> ();

    if (cur_token->kind != k_open_curly)
        return panic_nptr("Parsing Error: Missing open curly in parse_def_func_expr! "
                          "The parser expects open curly, but get %s\n",
                          names_kind[cur_token->kind]);

    // eat open curly
    next();

    while (!anyone(cur_token->kind, k_close_curly, k_EOF, k_unexpected)) {
        while (cur_token->kind == k_comment) next();

        if (cur_token->kind == kw_return) {
            // eat return
            next();
            if ((func->return_expr = parse_expr()))
                break;
            else
                return panic_nptr("Parsing Error: Fail to parse return expr in parse_def_func_expr\n");
        }
        func->func_body->v_expr.emplace_back(read_one_statement());

        if (cur_token->kind == k_EOF) return panic_nptr("Parsing Error: Fail to parse return expr in parse_def_func_expr! "
                                                        "The parser expects return key word\n");
    }
    // eat close curly
    next();

    return func;
}

// if_stmt -> if ( expr ) block
//            if ( expr ) block else block
ptr_expr Parser::handle_if_statement() {
    ptr_If_AST if_expr = std::make_shared<If_AST> ();

    // eat if keyword
    next();
    if (cur_token->kind != k_open_paren)
        return panic_nptr("Parsing Error: Missing open paren in handle_if_statement! "
                          "The parser expects open paren, but get %s\n",
                          names_kind[cur_token->kind]);
    // eat open paren
    next();
    if (cur_token->kind == k_close_paren)
        return panic_nptr("Parsing Error: if condition is an empty expression in handle_if_statement\n");

    if (!(if_expr->cond = parse_expr()))
        return panic_nptr("Parsing Error: if condition is an empty expression in handle_if_statement\n");

    if (cur_token->kind != k_close_paren)
        return panic_nptr("Parsing Error: Missing close paren in handle_if_statement! "
                          "The parser expects close paren, but get %s\n",
                          names_kind[cur_token->kind]);
    // eat close paren
    next();

    if_expr->if_block = std::move(parse_block());

    // if there is a else block
    if (cur_token->kind == kw_else) {
        // eat else
        next();
        if_expr->else_block = parse_block();
    }

    return if_expr;
}

// def_func_stmt -> func fun_name func_paren block
ptr_expr Parser::handle_def_func_statement() {
    // eat func
    next();
    auto proto  = parse_func_proto();

    // eat close paren
    next();
    return parse_def_func_expr(std::move(proto));
}

ptr_assign_expr Parser::parse_assign_expr() {
    auto assign_stmt = std::make_shared<Assign_AST> ();

    // check id
    if (cur_token->kind != k_var)
        return panic_nptr("Parsing Error: Unexpected token in parse_assign_expr! "
                          "The parser expects variable, but get %s\n",
                          names_kind[cur_token->kind]);

    assign_stmt->var = std::make_shared<Variable_AST>(cur_token->lexeme);

    // eat id
    next();

    if (cur_token->kind != op_assign)
        return panic_nptr("Parsing Error: Unexpected token in parse_assign_expr! "
                          "The parser expects assign symbol ('='), but get %s\n",
                          names_kind[cur_token->kind]);

    // eat =
    next();

    assign_stmt->rhs = parse_expr();
    return assign_stmt;
}

// def_stmt -> var id = expression
inline
ptr_expr Parser::handle_def_statement() {
    // eat var
    next();
    auto def_expr    = std::make_shared<Define_AST>();
    auto assign_expr = parse_assign_expr();
    def_expr->var = std::move(assign_expr->var);
    def_expr->rhs = std::move(assign_expr->rhs);
    assign_expr = nullptr;
    return def_expr;
}

inline
void Parser::next() {
    cur_token = std::move(lexer->next());
}

inline
char Parser::peek() {
    lexer->eat_white_space();
    return lexer->peek();
}

ptr_expr Parser::parse_unary_expr() {
//    std::unique_ptr<Token> tok = prev_tok ? std::move(prev_tok) : std::move(cur_token);
    switch (cur_token->kind) {
        default:
            return panic_nptr("Parsing Error: Unexpected token in parse_unary_expr! "
                             "The current token is %s\n",
                             names_kind[cur_token->kind]);
        case op_sub:
            return parse_neg_number_expr();
        case k_string:
            return parse_string_expr();
        case k_int:
            return parse_int_expr();
        case k_fp:
            return parse_fp_expr();
        case k_open_paren:
            return parse_paren_expr();
        case k_var:
            if (peek() == '(') {
                auto call_expr = std::make_shared<Function_call_AST> ();
                call_expr->name = raw_to_string(cur_token->lexeme);
                // eat func name
                next();
                call_expr->args = parse_func_call_expr();
                return call_expr;
            }
            return parse_id_expr();
    }
}

std::vector<ptr_Expression_AST>  Parser::parse_func_call_expr(){
    auto args = std::vector<ptr_Expression_AST> {};
    // eat open paren
    next();

    while (!anyone(cur_token->kind, k_close_paren, k_EOF)) {
        args.emplace_back(parse_expr());
        if (cur_token->kind == k_comma) next();
    }

    if (cur_token->kind != k_close_paren) {

        return panic_type<std::vector<ptr_Expression_AST>> (
                "Parsing Error: Missing close paren in parse_unary_expr! "
                "The parser expects an close paren, but get %s\n",
                names_kind[cur_token->kind]);
    }

    return args;
}

ptr_expr Parser::parse_atom() {
    if (cur_token->kind == k_open_paren) {
        auto call_expr = std::make_shared<Function_call_AST> ();
        call_expr->name = raw_to_string(prev_tok->lexeme);
        prev_tok = nullptr;

        call_expr->args = parse_func_call_expr();
        // eat close paren
        next();
        return call_expr;
    }
    return std::make_shared<Variable_AST> (prev_tok->lexeme);
}

ptr_expr Parser::parse_expr(int prev_prec) {
    ptr_expr expr;

    if (prev_tok) {
        expr = parse_atom();
        prev_tok = nullptr;
    } else {
        expr = std::make_shared<Unary_expr_AST> (parse_unary_expr());
        if (!expr) return nullptr;
        // eat id
        next();
    }

    while (is_op(cur_token->kind)) {
        Kind op = cur_token->kind;

        int cur_prec = precedence[static_cast<int>(op)];
        if (prev_prec > cur_prec) {
            return expr;
        }

        // eat op
        next();
        auto bi_expr = std::make_shared<Binary_expr_AST> (std::move(expr),
                                                          op,
                                                          parse_expr(cur_prec+1));
        expr = std::move(bi_expr); // !
    }
    return expr;
}

inline
ptr_Float_point_AST Parser::parse_fp_expr() {
    return std::make_shared<Float_point_AST> (get_val_tok_fp(cur_token.get()));
}

inline
ptr_Integer_AST Parser::parse_int_expr() {
    return std::make_shared<Integer_AST> (get_val_tok_int(cur_token.get()));
}

inline
ptr_STR_AST Parser::parse_string_expr() {
    return std::make_shared<STR_AST> (cur_token->lexeme);
}

inline
ptr_Variable_AST Parser::parse_id_expr() {
    return std::make_shared<Variable_AST> (cur_token->lexeme);
}

inline
ptr_expr Parser::parse_paren_expr() {
    // eat (
    next();
    auto expr = parse_expr();

    if (cur_token->kind != k_close_paren)
        return panic_nptr("Parsing Error: Missing close paren in parse_paren_expr! "
                          "The parser expects an close paren, but get %s\n",
                          names_kind[cur_token->kind]);

    return expr;
}

std::unique_ptr<Parser> Parser::make_parser(const char *code) {
    return std::make_unique<Parser> (Lexer::make_lexer(code));
}

void Parser::read_RT(const char *code) {
    lexer->load(code);
}

ptr_expr Parser::read_one_statement() {
    while (cur_token->kind == k_comment) next();

    switch (cur_token->kind) {
        default:
            return handle_statement();
        case kw_while:
            return handle_while_statement();
        case kw_if:
            return handle_if_statement();
        case k_close_paren:
        case k_close_curly:
            return nullptr;
        case k_unexpected:
            if (cur_token->lexeme.content == nullptr) return nullptr;
            return panic_nptr("Parsing Error: Unexpected token in read_one_statement! Token literal %s\n",
                              raw_to_string(cur_token->lexeme).c_str());
        case k_EOF:
            return nullptr;
        case kw_func:
            return handle_def_func_statement();
        case kw_def:
            return handle_def_statement();
    }
}

ptr_expr Parser::parse_neg_number_expr() {
    // eat negative
    next();

    if (cur_token->kind == k_int) {
        return std::make_shared<Integer_AST> (-1 * get_val_tok_int(cur_token.get()));
    }

    if (cur_token->kind == k_fp) {
        return std::make_shared<Float_point_AST> (-1 * get_val_tok_fp(cur_token.get()));
    }

    return panic_nptr("Parsing error : Wrong token type! The parser expects int or fp, "
                      "but get %s" , names_kind[cur_token->kind]);
}