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
#include <iostream>
#include <Ast.h>
#include "Ast.h"

using namespace llvm;

static std::unique_ptr<llvm::LLVMContext> context = std::make_unique<llvm::LLVMContext>();
static std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("jit", *context);
static std::unique_ptr<llvm::IRBuilder<>> builder = std::make_unique<llvm::IRBuilder<>>(*context);;
static std::map<std::string, llvm::Value *> name_to_pvalue;

// codegen VIRTUAL FUNCTION IMPLEMENTATION
Value *Float_point_AST::codegen() {
    return ConstantFP::get(*context, APFloat(val));
}

// codegen VIRTUAL FUNCTION IMPLEMENTATION
Value *Integer_AST::codegen() {
    return ConstantInt::get(*context, APInt(32, val, true)); // APInt(/*nbits*/32, val, /*bool is_signed*/true));
}

Value *Variable_AST::codegen() {
    // Look this variable up in the function.
    Value *V = name_to_pvalue[std::string(name.content, name.len)];

    return V ? V : LogError("Unknown variable name!\n");
}

Value *Block_AST::codegen() {
    return nullptr;
}

Value *Function_AST::codegen() {
    // check if it is declared / externed
    Function *func = module->getFunction(raw_to_string(args_with_func_name->name));

    if (!func)
        func = args_with_func_name->codegen();

    if (!func)
        return nullptr;

    if (!func->empty())
        return LogError("RT_Function cannot be redefined.");

    // Create a new basic block to start insertion into.
    BasicBlock *basic_block = BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(basic_block); // insert new block to the end of builder

    // Record the function arguments in the name_to_pvalue map.
    name_to_pvalue.clear();
    for (auto &arg : func->args())
        name_to_pvalue[arg.getName()] = &arg;

    // todo
    //       body code gen
    if (Value *body_val = func_body->codegen()) {

    }

    if (Value *return_val = return_expr->codegen()) {
        // Finish off the function.
        builder->CreateRet(return_val);

        // Validate the generated code, checking for consistency.
        verifyFunction(*func);

        return func;
    }

    // Error reading body, remove function.
    func->eraseFromParent();
    return nullptr;
}

Value *Unary_expr_AST::codegen() {
    return LHS->codegen();
}

Value *Binary_expr_AST::codegen() {
    Value *L = LHS->codegen();
    Value *R = RHS->codegen();
    if (!L || !R)
        return nullptr;
// todo
//  float type code gen -> FAdd for example
//  add support for mod & pow
    switch (op) {
        case op_add:
            return builder->CreateFAdd(L, R, "addtmp");
        case op_sub:
            return builder->CreateFSub(L, R, "subtmp");
        case op_mul:
            return builder->CreateFMul(L, R, "multmp");
        case op_div:
            return builder->CreateFDiv(L, R, "divtmp");
        case op_le:
            L = builder->CreateFCmpULE(L, R, "cpm_le_tmp");
        case op_lt:
            L = builder->CreateFCmpULT(L, R, "cmp_lt_tmp"); // returns an ‘i1’ value
            return builder->CreateUIToFP(L, Type::getDoubleTy(*context), "booltmp");
        case op_ge:
            L = builder->CreateFCmpUGE(L, R, "cpm_ge_tmp");
            return builder->CreateUIToFP(L, Type::getDoubleTy(*context), "booltmp");
        case op_gt:
            L = builder->CreateFCmpUGT(L, R, "cmp_gt_tmp");
            return builder->CreateUIToFP(L, Type::getDoubleTy(*context), "booltmp");
        case op_equal:
            L = builder->CreateFCmpUEQ(L, R, "cmp_eq_tmp");
            return builder->CreateUIToFP(L, Type::getDoubleTy(*context), "booltmp");
        default:
            return LogError("invalid binary operator\n");
    }
//    the name ("xxxtmp") is just a hint. For instance, if the code above emits multiple “addtmp” variables,
//    LLVM will automatically provide each one with an increasing, unique numeric suffix.
//    Local value names for instructions are purely optional, but it makes it much easier to read the IR dumps.
}

Value *Function_call_AST::codegen() {
    // Look up the name in the global module table.
    Function *callee = module->getFunction(std::string(name.content, name.len));
    if (!callee)
        return LogError("Unknown function referenced\n");

    // If argument mismatch error.
    if (callee->arg_size() != args.size())
        return LogError("Incorrect # arguments passed\n");

    std::vector<Value *> v_args;
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
        v_args.push_back(args[i]->codegen());
        if (!v_args.back())
            return nullptr;
    }
    return builder->CreateCall(callee, v_args, "calltmp");
}

