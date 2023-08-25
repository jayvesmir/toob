#include "lexer.hpp"
#include <cctype>

#define posview(pos, len) std::string_view(_src.data() + pos - 1, len)
#define view(len) posview(_pos, len)
#define shortToken(type)                                                       \
    Token { _pos++, type, view(1) }

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
            return shortToken(TT_Newline);

        case '+':
            if (std::isdigit(_src.at(++_pos))) {
                int64_t len     = 2;
                int64_t lastPos = _pos - 1;
                while (std::isdigit(_src.at(++_pos)) || _src[_pos] == '.')
                    len++;
                return Token{lastPos++, TT_NumberLiteral,
                             posview(lastPos, len)};
            }
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
            return shortToken(TT_Minus);
        case '*':
            return shortToken(TT_Star);
        case '/':
            if (_src.at(++_pos) == '/') {
                int64_t len     = 2;
                int64_t lastPos = _pos - 1;
                while (_src.at(++_pos) != '\n')
                    len++;
                return Token{lastPos++, TT_Comment, posview(lastPos, len)};
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
    }

    return Token{};
}