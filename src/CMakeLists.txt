# toob compiler
cmake_minimum_required(VERSION 3.14)
project(toobc CXX)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    file.cpp
    lexer.cpp
    parser.cpp
)