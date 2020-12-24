#include <iostream>

#include "Interpreter/Builtin_function.hpp"
#include "Interpreter/AST_Interpreter.h"
#include "Parser.h"

using namespace parser_ns;
using namespace runtime_ns;
using namespace register_ns;

// define the function you want to build in
// function should be type
// RT_Value (*) (Runtime*, std::vector<RT_Value>)
RT_Value builtin_println(Runtime* rt, std::vector<RT_Value> args) {
    for (const auto& arg : args) {
        std::cout << arg << "\n";
    }
    if (args.empty()) std::cout << "\n";
    return RT_Value();
}

RT_Value ones(Runtime* rt, std::vector<RT_Value> args) {
    if (args.size() > 2 || args.empty())
        return panic_type<RT_Value> ("Runtime Error: Wrong argument number in "
                                     "ones! It required 2 but got %d arguments\n",
                                     args.size());
    if (args[0].is_not_type<FP>() || args[1].is_not_type<FP>())
        return panic_type<RT_Value> ("Runtime Error: Wrong argument type in "
                                     "ones! It required 2 FP but got %s and %s.\n",
                                     names_rt_val_kind[args[0].type],
                                     names_rt_val_kind[args[1].type]);
    // one dimension array
    if (args.size() == 1) {
        if (args[0].is_not_type<FP>()) {
            return panic_type<RT_Value> ("Runtime Error: Wrong argument type in "
                                         "ones! It required a FP but got %s.\n",
                                         names_rt_val_kind[args[0].type]);
        }

        int len = static_cast<int>(args[0].data.fp);
        Mat mat {std::vector<float> (len, 1), std::vector<int> {len}};
        return RT_Value(std::move(mat));
    }

    // two dimension matrix
    int height = static_cast<int>(args[0].data.fp), width = static_cast<int>(args[1].data.fp);
    Mat mat {std::vector<float> (width*height, 1), std::vector<int> { height, width }};

    return RT_Value(std::move(mat));
}

// test code
const char *code = "print(123, 42, 52, [1,2,3],\"Hello\", [[1,2,3], [4,5,6]])\n"
                   "print(\"ones(2,3) * ones(3,6)\")\n"
                   "ones(2,3) * ones(3,6)\n"
                   "print(\"ones(15,3) * [0.5, 3.2, 0.4]\")\n"
                   "ones(15,3) * [0.5, 3.2, 0.4]\n"
                   "print(\"ones(4,2)\")\n"
                   "ones(4,2)";

int main() {
    builtin_register reg;
    AST_Interpreter interpreter {};

    // get parser obj
    auto parser = Parser::make_parser(code);

    reg.push_back("ones"/*function name*/, ones/*function ptr*/,
                  "print",                 builtin_println);

    // dump functions to runtime object
    reg._register(interpreter.rt.get());

    // parsing code to AST
    std::vector<std::shared_ptr<Expression_AST>> v = parser->parse();

    // traversal AST and interpret expression
    for (auto &&expr : v) {
        interpreter.evaluate(*expr);
        if (!interpreter.is_null())
            std::cout << interpreter.val << "\n";
    }
}