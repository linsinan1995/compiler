//
// Created by Lin Sinan on 2020-12-21.
//

#ifndef COMPILER_AST_LLVM_IR_EMITTER_H
#define COMPILER_AST_LLVM_IR_EMITTER_H

//
//using namespace llvm;
//using namespace runtime_ns;
//
//static std::unique_ptr<llvm::LLVMContext> context = std::make_unique<llvm::LLVMContext>();
//static std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("jit", *context);
//static std::unique_ptr<llvm::IRBuilder<>> builder = std::make_unique<llvm::IRBuilder<>>(*context);;
//static std::map<std::string, llvm::Value *> name_to_pvalue;
//
//// codegen VIRTUAL FUNCTION IMPLEMENTATION
//Value *Float_point_AST::codegen() {
//    return ConstantFP::get(*context, APFloat(val));
//}
//
//// codegen VIRTUAL FUNCTION IMPLEMENTATION
//Value *Integer_AST::codegen() {
//    return ConstantInt::get(*context, APInt(32, val, true)); // APInt(/*nbits*/32, val, /*bool is_signed*/true));
//}
//
//Value *Variable_AST::codegen() {
//    // Look this variable up in the function.
//    Value *V = name_to_pvalue[std::string(name.content, name.len)];
//
//    return V ? V : LogError("Unknown variable name!\n");
//}
//
//Value *Block_AST::codegen() {
//    return nullptr;
//}
//
//Value *Function_AST::codegen() {
//    // check if it is declared / externed
//    Function *func = module->getFunction(raw_to_string(args_with_func_name->name));
//
//    if (!func)
//        func = args_with_func_name->codegen();
//
//    if (!func)
//        return nullptr;
//
//    if (!func->empty())
//        return LogError("RT_Function cannot be redefined.");
//
//    // Create a new basic block to start insertion into.
//    BasicBlock *basic_block = BasicBlock::Create(*context, "entry", func);
//    builder->SetInsertPoint(basic_block); // insert new block to the end of builder
//
//    // Record the function arguments in the name_to_pvalue map.
//    name_to_pvalue.clear();
//    for (auto &arg : func->args())
//        name_to_pvalue[arg.getName()] = &arg;
//
//    // todo
//    //       body code gen
//    if (Value *body_val = func_body->codegen()) {
//
//    }
//
//    if (Value *return_val = return_expr->codegen()) {
//        // Finish off the function.
//        builder->CreateRet(return_val);
//
//        // Validate the generated code, checking for consistency.
//        verifyFunction(*func);
//
//        return func;
//    }
//
//    // Error reading body, remove function.
//    func->eraseFromParent();
//    return nullptr;
//}
//
//Value *Unary_expr_AST::codegen() {
//    return LHS->codegen();
//}
//
//Value *Binary_expr_AST::codegen() {
//    Value *L = LHS->codegen();
//    Value *R = RHS->codegen();
//    if (!L || !R)
//        return nullptr;
//// todo
////  float type code gen -> FAdd for example
////  add support for mod & pow
//    switch (op) {
//        case op_add:
//            return builder->CreateFAdd(L, R, "addtmp");
//        case op_sub:
//            return builder->CreateFSub(L, R, "subtmp");
//        case op_mul:
//            return builder->CreateFMul(L, R, "multmp");
//        case op_div:
//            return builder->CreateFDiv(L, R, "divtmp");
//        case op_le:
//            L = builder->CreateFCmpULE(L, R, "cpm_le_tmp");
//        case op_lt:
//            L = builder->CreateFCmpULT(L, R, "cmp_lt_tmp"); // returns an ‘i1’ value
//            return builder->CreateUIToFP(L, Type::getDoubleTy(*context), "booltmp");
//        case op_ge:
//            L = builder->CreateFCmpUGE(L, R, "cpm_ge_tmp");
//            return builder->CreateUIToFP(L, Type::getDoubleTy(*context), "booltmp");
//        case op_gt:
//            L = builder->CreateFCmpUGT(L, R, "cmp_gt_tmp");
//            return builder->CreateUIToFP(L, Type::getDoubleTy(*context), "booltmp");
//        case op_equal:
//            L = builder->CreateFCmpUEQ(L, R, "cmp_eq_tmp");
//            return builder->CreateUIToFP(L, Type::getDoubleTy(*context), "booltmp");
//        default:
//            return LogError("invalid binary operator\n");
//    }
////    the name ("xxxtmp") is just a hint. For instance, if the code above emits multiple “addtmp” variables,
////    LLVM will automatically provide each one with an increasing, unique numeric suffix.
////    Local value names for instructions are purely optional, but it makes it much easier to read the IR dumps.
//}
//
//Value *Function_call_AST::codegen() {
//    // Look up the name in the global module table.
//    Function *callee = module->getFunction(std::string(name.content, name.len));
//    if (!callee)
//        return LogError("Unknown function referenced\n");
//
//    // If argument mismatch error.
//    if (callee->arg_size() != args.size())
//        return LogError("Incorrect # arguments passed\n");
//
//    std::vector<Value *> v_args;
//    for (unsigned i = 0, e = args.size(); i != e; ++i) {
//        v_args.push_back(args[i]->codegen());
//        if (!v_args.back())
//            return nullptr;
//    }
//    return builder->CreateCall(callee, v_args, "calltmp");
//}
//
//Function *Function_proto_AST::codegen() {
//    // Make the function type:  fp (fp, fp...) etc.
//    std::vector<Type*> v_fp (args.size(), Type::getDoubleTy(*context));
//    FunctionType *func_type = FunctionType::get(Type::getDoubleTy(*context), v_fp, false);
//    Function *func = Function::Create(func_type, Function::ExternalLinkage, raw_to_string(name), module.get());
//    // Set names for all arguments.
//    unsigned idx = 0;
//
//    for (auto &llvm_arg : func->args()) {
//        llvm_arg.setName(raw_to_string(args[idx++]->name));
//    }
//
//    return func;
//}
//
//
//
//Value *Define_AST::codegen() {
//    if (name_to_pvalue[raw_to_string(var->name)]) return LogError("Redefine the variable!\n");
//    name_to_pvalue[raw_to_string(var->name)] = rhs->codegen();
//    return name_to_pvalue[raw_to_string(var->name)];
//}
//
//
//Value *Assign_AST::codegen() {
//    if (!name_to_pvalue[raw_to_string(var->name)]) return LogError("RT_Variable has not been defined!\n");
//
//    name_to_pvalue[raw_to_string(var->name)] = rhs->codegen();
//    return name_to_pvalue[raw_to_string(var->name)];
//}
//
//
//Value *If_AST::codegen() {
//    Value *cond_res = cond->codegen();
//    if (!cond_res)
//        return nullptr;
//
//    // Convert condition to a bool by comparing non-equal to 0.0.
//    cond_res = builder->CreateFCmpONE(cond_res, ConstantFP::get(*context, APFloat(0.0)), "ifcond");
//
//    // it gets the current RT_Function object that is being built. It gets this by asking the builder for the current
//    // BasicBlock, and asking that block for its “parent” (the function it is currently embedded into)
//    Function *func = builder->GetInsertBlock()->getParent();
//
//    // Create blocks for the then and else cases.  Insert the 'then' block at the
//    // end of the function.
//
//    // it passes “func” into the constructor for the “if” block. This causes the constructor to automatically
//    // insert the new block into the end of the specified function. The pic two blocks are created,
//    // but aren’t yet inserted into the function.
//    BasicBlock *if_basic_block   = BasicBlock::Create(*context, "ifblock", func);
//    BasicBlock *else_basic_block = BasicBlock::Create(*context, "elseblock");
//    BasicBlock *merged_basic_block     = BasicBlock::Create(*context, "ifcont");
//
//    builder->CreateCondBr(cond_res, if_basic_block, else_basic_block);
//
//    // Emit then value.
//    builder->SetInsertPoint(if_basic_block);
//
//    Value *if_block_res = if_block->codegen();
//    if (!if_block_res)
//        return nullptr;
//
//    builder->CreateBr(merged_basic_block);
//    // Codegen of 'if_block' can change the current block, update if_basic_block for the PHI.
//    if_basic_block = builder->GetInsertBlock();
//
//    // Emit else block.
//    func->getBasicBlockList().push_back(else_basic_block);
//    builder->SetInsertPoint(else_basic_block);
//
//    Value *else_block_res = else_block->codegen();
//    if (!else_block_res)
//        return nullptr;
//
//    builder->CreateBr(merged_basic_block);
//    // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
//    else_basic_block = builder->GetInsertBlock();
//
//    // Emit merge block.
//    func->getBasicBlockList().push_back(merged_basic_block);
//    builder->SetInsertPoint(merged_basic_block);
//    PHINode *phi_node = builder->CreatePHI(Type::getDoubleTy(*context), 2, "iftmp");
//
//    phi_node->addIncoming(if_block_res, if_basic_block);
//    phi_node->addIncoming(else_block_res, else_basic_block);
//    return phi_node;
//}
//
//Value *While_AST::codegen() {
//    return nullptr;
//}



#endif //COMPILER_AST_LLVM_IR_EMITTER_H
