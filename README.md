# compiler

A simple compiler project.


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
    - Visitor pattern (enum visitor to break dependency of ast & interpreter)


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
[FUNC_PROTO]
func name: max
(
    [VAR_EXP] x
    ,
    [VAR_EXP] z
)
func body:
[BLOCK]
    [IF_STMT]
        if [LHS_EXP]
                [UNARY_EXP]
                    [VAR_EXP] x
           [BIN_OP ] op_gt
           [RHS_EXP]
                [UNARY_EXP]
                    [VAR_EXP] z
        is not 0

    [BLOCK]
        [ASSIGN]
            [VAR_EXP] x
            =
            [UNARY_EXP]
                [VAR_EXP] z
        [ASSIGN]
            [VAR_EXP] z
            =
            [UNARY_EXP]
                [VAR_EXP] x
return:
    [UNARY_EXP]
        [VAR_EXP] z
=========line 2=========
[FUNC_CALL]
func : max (
    [UNARY_EXP]
        [FP_EXP] 1.000000
    ,
    [UNARY_EXP]
        [FP_EXP] 3.000000
)
=========line 3=========
[FUNC_CALL]
func : max (
    [UNARY_EXP]
        [FUNC_CALL]
            func : max (
                [UNARY_EXP]
                    [FP_EXP] 1.000000
                ,
                [UNARY_EXP]
                    [FP_EXP] 4.000000
        )
    ,
    [UNARY_EXP]
        [FP_EXP] 3.000000
)
============================================
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


# Problems on execution condition statement

The current AST structure can not execute code like this
```
func max(a, b) {
    if (a > b) { return a }
    return b
}
```
To execute, you should write like this
```
func max(a, b) {
    if (a > b) {
        b = a
    }
    return b
}
```
In the interpreter mode, it is easy to be engineered (just add an enum for execution state, break, return, cont).