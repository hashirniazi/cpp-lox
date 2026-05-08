cmake_minimum_required(VERSION 3.10)

project(cpp_lox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cpp_lox 
    src/main.cpp 
    src/Scanner.cpp
    src/Parser.cpp
    src/Interpreter.cpp
)