Function *Function_proto_AST::codegen() {
    // Make the function type:  fp (fp, fp...) etc.
    std::vector<Type*> v_fp (args.size(), Type::getDoubleTy(*context));
    FunctionType *func_type = FunctionType::get(Type::getDoubleTy(*context), v_fp, false);
    Function *func = Function::Create(func_type, Function::ExternalLinkage, raw_to_string(name), module.get());
    // Set names for all arguments.
    unsigned idx = 0;

    for (auto &llvm_arg : func->args()) {
        llvm_arg.setName(raw_to_string(args[idx++]->name));
    }

    return func;
}

Value *Define_AST::codegen() {
    if (name_to_pvalue[raw_to_string(var->name)]) return LogError("Redefine the variable!\n");
    name_to_pvalue[raw_to_string(var->name)] = rhs->codegen();
    return name_to_pvalue[raw_to_string(var->name)];
}

Value *Assign_AST::codegen() {
    if (!name_to_pvalue[raw_to_string(var->name)]) return LogError("RT_Variable has not been defined!\n");

    name_to_pvalue[raw_to_string(var->name)] = rhs->codegen();
    return name_to_pvalue[raw_to_string(var->name)];
}

Value *If_AST::codegen() {
    Value *cond_res = cond->codegen();
    if (!cond_res)
        return nullptr;

    // Convert condition to a bool by comparing non-equal to 0.0.
    cond_res = builder->CreateFCmpONE(cond_res, ConstantFP::get(*context, APFloat(0.0)), "ifcond");

    // it gets the current RT_Function object that is being built. It gets this by asking the builder for the current
    // BasicBlock, and asking that block for its “parent” (the function it is currently embedded into)
    Function *func = builder->GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.

    // it passes “func” into the constructor for the “if” block. This causes the constructor to automatically
    // insert the new block into the end of the specified function. The other two blocks are created,
    // but aren’t yet inserted into the function.
    BasicBlock *if_basic_block   = BasicBlock::Create(*context, "ifblock", func);
    BasicBlock *else_basic_block = BasicBlock::Create(*context, "elseblock");
    BasicBlock *merged_basic_block     = BasicBlock::Create(*context, "ifcont");

    builder->CreateCondBr(cond_res, if_basic_block, else_basic_block);

    // Emit then value.
    builder->SetInsertPoint(if_basic_block);

    Value *if_block_res = if_block->codegen();
    if (!if_block_res)
        return nullptr;

    builder->CreateBr(merged_basic_block);
    // Codegen of 'if_block' can change the current block, update if_basic_block for the PHI.
    if_basic_block = builder->GetInsertBlock();

    // Emit else block.
    func->getBasicBlockList().push_back(else_basic_block);
    builder->SetInsertPoint(else_basic_block);

    Value *else_block_res = else_block->codegen();
    if (!else_block_res)
        return nullptr;

    builder->CreateBr(merged_basic_block);
    // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
    else_basic_block = builder->GetInsertBlock();

    // Emit merge block.
    func->getBasicBlockList().push_back(merged_basic_block);
    builder->SetInsertPoint(merged_basic_block);
    PHINode *phi_node = builder->CreatePHI(Type::getDoubleTy(*context), 2, "iftmp");

    phi_node->addIncoming(if_block_res, if_basic_block);
    phi_node->addIncoming(else_block_res, else_basic_block);
    return phi_node;
}

Value *While_AST::codegen() {
    return nullptr;
}


// PRINT VIRTUAL FUNCTION IMPLEMENTATION
void Variable_AST::print() { printf("[VAR_EXP] %.*s\n", (int)name.len, name.content); }
void Float_point_AST::print() { printf("[FP_EXP] %f\n", val); }

void Integer_AST::print() { printf("[INT_EXP] %d\n", val); }

void Block_AST::print() {
    printf("[BLOCK]\n");
    if (v_expr.empty()) printf("empty code block!\n");
    for (auto &&expr : v_expr) {
        expr->print();
    }
}

