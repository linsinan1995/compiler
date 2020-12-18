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

static std::map<int, int> precedence {
        {op_equal, 30}, {op_gt, 30}, {op_lt, 30}, {op_le, 30}, {op_ge, 30},
        {op_add, 40}, {op_sub, 40},
        {op_mul, 50}, {op_div, 50}, {op_mod, 50},
        {op_pow, 60}
};

ptr_While_AST Parser::handle_while_statement() {
    ptr_While_AST while_expr = std::make_unique<While_AST> ();

    // eat while/if keyword
    next();
    if (cur_token->kind != k_open_paren) return LogError("Missing an open paren!\n");
    // eat open paren
    next();

    while_expr->cond = parse_expr();

    if (cur_token->kind != k_close_paren) return LogError("Missing an close paren!\n");
    // eat close paren
    next();

    if (!(while_expr->if_block = std::move(parse_block())))
        return LogError("Em empty if block!\n");
    return while_expr;
}

// var =  expr
// var == expr
// func(3) op expr ...
// var other op expr
//    return parse_assign_expr();
Parser::ptr_expr Parser::handle_statement() {
    if (cur_token->kind != k_var) return parse_expr();

    prev_tok = std::move(cur_token);
    next();

    // assign
    if (cur_token->kind == op_assign) {
        auto assign_stmt = std::make_unique<Assign_AST> ();
        assign_stmt->var = std::make_unique<Variable_AST>(prev_tok->lexeme); // copy
        prev_tok = nullptr;
        // eat =
        next();

        assign_stmt->rhs = std::move(parse_expr());
        return assign_stmt;
    }

//    // open paren -> f(x)
//    if (cur_token->kind == k_open_paren) {
//        auto call_expr = std::make_unique<Function_call_AST> ();
//        call_expr->name = prev_tok->lexeme;
//        prev_tok = nullptr;
//
//        call_expr->args = parse_func_call_expr();
//        return call_expr;
//    }


    return parse_expr();
}

Parser::v_expr_ptr Parser::parse() {
    Parser::v_expr_ptr parsed_vec_stmt {};
    Parser::ptr_expr   stmt;

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
    auto block = std::make_unique<Block_AST> ();

    if (cur_token->kind != k_open_curly)
        LogError("Missing an open curly!\n");

    // eat open curly
    next();

    while (cur_token->kind != k_close_curly) {
        block->v_expr.emplace_back(read_one_statement());

        if (cur_token->kind == k_EOF)
            return LogError("Missing close curly in block parsing!\n");
    }

    // eat close curly
    next();
    return block;
}

ptr_Function_proto_AST Parser::parse_func_proto() {
    auto proto = std::make_unique<Function_proto_AST> ();

    proto->name = cur_token->lexeme;
    // eat func name
    next();

    if (cur_token->kind != k_open_paren)
        return LogError("Missing open paren in func proto parsing\n");

    // eat open paren
    next();

    while (!anyone(cur_token->kind, k_close_paren, k_EOF)) {
        proto->args.emplace_back(parse_id_expr());
        next(); // eat id
        if (cur_token->kind == k_comma) next();
    }

    if (cur_token->kind != k_close_paren)
        return LogError("Missing close paren in func call parsing\n");

    return proto;
}

// block -> { expr_1 \n expr_2 \n ... expr_3 \n return xxx \n }
ptr_Function_AST Parser::parse_def_func_expr(ptr_Function_proto_AST proto) {
    auto func = std::make_unique<Function_AST> ();

    func->args_with_func_name = std::move(proto);
    func->func_body = std::make_unique<Block_AST> ();

    if (cur_token->kind != k_open_curly)
        LogError("Missing an open curly!\n");

    // eat open curly
    next();

    while (cur_token->kind != k_close_curly) {
        if (cur_token->kind == kw_return) {
            // eat return
            next();
            if ((func->return_expr = parse_expr()))
                break;
            else
                return LogError("Fail to parse return expr");
        }
        func->func_body->v_expr.emplace_back(parse_id_expr());

        if (cur_token->kind == k_EOF)
            return LogError("Fail to parse return expr missing close paren\n");
    }
    // eat close curly
    next();

    return func;
}

