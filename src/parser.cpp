#include "parser.hpp"
#include "lexer.hpp"
#include "utils.hpp"
#include <format>
#include <iostream>

void parser::generateParseTree(const std::string& _src) {
    namespace lx = lexer;

    int64_t pos = 0;
    lx::Token currentToken;
    utils::CircularQueue<lx::Token, 24> tokenBuf;

    std::cout << std::format("lexed:\n");
    while ((currentToken = lx::getToken(_src, pos)).type != lx::TT_EOF) {
        tokenBuf.push(std::move(currentToken));
        std::cout << std::format("  {}\n", tokenBuf[0]);
    }
}