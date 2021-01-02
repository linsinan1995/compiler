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
    ptr_While_AST while_expr = std::make_unique<While_AST> ();

    // eat while
    next();

    // eat open paren
    consume(k_open_paren, "handle_while_statement");

    while_expr->cond = std::move(parse_expr());

    consume(k_close_paren, "handle_while_statement");


    if (!(while_expr->while_block = std::move(parse_block())))
        return panic_nptr("Parsing Error: if block is empty!\n");
    return while_expr;
}

ptr_expr Parser::handle_class_statement() {
    ptr_Class_AST class_expr = std::make_unique<Class_AST> ();

    // eat class keyword
    next();
    if (cur_token->kind == k_class_decl) {
        panic("Parsing warning: Overloaded a class!\n");
        cur_token->kind = k_var;
    }

    if (cur_token->kind != k_var)
        return panic_nptr("Parsing Error: The parser expects a k_var, "
                          "but get %s\n", names_kind[cur_token->kind]);

    class_expr->type_name = cur_token->lexeme.to_string();
    lexer->register_class(cur_token.get());

    next();
    // eat open curly
    consume(k_open_curly, "handle_class_statement");

    while (anyone(cur_token->kind, kw_func, kw_def, k_var)) {
        if (cur_token->kind == kw_func)
            class_expr->funcs.emplace_back(handle_def_func_statement());
        else if (cur_token->kind == kw_def)
            class_expr->vars.emplace_back(handle_def_statement());
        else {
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

                class_expr->vars.emplace_back(std::move(assign_stmt));

                if (cur_token->kind == k_semi) {
                    panic("Parsing warning: Do not recommend to use semicolon!\n");
                    next();
                }
            } else {
                panic("Parsing warning: Unknown expression in handle_class_statement!\n");
                parse_expr(); // error handling by assuming it is a invalid expression
            }
        }
    }

    // eat close curly
    consume(k_close_curly, "handle_class_statement");

    return class_expr;
}

// var =  expr
// var == expr
// func(3) op expr ...
// var pic op expr
//    return parse_assign_expr();
ptr_expr Parser::handle_general_statement() {
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
    } else if (cur_token->kind == k_dot) {
        // eat k_dot
        next();
        if (cur_token->kind != k_var) {
            prev_tok = nullptr;
            return panic_nptr("Parsing Error: Unexpected exprssion! "
                              "Parser does not understand the k_dot in this "
                              "expression\n");
        }

        auto dot_var_name = cur_token->lexeme.to_string();
        // eat k_var
        next();

        // check it is a member variable or member function
        if (cur_token->kind == k_open_paren) {
            auto object_call_stmt = std::make_unique<Class_Call_AST> ();
            object_call_stmt->obj_name = prev_tok->lexeme.to_string();
            object_call_stmt->func_name = std::move(dot_var_name);
            // reset prev_tok before parsing expression!
            prev_tok = nullptr;
            object_call_stmt->args = std::move(parse_func_call_expr());

            return object_call_stmt;
        } else {
            auto object_var_stmt = std::make_unique<Class_Var_AST> ();
            object_var_stmt->obj_name = prev_tok->lexeme.to_string();
            object_var_stmt->var_name = std::move(dot_var_name);
            prev_tok = nullptr;
            return object_var_stmt;
        }
    }

    return parse_expr();
}


ptr_expr Parser::handle_class_decl() {
    auto expr = std::make_unique<Class_Decl_AST>();
    expr->class_name = cur_token->lexeme.to_string();

    // eat class name
    next();

    if (cur_token->kind != k_var)
        return panic_nptr("Parsing Error: Missing a k_var in handle_class_decl! "
                          "The parser expects a k_var, but get %s\n",
                          names_kind[cur_token->kind]);

    expr->var_name = cur_token->lexeme.to_string();
    next();
    return expr;
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
{}

// block -> { expr_1 \n expr_2 \n ... expr_3 \n }
ptr_Block_AST Parser::parse_block() {
    auto block = std::make_unique<Block_AST> ();

    // eat open curly
    consume(k_open_curly, "parse_block");

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
    auto proto = std::make_unique<Function_proto_AST> ();
    if (cur_token->kind != k_var) {
        return panic_nptr("Parsing Error: Function name should be a k_var instead of %s! \n",
                          names_kind[cur_token->kind]);
    }

    proto->name = cur_token->lexeme.to_string();
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
        if (cur_token->kind == k_comma) next();
    }

    consume(k_close_paren, "parse_func_proto");
    return proto;
}

