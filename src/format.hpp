#pragma once
#include <format>
#include <string_view>
#include <unordered_map>

/* lexer namespace */
#if defined(INCLUDE_LEXER_FORMAT)
template <> struct std::formatter<lexer::TokenType> {
  private:
    const std::unordered_map<lexer::TokenType, std::string_view>
        tokenTypeStrings = {
            {lexer::TT_EOF, "EOF"},
            {lexer::TT_Generic, "generic"},
            {lexer::TT_Comment, "comment"},
            {lexer::TT_Fn, "function declaration"},
            {lexer::TT_Return, "return statement"},
            {lexer::TT_Newline, "newline"},
            {lexer::TT_Plus, "plus"},
            {lexer::TT_Minus, "minus"},
            {lexer::TT_Star, "star"},
            {lexer::TT_Slash, "slash"},
            {lexer::TT_Equals, "equals"},
            {lexer::TT_Comma, "comma"},
            {lexer::TT_Semicolon, "semicolon"},
            {lexer::TT_OpeningParen, "opening parenthesis"},
            {lexer::TT_ClosingParen, "closing parenthesis"},
            {lexer::TT_OpeningCurly, "opening curly bracket"},
            {lexer::TT_ClosingCurly, "closing curly bracket"},
            {lexer::TT_OpeningBracket, "opening bracket"},
            {lexer::TT_ClosingBracket, "closing bracket"},
            {lexer::TT_Identifier, "identifier"},
            {lexer::TT_NumberLiteral, "number literal"},
        };

  public:
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const lexer::TokenType& tokenType, std::format_context& ctx) {
        return std::format_to(ctx.out(), "{}",
                              tokenTypeStrings.contains(tokenType)
                                  ? tokenTypeStrings.find(tokenType)->second
                                  : "unknown token type");
    }
};

template <> struct std::formatter<lexer::Token> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const lexer::Token& token, std::format_context& ctx) {
        return std::format_to(
            ctx.out(), "{} at {}: '{}'", token.type, token.pos,
            token.type == lexer::TT_Newline ? "<newline>" : token.content);
    }
};
#endif // defined(INCLUDE_LEXER_FORMAT)