// if_stmt -> if ( expr ) block
//            if ( expr ) block else block
Parser::ptr_expr Parser::handle_if_statement() {
    ptr_If_AST if_expr = std::unique_ptr<If_AST> (handle_while_statement());
    // if there is a else block
    if (cur_token->kind == kw_else) {
        // eat else
        next();
        return std::make_unique<Else_AST> (std::move(if_expr->cond), std::move(if_expr->if_block), parse_block());
    }

    return if_expr;
}

// def_func_stmt -> func fun_name func_paren block
Parser::ptr_expr Parser::handle_def_func_statement() {
    // eat func
    next();
    auto proto  = parse_func_proto();

    // eat close paren
    next();
    return parse_def_func_expr(std::move(proto));
}

Parser::ptr_assign_expr Parser::parse_assign_expr() {
    auto assign_stmt = std::make_unique<Assign_AST> ();

    // check id
    if (cur_token->kind != k_var)
        throw std::invalid_argument("expect a identifier");
    assign_stmt->var = std::make_unique<Variable_AST>(cur_token->lexeme);

    // eat id
    next();

    if (cur_token->kind != op_assign)
        throw std::invalid_argument("expect a '='");

    // eat =
    next();

    assign_stmt->rhs = parse_expr();
    return assign_stmt;
}

// def_stmt -> var id = expression
inline
Parser::ptr_expr Parser::handle_def_statement() {
    // eat var
    next();
    auto def_expr    = std::make_unique<Define_AST>();
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

Parser::ptr_expr Parser::parse_unary_expr() {
//    std::unique_ptr<Token> tok = prev_tok ? std::move(prev_tok) : std::move(cur_token);
    switch (cur_token->kind) {
        default:
            return LogError("unknown token when expecting an expression");
        case op_sub:
        case k_int:
            return parse_int_expr();
        case k_open_paren:
            return parse_paren_expr();
        case k_var:
            if (peek() == '(') {
                auto call_expr = std::make_unique<Function_call_AST> ();
                call_expr->name = cur_token->lexeme;
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
        LogError("Missing close paren in func call parsing\n");
        return {};
    }

    return args;
}

Parser::ptr_expr Parser::parse_atom() {
    if (cur_token->kind == k_open_paren) {
        auto call_expr = std::make_unique<Function_call_AST> ();
        call_expr->name = prev_tok->lexeme;
        prev_tok = nullptr;

        call_expr->args = parse_func_call_expr();
        // eat close paren
        next();
        return call_expr;
    }
    return std::make_unique<Variable_AST> (prev_tok->lexeme);
}

Parser::ptr_expr Parser::parse_expr(int prev_prec) {
    ptr_expr expr;

    if (prev_tok) {
        expr = parse_atom();
    } else {
        expr = std::make_unique<Unary_expr_AST> (parse_unary_expr());
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
        auto bi_expr = std::make_unique<Binary_expr_AST> ();
        bi_expr->LHS = std::move(expr);
        bi_expr->op = op;
        bi_expr->RHS = parse_expr(cur_prec+1);
        expr = std::move(bi_expr); // !
    }
    return expr;
}


inline
ptr_Integer_AST Parser::parse_int_expr() {
    if (cur_token->kind == op_sub) {
        next();
        return std::make_unique<Integer_AST> (-1 * get_val_tok_fp(cur_token.get()));
    }

    return std::make_unique<Integer_AST> (get_val_tok_fp(cur_token.get()));
}

inline
ptr_Variable_AST Parser::parse_id_expr() {
    return std::make_unique<Variable_AST> (cur_token->lexeme);
}

inline
Parser::ptr_expr Parser::parse_paren_expr() {
    // eat (
    next();
    auto expr = parse_expr();

    if (cur_token->kind != k_close_paren)
        return LogError("Missing a close paren!\n");

    return expr;
}

std::unique_ptr<Parser> Parser::make_parser(const char *code) {
    return std::make_unique<Parser> (Lexer::make_lexer(code));
}

void Parser::read_RT(const char *code) {
    lexer->load(code);
}

Parser::ptr_expr Parser::read_one_statement() {
    printf("%s\n", names_kind[cur_token->kind]);
    if (cur_token->kind == k_comment) next();

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
            if (cur_token->lexeme.content == NULL) return nullptr;
            return LogError(std::string(names_kind[cur_token->kind]) + std::string(" is unexpected!\n"));
        case k_EOF:
            return nullptr;
        case kw_func:
            return handle_def_func_statement();
        case kw_def:
            return handle_def_statement();
    }
}
