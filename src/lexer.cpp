#include "lexer.hpp"
#include <cctype>

#define LEX_NEWLINE_AS_WHITESPACE 1
#define LEX_COMMENT_AS_WHITESPACE 1

#define posview(pos, len) std::string_view(_src.data() + pos - 1, len)
#define view(len)         posview(_pos, len)
#define shortToken(type) \
    Token { _pos++, type, view(1) }

bool lexer::Token::isOperator() const {
    return (type == TT_Equals || type == TT_Plus || type == TT_Minus ||
            type == TT_Star || type == TT_Slash);
}

lexer::Token lexer::getToken(const std::string& _src, int64_t& _pos) {
    for (; (uint64_t)_pos < _src.size(); _pos++) {
        switch (_src[_pos]) {
        case ' ':
            [[fallthrough]];
        case '\t':
            [[fallthrough]];
        case '\r':
            continue;

        case '\n':
#if LEX_NEWLINE_AS_WHITESPACE
            continue;
#else
            return shortToken(TT_Newline);
#endif

        case '+':
            if (std::isdigit(_src.at(++_pos))) {
                int64_t len     = 2;
                int64_t lastPos = _pos - 1;
                while (std::isdigit(_src.at(++_pos)) || _src[_pos] == '.')
                    len++;
                return Token{lastPos++, TT_NumberLiteral,
                             posview(lastPos, len)};
            }
            _pos--;
            return shortToken(TT_Plus);
        case '-':
            if (std::isdigit(_src.at(++_pos))) {
                int64_t len     = 2;
                int64_t lastPos = _pos - 1;
                while (std::isdigit(_src.at(++_pos)) || _src[_pos] == '.')
                    len++;
                return Token{lastPos++, TT_NumberLiteral,
                             posview(lastPos, len)};
            }
            _pos--;
            return shortToken(TT_Minus);
        case '*':
            return shortToken(TT_Star);
        case '/':
            if (_src.at(++_pos) == '/') {
                [[maybe_unused]] int64_t len     = 2;
                [[maybe_unused]] int64_t lastPos = _pos - 1;
                while (_src.at(++_pos) != '\n')
                    len++;
#if LEX_COMMENT_AS_WHITESPACE
                continue;
#else
                return Token{lastPos++, TT_Comment, posview(lastPos, len)};
#endif
            }
            return shortToken(TT_Slash);
        case '=':
            return shortToken(TT_Equals);
        case ',':
            return shortToken(TT_Comma);
        case ';':
            return shortToken(TT_Semicolon);

        case '(':
            return shortToken(TT_OpeningParen);
        case ')':
            return shortToken(TT_ClosingParen);
        case '{':
            return shortToken(TT_OpeningCurly);
        case '}':
            return shortToken(TT_ClosingCurly);
        case '[':
            return shortToken(TT_OpeningBracket);
        case ']':
            return shortToken(TT_ClosingBracket);
        }

        if (std::isalpha(_src[_pos])) {
            int64_t len     = 1;
            int64_t lastPos = _pos;
            while (std::isalnum(_src.at(++_pos)))
                len++;
            std::string_view token = posview(lastPos + 1, len);
            return Token{lastPos,
                         keywords.contains(token) ? keywords.find(token)->second
                                                  : TT_Identifier,
                         token};
        }

        if (std::isdigit(_src[_pos])) {
            int64_t len     = 1;
            int64_t lastPos = _pos;
            while (std::isdigit(_src.at(++_pos)) || _src[_pos] == '.')
                len++;
            return Token{lastPos++, TT_NumberLiteral, posview(lastPos, len)};
        }

        throw std::runtime_error(
            std::format("unexpected '{}' at {}", _src[_pos], _pos));
    }

    return Token{};
}