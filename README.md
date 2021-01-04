# Intro
Design a language with interpreter, JIT and AoT compilation execution modes. It is supposed to support object-oriented programming paradigm and high performance matrix multiplication through utilizing MLIR.

<p align="center">
  <img src="https://github.com/linsinan1995/compiler/blob/master/pic/res.png" img width="400">
</p>

# Table of Content
- [Intro](#intro)
- [Feature](#feature)
- [How to build](#how-to-build)
- [Interpreter](#interpreter)
  * [1. define a variable](#1-define-a-variable)
  * [2. control flow](#2-control-flow)
  * [3. define a function](#3-define-a-function)
  * [4. error handling](#4-error-handling)
  * [5. built-in function](#5-built-in-function)
  * [6. customized class](#6-customized-class)
- [Lexing](#lexing)
- [Parsing](#parsing)
- [Execution](#execution)
  * [1.Interpreter](#1interpreter)
- [Note](#note)
  * [Putting object with non-trivial ctor to Union](#putting-object-with-non-trivial-ctor-to-union)
  * [RAII helper for indentation control](#raii-helper-for-indentation-control)
  * [RAII switch helper](#raii-switch-helper)
  * [Variadic template for pushing data elegantly](#variadic-template-for-pushing-data-elegantly)
  * [X macro for managing enum value](#x-macro-for-managing-enum-value)
  * [Parameter unpacking for perfect forwarding and placement new](#parameter-unpacking-for-perfect-forwarding-and-placement-new)
  * [Generic warning printer](#generic-warning-printer)
  * [Template type alignment for clang and gcc](#template-type-alignment-for-clang-and-gcc)

# Feature
- [x] Interpreter
  - [x] arithmetic operation
  - [x] function
  - [x] control flow
  - [x] customized class
  - [x] matrix, string, float
  - [x] built-in function registered from C++ interface
  - [ ] indexing
  - [ ] pass-by-value and pass-by-reference
- [ ] JIT
- [ ] AoT
- [ ] Code optimization
- [ ] better garbage collection
- [ ] performance and memory optimization

# How to build

```
git clone https://github.com/linsinan1995/compiler.git
cd compiler
mkdir build
cd build
cmake ..

// build ide only
cmake --build . -j 4 -t ide
// build all
// cmake --build . -j 4
```
# Interpreter
Runtime design:

<p align="center">
  <img src="https://github.com/linsinan1995/compiler/blob/master/pic/runtime.png" img width="600">
</p>

Some demo for representing the features of this language.

## 1. define a variable
<p align="center">
  <img src="https://github.com/linsinan1995/compiler/blob/master/pic/define_a_var.png" img width="400">
</p>

## 2. control flow
<p align="center">
  <img src="https://github.com/linsinan1995/compiler/blob/master/pic/control_flow.png" img width="300">
</p>

## 3. define a function
<p align="center">
  <img src="https://github.com/linsinan1995/compiler/blob/master/pic/define_a_fun.png" img width="500">
</p>

## 4. error handling
<p align="center">
  <img src="https://github.com/linsinan1995/compiler/blob/master/pic/error_handling.png" img>
</p>

## 5. built-in function
<p align="center">
  <img src="https://github.com/linsinan1995/compiler/blob/master/pic/customized_built_in.png" img width="300">
</p>

## 6. customized class
<p align="center">
  <img src="https://github.com/linsinan1995/compiler/blob/master/pic/customized_class.png" img width="500">
</p>

<p align="center">
  <img src="https://github.com/linsinan1995/compiler/blob/master/pic/customized_class2.png" img width="450">
</p>

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

# Note
There are notes on how to engineer some features in this project.

## Putting object with non-trivial ctor to Union
Why use union?

A: My personal laptop is in a quite old version of MacOS, and it doesn't support generic data containers, such as std::any and std::variant. Also, using boost in such a micro project is not an appealing solution.

tagged union & placement new
```cpp
struct Mat {
    std::vector<float> data;
    std::vector<int> dim;
};

class Value {
    // tagged union can have ctor & dtor in ISO C++
    union VALUE_Data {
        float       fp;
        int         _int;
        bool        _bool;
        std::string _str;
        Mat         matrix;
        VALUE_Data() {}
        ~VALUE_Data() {}
    };

    VALUE_Data data;
    // VOID, MATRIX are enum value that is used for RTTI
    RT_Value() : type(VOID) {};
    ...

    // placement new & tagged union for storing object in union
    explicit RT_Value(Mat m_val) : type(MATRIX) { new (&data.matrix) Mat(std::move(m_val)); };

    // do not forget that we break the rule of five
    RT_Value(const RT_Value& m_val);  // new (&data.matrix) Mat(m_val)
    RT_Value(RT_Value&& m_val) noexcept;
    RT_Value &operator=(RT_Value m_val);
}
```


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

## RAII switch helper
RAII with a simple bool lock can be used to print desired result in a recursive function.

```cpp
// control the print switch by RAII
struct Switch {
    explicit Switch(bool &_switch) : m_switch(_switch) {
        // Is this object turns on the switch?
        inner = !m_switch;
        // turn on switch
        if (!m_switch) m_switch = true;
    }
    ~Switch() { if (inner) m_switch = false; }
    bool &m_switch;
    bool inner;
};

void AST_Printer::visit_mat(Matrix_AST &expr) {
    Indent ind(cur_indent);

    if (!no_info)
        os << ind.get_indent() << "[MATRIX]\n";

    if (expr.dim.empty()) {
        os << ind.get_indent() << "Empty matrix\n";
        return ;
    }
    if (!no_info) {
        os << ind.get_indent() << "dims: ";
        for (int i = 0; i < expr.dim.size()-1; i++)
            os << expr.dim[i] << ",";
        os << expr.dim.back() << "\n";
        os << ind.get_indent() << "value:\n";
    }

    // control print by RAII
    // by adding an inner lock in Switch class, we can control the
    // print result in a recursive function.

    // only the first initialization can turn the switch on after
    // it is not alive, the switch will be turned off.
    Switch switcher(no_info);

    for (int i = 0; i < expr.dim[0]; i++) {
        if (auto inner = dynamic_cast<Float_point_AST*> (expr.values[i].get())) {
            if (i == 0) {
                os << ind.get_indent();
            }
            os << inner->m_val << " ";
        } else {
            Expression_AST &mat = *(expr.values[i]);
            visit_mat(dynamic_cast<Matrix_AST&> (mat));
        }
    }
    os << "\n";
}

```

## Variadic template for pushing data elegantly
The paradigm below can be easily implemented by using variadic template, but it does not look great for extra function (make_pair).
```cpp
push_back(make_pair(a,b),
          make_pair(c,d),
          make_pair(e,f))
```

To do it more elegantly, we can unpack arguments by two, and use one class Entries to store remaining chunks
```cpp
// as terminator
template <typename STR>
void push_back(STR&& func, Runtime::builtin_func_t func_ptr) {
    funcs.emplace_back(std::forward<STR>(func), func_ptr);
}

template <typename STR, typename... Entries>
void push_back(STR&& func, Runtime::builtin_func_t func_ptr, Entries && ...args) {
    funcs.emplace_back(std::forward<STR>(func), func_ptr);
    push_back(std::forward<Entries> (args)...);
}

// then
string func_name   = "println";
string func_name_2 = "func_info";
reg->push_back(func_name,         builtin_println,
               move(func_name_2), builtin_print_func_args,
               "info",            builtin_print_statue);
```

## X macro for managing enum value

see include/Kind.def and include/Enum.h


## Parameter unpacking for perfect forwarding and placement new
```c++
...
template <typename ...Args>
RT_Value* alloc_var(Args &&... args)  { return pool_var.alloc(std::forward<Args>(args)...); }
...


template <typename... Args>
T *alloc(Args &&... args) {
    ...

    // get the storage for type T.
    T *result = cur_item->get_data();
    // construct the object.
    new (result) T(std::forward<Args>(args)...);
    return result;
}
```

## Generic warning printer

1. Return value is expected to be an object
```c++
template <typename Ret_Type>
static Ret_Type panic_type(char const* const format, ...) {
    cmd_color_red();
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    cmd_color_reset();
    return {};
}

// example : return an empty object
...
if (func_name->is_not_type<STRING>()) {
    return panic_type<RT_Value>("Runtime Error: Wrong argument type in "
                                "builtin_print_func_args! It required string but got %s "
                                "arguments\n", names_rt_val_kind[func_name->type]);
}
...
```

2. Return value is expected to be a pointer
```c++
static std::nullptr_t panic_nptr(char const* const format, ...) {
    cmd_color_red();
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    cmd_color_reset();
    return nullptr;
}

// example : return a nullptr
...
if (cur_token->kind == k_EOF)
    return panic_nptr("Parsing Error: Fail to parse return expr in parse_def_func_expr! "
                      "The parser expects return key word\n");
...
```

3. A deadly error
```c++
[[noreturn]] static void panic_noreturn(char const* const format, ...) {
    cmd_color_red();
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    cmd_color_reset();
    exit(EXIT_FAILURE);
}

// error in memory allocation
...
auto *ptr_buffer = static_cast<string_buffer*> (malloc(sizeof(string_buffer)));
if (ptr_buffer == nullptr)
    panic_noreturn("Error: Fail to allocate memory for string_buffer!\n");
...
```

## Template type alignment for clang and gcc

Gcc and clang use different ways to specify the alignment requirement. However, somehow, clang pretends to be gcc, and also has macro __GNU__. Here is my solution.

```c++
#if defined(__clang__)
        using StorageType = __attribute__((alignof(T))) char[sizeof(T)];
#else
        using StorageType = alignas(alignof(T)) char[sizeof(T)];
#endif
```