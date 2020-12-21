# compiler
Design a language with interpreter, JIT and AoT compilation execution modes. It is supposed to support high performance matrix multiplication through utilizing MLIR.

<p align="center">
  <img src="https://github.com/linsinan1995/compiler/blob/master/other/res.jpg" img width="240">
</p>
  
# Work List
- Finished
  - Lexer
  - Parser
    - Recursive descend
    - Precedence
  - Execution
    - Interpreter
    - AoT compilation
        - codegen (partial)
  - Code reorganize
    - Visitor pattern for AST

- To do
  - Error handling
  - MLIR IR emitter
  - Code Gen
    - control flow
    - mutable variable
  - JIT
  - Rich data type
    - matrix
    - indexing
  - code redesign
    - performance bottleneck (llvm::StringRef, llvm::cl ... )


# Lexing

source code:
```
func x(a, b) {
    return a * b
}

x(1,2)
x(x(1,2),3)
```

```
==============TEST: Readme==============
kw_func             	func
k_var               	x
k_open_paren        	(
k_var               	a
k_comma             	,
k_var               	b
k_close_paren       	)
k_open_curly        	{
kw_return           	return
k_var               	a
op_mul              	*
k_var               	b
k_close_curly       	}
k_var               	x
k_open_paren        	(
k_fp                	1
k_comma             	,
k_fp                	2
k_close_paren       	)
k_var               	x
k_open_paren        	(
k_var               	x
k_open_paren        	(
k_fp                	1
k_comma             	,
k_fp                	2
k_close_paren       	)
k_comma             	,
k_fp                	3
k_close_paren       	)
k_EOF
============================================
```


# Parsing

source code:
```
func x(a, b) {
    return a * b
}

x(1,2)
x(x(1,2),3)
```

```
==============  TEST 5: readme  ==============
=========line 1=========
[FUNC]
    func name: x(
        [VAR_EXP] a
    ,
        [VAR_EXP] b
    )
func body:
    [BLOCK]
    empty code block!
return:
    [LHS_EXP]
        [UNARY_EXP]
            [VAR_EXP] a
    [BIN_OP ] op_mul
    [RHS_EXP]
        [UNARY_EXP]
            [VAR_EXP] b
=========line 2=========
[FUNC_CALL]
func : x (
    [UNARY_EXP]
        [FP_EXP] 1
,
    [UNARY_EXP]
        [FP_EXP] 2
)
=========line 3=========
[FUNC_CALL]
func : x (
    [UNARY_EXP]
        [FUNC_CALL]
        func : x (
            [UNARY_EXP]
                [FP_EXP] 1
        ,
            [UNARY_EXP]
                [FP_EXP] 2
        )
,
    [UNARY_EXP]
        [FP_EXP] 3
)

==============================================
```

# Execution

## 1.Interpreter
```
func x(a, b) {
    return a * b
}
x(1,2)
x(x(1,2),3);
```

```
=============  TEST 1: Read me  ==============
2.000000
6.000000
==============================================
```

# Register built-in function from C++
A demo for registering functions

```cpp
#include <iostream>

#include "AST_visitor/AST_Interpreter.h"
#include "Parser.h"

using namespace parser_ns;
using namespace runtime_ns;

// example code
const char *code =
        "# helloworld & println are functions ported from C++ interface\n"
        "helloworld()\n"
        "println(123,42,52)";

// helper function for converting RT_Value to std::string
static std::string rt_value_to_string(RT_Value val) {
    if (val.is_type<VOID>()) return "null";
    if (val.is_type<INT>()) return std::to_string(val.data._int);
    if (val.is_type<BOOL>()) return std::to_string(val.data._bool);
    if (val.is_type<FP>()) return std::to_string(val.data.fp);
    return {};
}

// overwrite << operator for printing results
std::ostream& operator<<(std::ostream &os, RT_Value val) {
    switch (val.type) {
        default:
            return os;
        case FP:
            os << std::to_string(val.data.fp);
            return os;
        case INT:
            os << std::to_string(val.data._int);
            return os;
        case BOOL:
            os << std::boolalpha << val.data._bool;
            return os;
    }
}

// helper class for registering functions
struct builtin_register {
    std::vector<std::pair<std::string, Runtime::buildin_func_t>> funcs;

    // use register_builtin_func(std::string function_name, buildin_func_t func_t) to add
    // your customized functions into the global function symbol map.
    void _register(Runtime *rt) const {
        for (auto & [name, func] : funcs)
            rt->register_builtin_func(name, func);
    }
};

// two functions will be registered, std::vector<RT_Value> args is the argument list
RT_Value builtin_helloworld(Runtime* rt, std::vector<RT_Value> args) {
    std::cout << "Hello world!\n";
    return RT_Value();
}

RT_Value builtin_println(Runtime* rt, std::vector<RT_Value> args) {
    for (auto arg : args) {
        std::cout << rt_value_to_string(arg) << "\n";
    }
    if (args.empty()) std::cout << "\n";
    return RT_Value();
}

int main () {
    builtin_register reg;
    AST_Interpreter interpreter {};

    reg.funcs.emplace_back(std::make_pair("helloworld", builtin_helloworld));
    reg.funcs.emplace_back(std::make_pair("println", builtin_println));
    // dump buildin functions into the global scope of runtime.
    reg._register(interpreter.rt.get());

    auto parser = Parser::make_parser(code);

    std::vector<std::unique_ptr<Expression_AST>> v = parser->parse();

    for (auto &&expr : v) {
        interpreter.evaluate(*expr);
        if (!interpreter.is_null())
            std::cout << interpreter.val << "\n";
    }

}
```

result
```
Hello world!
123.000000
42.000000
52.000000
```

# Problems on execution condition statement

The current AST structure can not execute code that might return value while branching
```
func max(a, b) {
    if (a > b) { return a }
    return b
}
```
To execute, you should write like this way
```
func max(a, b) {
    if (a > b) {
        b = a
    }
    return b
}
```
In the interpreter mode, it is easy to be engineered (just add an enum for execution state, break, return, cont).

# Note
## RAII helper for indentation control
```cpp
// AST_Printer.cpp
#define INDENT_EACH_STEP 2

struct Indent {
    explicit Indent(int &level) : level(level) { level += INDENT_EACH_STEP; }
    std::string get_indent() { return std::string(level, ' '); }
    ~Indent() { level -= INDENT_EACH_STEP; }
    int &level;
};

// level increment 2 every time we initialize an Indent object, so the initial
// level should be -1 *  2, otherwise we cannot get printed result starting without
// indentations.
void AST_Printer::AST_Printer() : cur_indent(-INDENT_EACH_STEP)
{}

void AST_Printer::visit_if(If_AST &expr) {
    Indent ind(cur_indent);
    std::cout << ind.get_indent() << "[IF_STMT]\n";
    std::cout << ind.get_indent() << "if ";
    expr.cond->accept(*this);
    std::cout << ind.get_indent() << " is not 0\n";
    expr.if_block->accept(*this);
    if (expr.else_block) {
        std::cout << ind.get_indent() << "[ELSE]\n";
        expr.else_block->accept(*this);
    }
    // destructor is evoked when this function call is finished, and it will decrease the cur_indent by 2
}
```
