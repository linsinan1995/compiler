cmake_minimum_required(VERSION 3.14)
project(compiler)

set(CMAKE_CXX_STANDARD 17)

set(LLVM_DIR /usr/local/Cellar/llvm/8.0.0/lib/cmake/llvm)
find_package(LLVM 8.0.0 CONFIG)

set( CMAKE_VERBOSE_MAKEFILE on )
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "CMAKE_ROOT ${CMAKE_ROOT}")
message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")
message(STATUS "LLVM_FOUND ${LLVM_FOUND}")
message(STATUS "LLVM_DIR ${LLVM_DIR}")
message(STATUS "LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM_DEFINITIONS: ${LLVM_DEFINITIONS}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(include)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

## Find the libraries that correspond to the LLVM components
## that we wish to use
llvm_map_components_to_libnames(llvm core support native irreader)
message(STATUS "Using llvm pkgconfig in: ${llvm}")

add_subdirectory(src)

add_executable(test_lexer test/test_lexer.cpp src/Lexer.cpp)
add_executable(test_parser test/test_parser.cpp)
add_executable(test_interpreter test/test_interpreter.cpp)
add_executable(main main.cpp)

target_link_libraries(
    test_parser
    libcompiler
)

target_link_libraries(
    test_lexer
    liblexer
)

target_link_libraries(
    test_interpreter
    libcompiler
)

target_link_libraries(
    main
    libcompiler
)