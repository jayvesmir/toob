#include "parser.hpp"
#include "lexer.hpp"
#include <format>
#include <iostream>

void parser::generateParseTree(const std::string& _src) {
    namespace lx = lexer;

    int64_t pos = 0;
    lx::Token currentToken;
    std::cout << std::format("lexed:\n");
    while ((currentToken = lx::getToken(_src, pos)).type != lx::TT_EOF) {
        std::cout << std::format("  {}\n", currentToken);
    }
}