cmake_minimum_required(VERSION 3.14)
project(libcompiler)

set(CMAKE_CXX_STANDARD 17)

#set(LLVM_DIR /usr/local/Cellar/llvm/8.0.0/lib/cmake/llvm)
#find_package(LLVM 8.0.0 CONFIG)
#
#set( CMAKE_VERBOSE_MAKEFILE on )
#message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
#message(STATUS "CMAKE_ROOT ${CMAKE_ROOT}")
#message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")
#message(STATUS "LLVM_FOUND ${LLVM_FOUND}")
#message(STATUS "LLVM_DIR ${LLVM_DIR}")
#message(STATUS "LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
#message(STATUS "LLVM_DEFINITIONS: ${LLVM_DEFINITIONS}")
#message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
#
#message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
#message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
#
#include_directories(${LLVM_INCLUDE_DIRS})
#add_definitions(${LLVM_DEFINITIONS})
#
#llvm_map_components_to_libnames(llvm core support native irreader)

include_directories(../include)

add_library(
    lexer
    Lexer.cpp
)

add_library(
    interpreter
    Lexer.cpp
    Interpreter/Runtime.cpp
    Interpreter/AST_Interpreter.cpp
    Interpreter/Symbol_table.cpp
    Interpreter/Value.cpp
    String_buffer.cpp
    Ast.cpp
    Parser.cpp
    AST_visitor/AST_Printer.cpp
)