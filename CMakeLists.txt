cmake_minimum_required(VERSION 3.14)
project(compiler)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(compiler main.cpp src/Lexer.cpp)
add_executable(test_lexer test/test_lexer.cpp src/Lexer.cpp)
add_executable(test_parser test/test_parser.cpp src/Parser.cpp src/Lexer.cpp)