// block -> { expr_1 \n expr_2 \n ... expr_3 \n return xxx \n }
ptr_Function_AST Parser::parse_def_func_expr(ptr_Function_proto_AST proto) {
    auto func = std::make_unique<Function_AST> ();

    func->args_with_func_name = std::move(proto);
    func->func_body = std::make_unique<Block_AST> ();

    consume(k_open_curly, "parse_def_func_expr");

    while (!anyone(cur_token->kind, k_close_curly, k_EOF, k_unexpected)) {
        while (cur_token->kind == k_comment) next();

        if (cur_token->kind == kw_return) {
            // eat return
            next();
            if ((func->return_expr = parse_expr())) {
                if (cur_token->kind == k_semi) {
                    panic("Parsing warning: Do not recommend to use semicolon!\n");
                    next();
                }
                break;
            } else return panic_nptr("Parsing Error: Fail to parse "
                                     "return expr in parse_def_func_expr\n");
        }
        func->func_body->v_expr.emplace_back(read_one_statement());

        if (cur_token->kind == k_EOF)
            return panic_nptr("Parsing Error: Fail to parse return expr in parse_def_func_expr! "
                              "The parser expects return key word\n");
    }
    // eat close curly
    consume(k_close_curly, "parse_def_func_expr");

    return func;
}

