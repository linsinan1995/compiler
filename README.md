# compiler

A simple compiler project.

- Finish
  - Lexer
  - Parser
    - Recursive descend
    - Precedence

- To do
  - Code Gen / half
  - Control flow
  - JIT
  - Rich data type
    - fp
    - matrix
    - indexing
  - MLIR

# Lexing

source code:
```
func x(x, z) {
    return x * z
}

print(x(1,2))
```

```
==============TEST: func block==============
kw_func             	func
k_var               	x
k_open_paren        	(
k_var               	x
k_comma             	,
k_var               	z
k_close_paren       	)
k_open_curly        	{
kw_return           	return
k_var               	x
op_mul              	*
k_var               	z
k_close_curly       	}
k_var               	print
k_open_paren        	(
k_var               	x
k_open_paren        	(
k_int               	1
k_comma             	,
k_int               	2
k_close_paren       	)
k_close_paren       	)
k_EOF
============================================
```


# Parsing

source code:
```
func x(x, z) {
    return x * z
}

print(x(1,2))
```

```
==============TEST 3: func block==============

=========line 1=========
[FUNC]
    [FUNC_PROTO]
        func name: x
        (
            [VAR_EXP] x
        ,
            [VAR_EXP] z
        )
    func body:
        [BLOCK]
            empty code block!
    return:
        [LHS_EXP]
            [UNARY_EXP]
                [VAR_EXP] x
        [BIN_OP ] op_mul
        [RHS_EXP]
            [UNARY_EXP]
                [VAR_EXP] z

=========line 2=========
[FUNC_CALL]
func : print (
    [UNARY_EXP]
        [FUNC_CALL]
            func : x
            (
                [UNARY_EXP]
                    [INT_EXP] 1
                ,
                [UNARY_EXP]
                    [INT_EXP] 2
                )
            )
============================================
```