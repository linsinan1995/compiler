/******************************************************************* 
 * 
 *    @Time           @Author       @Description
 * ------------      ---------      -----------
 * 2020-12-17 17:28   Lin Sinan         None
 *
 * @File    :   CodeGen.py    
 * @Contact :   mynameisxiaou@gmail.com
 * @github  :   https://github.com/linsinan1995
 * @usage   :   
 * 
 ******************************************************************/

//
// Created by Lin Sinan on 2020-12-17.
//

#include "Ast.h"

using namespace llvm;

static std::unique_ptr<llvm::LLVMContext> context = std::make_unique<llvm::LLVMContext>();
static std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("jit", *context);
static std::unique_ptr<llvm::IRBuilder<>> builder = std::make_unique<llvm::IRBuilder<>>(*context);;
static std::map<std::string, llvm::Value *> name_to_pvalue;


// codegen VIRTUAL FUNCTION IMPLEMENTATION
Value *Integer_AST::codegen() {
    return ConstantFP::get(*context, APFloat(val)); // APInt(/*nbits*/32, val, /*bool is_signed*/true));
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
    Function *func = module->getFunction(raw_to_string(args_with_func_name->name));

    if (!func)
        func = args_with_func_name->codegen();

    if (!func)
        return nullptr;

    if (!func->empty())
        return LogError("Function cannot be redefined.");

    // Create a new basic block to start insertion into.
    BasicBlock *basic_block = BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(basic_block);

    // Record the function arguments in the name_to_pvalue map.
    name_to_pvalue.clear();
    for (auto &arg : func->args())
        name_to_pvalue[arg.getName()] = &arg;


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
    if (!name_to_pvalue[raw_to_string(var->name)]) return LogError("Variable has not been defined!\n");

    name_to_pvalue[raw_to_string(var->name)] = rhs->codegen();
    return name_to_pvalue[raw_to_string(var->name)];
}

Value *If_AST::codegen() {
    return nullptr;
}

Value *While_AST::codegen() {
    return nullptr;
}

Value *Else_AST::codegen() {
    return nullptr;
}

// PRINT VIRTUAL FUNCTION IMPLEMENTATION
void Variable_AST::print() { printf("[VAR_EXP] %.*s\n", (int)name.len, name.content); }
void Integer_AST::print() { printf("[INT_EXP] %f\n", val); }

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
    printf(" is equal to 0\n");
    if_block->print();
}

void While_AST::print() {
    printf("[While_STMT]\n");
    printf("While ");
    cond->print();
    printf(" is equal to 0\n");
    if_block->print();
}

void Else_AST::print() {
    printf("[IF_STMT]\n");
    printf("if ");
    cond->print();
    printf(" is less than 0\n");
    printf("[ELSE]\n");
    else_block->print();
}