void Function_proto_AST::print() {
    printf("[FUNC_PROTO]\n");
    printf("func name: %.*s\n", (int)name.len, name.content);
    printf("(\n");
    for (int i=0; i < args.size(); i++) {
        args[i] -> print();
        if (i < args.size()-1) printf(", \n");
    }
    printf(")\n");
}

void Function_AST::print() {
    printf("[FUNC]\n");
    args_with_func_name->print();
    printf("func body: \n");
    func_body->print();
    printf("return: \n");
    return_expr->print();
}

void Function_call_AST::print() {
    printf("[FUNC_CALL]\n");
    printf("func : %.*s (\n", (int)name.len, name.content);
    for (int i=0; i < args.size(); i++) {
        args[i] -> print();
        if (i < args.size()-1) printf(", \n");
    }
    printf(")\n");
}

void Unary_expr_AST::print() {
    printf("[UNARY_EXP]\n");
    LHS->print();
}

void Binary_expr_AST::print() {
    printf("[LHS_EXP]\n");
    LHS->print();
    printf("[BIN_OP ] %s\n", names_kind[op]);
    printf("[RHS_EXP]\n");
    RHS->print();
}

void Define_AST::print() {
    printf("[DEF_STMT]\n");
    var->print();
    printf("=\n");
    rhs->print();
}

void Assign_AST::print() {
    printf("[ASSIGN]\n");
    var->print();
    printf("=\n");
    rhs->print();
}

void If_AST::print() {
    printf("[IF_STMT]\n");
    printf("if ");
    cond->print();
    printf(" is not 0\n");
    if_block->print();
    if (else_block) {
        printf("[ELSE]\n");
        else_block->print();
    }
}

void While_AST::print() {
    printf("[While_STMT]\n");
    printf("While ");
    cond->print();
    printf(" is not 0\n");
    while_block->print();
}

using namespace runtime_ns;

using ptr_value = std::unique_ptr<RT_Value>;

RT_Value Float_point_AST::eval(Runtime *rt) {
    return RT_Value(FP, val);
}

RT_Value Variable_AST::eval(Runtime *rt) {
    return rt->get_variable(raw_to_string(name));
}

RT_Value Assign_AST::eval(Runtime *rt) {
    auto res = rhs->eval(rt);
    rt->creat_variable(raw_to_string(var->name), res);
    return RT_Value();
}

RT_Value Define_AST::eval(Runtime *rt) {
    auto res = rhs->eval(rt);
    rt->creat_variable(raw_to_string(var->name), res);
    return RT_Value();
}

RT_Value Unary_expr_AST::eval(Runtime *rt) {
    return LHS->eval(rt);
}

RT_Value Binary_expr_AST::eval(Runtime *rt) {
    RT_Value l_val = LHS->eval(rt);
    RT_Value r_val = RHS->eval(rt);

    switch(op) {
        default:
            LogError(std::string("Unexpected operator " + std::string(names_kind[op])));
            return RT_Value();
        case op_add:
            return l_val + r_val;
        case op_sub:
            return l_val - r_val;
        case op_mul:
            return l_val * r_val;
        case op_div:
            return l_val / r_val;
        case op_gt:
            return l_val > r_val;
        case op_ge:
            return l_val >= r_val;
        case op_lt:
            return l_val < r_val;
        case op_le:
            return l_val <= r_val;
        case op_pow:
            return l_val * r_val;
        case op_equal:
            return l_val == r_val;
    }

    return RT_Value();
}


RT_Value Block_AST::eval(Runtime *rt) {
    for (auto &expr : v_expr) {
        expr->eval(rt);
    }

    return RT_Value();
}

RT_Value Function_AST::eval(Runtime *rt) {
    auto *func = new RT_Function;

    for (auto & arg : args_with_func_name->args) {
        func->params_name.push_back(raw_to_string(arg->name));
    }

    func->block = std::move(func_body);
    func->ret = std::move(return_expr);

    rt->creat_function(raw_to_string(args_with_func_name->name), func);
    return RT_Value();
}

RT_Value Function_call_AST::eval(Runtime *rt) {
    RT_Function *func = rt->get_function(raw_to_string(name));

    if (func->params_name.size() != args.size()) {
        panic("Runtime Error : wrong argument number! func %s required %d, but provided %d\n",
                raw_to_string(name).c_str(), func->params_name.size(), args.size());
    }

    std::vector<RT_Value> v_args;

    for (auto &arg : args) {
        v_args.emplace_back(arg->eval(rt));
    }

    rt->creat_context();
    rt->creat_variables(func->params_name, std::move(v_args));
    func->block->eval(rt);
    RT_Value res = func->ret->eval(rt);
    rt->ruin_context();

    return res;
}

