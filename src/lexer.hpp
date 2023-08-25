#pragma once
#include <string>
#include <string_view>
#include <unordered_map>

namespace lexer {
enum TokenType {
    TT_EOF = -1,
    TT_Generic,
    TT_Comment,

    TT_Fn,
    TT_Return,

    TT_Newline,
    TT_Plus,
    TT_Minus,
    TT_Star,
    TT_Slash,
    TT_Equals,
    TT_Comma,
    TT_Semicolon,

    TT_OpeningParen,
    TT_ClosingParen,
    TT_OpeningCurly,
    TT_ClosingCurly,
    TT_OpeningBracket,
    TT_ClosingBracket,

    TT_Identifier,
    TT_NumberLiteral,
};

struct Token {
    int64_t pos              = -1;
    TokenType type           = TT_EOF;
    std::string_view content = "";
};

static const std::unordered_map<std::string_view, TokenType> keywords = {
    {"fn", TT_Fn},
    {"return", TT_Return},
};

Token getToken(const std::string& source, int64_t& pos);
} // namespace lexer

#define INCLUDE_LEXER_FORMAT
#include "format.hpp"