// if_stmt -> if ( expr ) block
//            if ( expr ) block else block
ptr_expr Parser::handle_if_statement() {
    ptr_If_AST if_expr = std::make_unique<If_AST> ();

    // eat if keyword
    next();
    consume(k_open_paren, "handle_if_statement");

    if (cur_token->kind == k_close_paren)
        return panic_nptr("Parsing Error: if condition is an empty expression in handle_if_statement\n");

    if (!(if_expr->cond = parse_expr()))
        return panic_nptr("Parsing Error: if condition is an empty expression in handle_if_statement\n");

    // eat close paren
    consume(k_close_paren, "handle_if_statement");

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
ptr_Function_AST Parser::handle_def_func_statement() {
    // eat func
    next();
    auto proto  = parse_func_proto();

    auto res = parse_def_func_expr(std::move(proto));

    if (cur_token->kind == k_semi) {
        panic("Parsing warning: Do not recommend to use semicolon!\n");
        next();
    }

    return res;
}

ptr_assign_expr Parser::parse_assign_expr() {
    auto assign_stmt = std::make_unique<Assign_AST> ();

    // check id
    if (cur_token->kind != k_var)
        return panic_nptr("Parsing Error: Unexpected token in parse_assign_expr! "
                          "The parser expects variable, but get %s\n",
                          names_kind[cur_token->kind]);

    assign_stmt->var = std::make_unique<Variable_AST>(cur_token->lexeme);

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
ptr_Define_AST Parser::handle_def_statement() {
    // eat var
    next();
    auto def_expr    = std::make_unique<Define_AST>();
    auto assign_expr = parse_assign_expr();
    def_expr->var = std::move(assign_expr->var);
    def_expr->rhs = std::move(assign_expr->rhs);
    assign_expr = nullptr;

    if (cur_token->kind == k_semi) {
        panic("Parsing warning: Do not recommend to use semicolon!\n");
        next();
    }

    return def_expr;
}

inline
void Parser::consume(Kind kind, const std::string &loc) {
    if (kind == cur_token->kind) next();
    else panic("Parsing Error: Unexpected token in %s! Expecting a %s, but got a %s.\n",
               loc.c_str(), names_kind[kind], names_kind[cur_token->kind]);
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
    auto k = cur_token->kind;
    switch (k) {
        default:
            next();
            return panic_nptr("Parsing Error: Unexpected token in parse_unary_expr! "
                             "The current token is %s\n",
                             names_kind[k]);
        case k_open_bracket:
            return parse_matrix_expr();
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
                auto call_expr = std::make_unique<Function_call_AST> ();
                call_expr->name = cur_token->lexeme.to_string();
                // eat func name
                next();
                call_expr->args = parse_func_call_expr();
                return call_expr;
            }
            return parse_id_expr();
        case k_semi:
            next();
            return panic_nptr("Parsing Warning: Do not use semicolon!\n");
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
    next();

    return args;
}

ptr_expr Parser::parse_atom() {
    if (cur_token->kind == k_open_paren) {
        auto call_expr = std::make_unique<Function_call_AST> ();
        call_expr->name = prev_tok->lexeme.to_string();
        prev_tok = nullptr;

        call_expr->args = std::move(parse_func_call_expr());
        // eat close paren
        return call_expr;
    }
    return std::make_unique<Variable_AST> (prev_tok->lexeme);
}

ptr_expr Parser::parse_expr(int prev_prec) {
    ptr_expr expr;

    if (prev_tok) {
        expr = parse_atom();
        prev_tok = nullptr;
    } else {
        expr = std::move(parse_unary_expr());
        if (!expr) { return nullptr; }
    }

    while (lexer->is_op(cur_token->kind)) {
        Kind op = cur_token->kind;

        int cur_prec = precedence[static_cast<int>(op)];
        if (prev_prec > cur_prec) {
            return expr;
        }

        // eat op
        next();
        auto rhs = parse_expr(cur_prec+1);
        if (!rhs) return nullptr;
        auto bi_expr = std::make_unique<Binary_expr_AST> (std::move(expr),
                                                          op,
                                                          std::move(rhs));
        expr = std::move(bi_expr); // !
    }
    return expr;
}

inline
ptr_Float_point_AST Parser::parse_fp_expr() {
    auto expr = std::make_unique<Float_point_AST> (cur_token->to_fp());
    next();
    return expr;
}

inline
ptr_Integer_AST Parser::parse_int_expr() {
    auto expr = std::make_unique<Integer_AST> (cur_token->to_int());
    next();
    return expr;
}

inline
ptr_STR_AST Parser::parse_string_expr() {
    auto expr = std::make_unique<STR_AST> (cur_token->lexeme);
    next();
    return expr;
}

inline
ptr_Variable_AST Parser::parse_id_expr() {
    auto expr = std::make_unique<Variable_AST> (cur_token->lexeme);
    next();
    return expr;
}

inline
ptr_expr Parser::parse_paren_expr() {
    // eat (
    next();
    auto expr = parse_expr();

    consume(k_close_paren, "parse_paren_expr");
    return expr;
}

ptr_expr Parser::parse_neg_number_expr() {
    // eat negative
    next();

    if (cur_token->kind == k_int) {
        auto expr = std::make_unique<Integer_AST> (-1 * cur_token->to_int());
        next();
        return expr;
    }

    if (cur_token->kind == k_fp) {
        auto expr = std::make_unique<Float_point_AST> (-1 * cur_token->to_int());
        next();
        return expr;
    }

    return panic_nptr("Parsing error : Wrong token type! The parser expects int or fp, "
                      "but get %s" , names_kind[cur_token->kind]);
}

ptr_expr Parser::parse_matrix_expr() {
    consume(k_open_bracket, "parse_matrix_expr");
    auto mat = std::make_unique<Matrix_AST>();

    do {
        while (cur_token->kind == k_open_bracket) {
            mat->values.emplace_back(parse_matrix_expr());
            if (!(mat->values.back())) return panic_nptr("Parsing error: Wrong format of Matrix expression.\n");
        }

        if (cur_token->kind == k_close_bracket) break;

        if (!anyone(cur_token->kind, k_int, k_fp))
            return panic_nptr("Parsing error: Wrong data type of Matrix expression. Got type %s\n",
                              names_kind[cur_token->kind]);

        if (cur_token->kind == k_fp) {
            mat->values.push_back(parse_fp_expr());
        } else if (cur_token->kind == k_int) {
            mat->values.push_back(parse_int_expr());
        }

        if (cur_token->kind == k_close_bracket) break;
        consume(k_comma, "parse_matrix_expr");
    } while (true);

    if (mat->values.empty()) return panic_nptr("Parsing error: Wrong format of Matrix expression.\n");
    consume(k_close_bracket, "parse_matrix_expr");

    mat->dim.push_back(mat->values.size());

    // update dimensions from most nested matrix
    auto inner_matrix = dynamic_cast<Matrix_AST*>(mat->values[0].get());
    if (inner_matrix)
        mat->dim.insert(mat->dim.end(), inner_matrix->dim.begin(), inner_matrix->dim.end());

    if (cur_token->kind == k_comma) next();
    return mat;
}

std::unique_ptr<Parser> Parser::make_parser(const char *code) {
    return code ? std::make_unique<Parser> (Lexer::make_lexer(code)) :
           std::make_unique<Parser> (Lexer::make_lexer());
}

void Parser::read(const char *code) {
    lexer->load(code);
}

ptr_expr Parser::read_one_statement() {
    while (cur_token->kind == k_comment) next();
    ptr_expr expr;

    switch (cur_token->kind) {
        default:           expr = handle_general_statement();  break;
        case kw_class:     expr = handle_class_statement();    break;
        case kw_while:     expr = handle_while_statement();    break;
        case kw_if:        expr = handle_if_statement();       break;
        case k_class_decl: expr = handle_class_decl();         break;
        case kw_func:      expr = handle_def_func_statement(); break;
        case kw_def:       expr = handle_def_statement();      break;
        case k_EOF:        return nullptr;
        case k_unexpected: return panic_nptr("Parsing Error: Unexpected token "
                                             "in read_one_statement!\n");
        }

    if (cur_token->kind == k_semi) {
        panic("Parsing warning: Do not recommend to use semicolon!\n");
        next();
    }

    return expr;
}