RT_Value If_AST::eval(Runtime* rt) {
    RT_Value cond_res = cond->eval(rt);
    if (cond_res.to_bool() && if_block){
        if_block->eval(rt);
    } else {
        if (else_block) else_block->eval(rt);
    }

    return RT_Value();
}

RT_Value While_AST::eval(Runtime* rt) {
    while (cond->eval(rt).to_bool()){
        while_block->eval(rt);
    }
    return RT_Value();
}

void Context::creat_variable(const std::string& id_name, RT_Value val) {
    vars[id_name] = val;
}

void Context::creat_variables(std::vector<std::string> id_names, std::vector<RT_Value> vals) {
    for (int i = 0; i < id_names.size(); i++) {
        vars.emplace(std::move(id_names[i]), vals[i]);
    }
}

RT_Value Context::get_variable(const std::string &id_name) {
    if (auto res = vars.find(id_name); res != vars.end()) {
        return res->second;
    }

    panic("Variable %s not found!\n", id_name.c_str());
    return RT_Value();
}

bool Context::has_variable(const std::string& id_name) {
    return vars.find(id_name) != vars.end();
}

void Context::creat_function(std::string name, RT_Function* f) {
    funcs.emplace(std::move(name), f);
}

bool Context::has_function(const std::string& name) {
    return funcs.find(name) != funcs.end();
}

RT_Function* Context::get_function(const std::string& name) {
    if (auto res = funcs.find(name); res != funcs.end()) {
        return res->second;
    }
    return nullptr;
}

template <int _Value_Type>
inline bool RT_Value::is_type() {
    return this->type == _Value_Type;
}

RT_Value RT_Value::operator+(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(FP, data.fp + rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(INT, data._int + rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator-(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(FP, data.fp - rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(INT, data._int - rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator*(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(FP, data.fp * rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(INT, data._int * rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator/(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(FP, data.fp / rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(INT, data._int / rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator%(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(INT, data._int / rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator>(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(BOOL, data.fp > rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(BOOL, data._int > rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator<(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(BOOL, data.fp < rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(BOOL, data._int < rhs.data._int);
    }

    return RT_Value();
}
RT_Value RT_Value::operator==(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(BOOL, data.fp == rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(BOOL, data._int == rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator>=(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(BOOL, data.fp >= rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(BOOL, data._int >= rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator<=(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(BOOL, data.fp <= rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(BOOL, data._int <= rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator^(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (!(rhs.is_type<INT>())) {
        panic("Runtime Error : the second operand of ^ operator should be integer!\n");
    }

    if (!(rhs.is_type<INT>() || rhs.is_type<FP>())) {
        panic("Runtime Error : the first operand of ^ operator should be integer or float!\n");
    }

    for (int i = 0; i < rhs.data._int; i++) {
        rhs = rhs * rhs;
    }

    return rhs;
}

bool RT_Value::to_bool() {
    if (is_type<BOOL>()) return data._bool;
    if (is_type<VOID>()) return false;
    if (is_type<INT>()) return data._int == 0;
    if (is_type<FP>()) return data.fp == 0.;

    panic("Runtime Error : Unexpected RT_Value type!\n");
    return false;
}

void Runtime::creat_function(std::string name, RT_Function *f) {
    contexts.back()->creat_function(std::move(name), f);
}

RT_Function *Runtime::get_function(const std::string &name) {
    if (contexts.empty()) {
        return nullptr;
    }

    return contexts.back()->get_function(name);
}

void Runtime::creat_variable(const std::string& name, RT_Value rt) {
    contexts[contexts.size()-1]->creat_variable(std::move(name), rt);
}

RT_Value Runtime::get_variable(const std::string &name) {
    return contexts[contexts.size()-1]->get_variable(name);
}

void Runtime::creat_context() {
    contexts.push_back(new Context);
}

void Runtime::ruin_context() {
    contexts.pop_back();
}

void Runtime::creat_variables(std::vector<std::string> id_names, std::vector<RT_Value> vals) {
    contexts[contexts.size()-1]->creat_variables(std::move(id_names), std::move(vals));
}
