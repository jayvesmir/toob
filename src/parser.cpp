#include "parser.hpp"
#include "lexer.hpp"
#include "utils.hpp"
#include <format>
#include <iostream>

#define unexpected(token)     \
    throw std::runtime_error( \
        std::format("unexpected '{}' at {}", token.content, token.pos))
#define expectToken(token, tokenType) \
    if (token.type != tokenType)      \
    unexpected(token)

static utils::CircularQueue<lexer::Token, 24> tokenBuf;

namespace lx = lexer;
namespace ps = parser;

std::unique_ptr<ps::VariableDeclTreeNode>
parseVariableDecl(const std::string& _src, int64_t& _pos) {
    std::unique_ptr<ps::VariableDeclTreeNode> var =
        std::make_unique<ps::VariableDeclTreeNode>();

    // parse name

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_Identifier);
    var->name = tokenBuf[0].content;

    // parse type

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_OpeningBracket);

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_Identifier);
    var->type = tokenBuf[0].content;

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_ClosingBracket);

    return var;
}

std::unique_ptr<ps::FuncTreeNode> parseFunction(const std::string& _src,
                                                int64_t& _pos) {
    std::unique_ptr<ps::FuncTreeNode> func =
        std::make_unique<ps::FuncTreeNode>();

    // parse name

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_Identifier);
    func->signature.name = tokenBuf[0].content;

    // parse arguments

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_OpeningParen);

    int64_t pos = _pos;
    if (tokenBuf.push(lx::getToken(_src, pos)).type != lx::TT_ClosingParen) {
        do {
            func->signature.args.push_back(parseVariableDecl(_src, _pos));
        } while (tokenBuf.push(lx::getToken(_src, _pos)).type == lx::TT_Comma);
    }

    expectToken(tokenBuf[0], lx::TT_ClosingParen);

    // parse type

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_OpeningBracket);

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_Identifier);
    func->signature.returnType = tokenBuf[0].content;

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_ClosingBracket);

    // parse body

    std::cout << std::format("{}\n", func->str());

    return std::move(func);
}

std::unique_ptr<ps::ParseTreeBase>
ps::generateParseTree(const std::string& _src) {
    int64_t pos = 0;
    lx::Token currentToken;
    std::unique_ptr<ps::ParseTreeBase> parseTree =
        std::make_unique<ps::ParseTreeBase>();

    while ((currentToken = lx::getToken(_src, pos)).type != lx::TT_EOF) {
        const lx::Token& tok = tokenBuf.push(std::move(currentToken));
        // top level statements can only be function declarations
        // for now
        switch (tok.type) {
        case lx::TT_Comment:
            [[fallthrough]];
        case lx::TT_Newline:
            continue;

        case lx::TT_Fn: {
            std::unique_ptr<ps::FuncTreeNode> func = parseFunction(_src, pos);
        } break;
        default:
            unexpected(tok);
        }
    }

    return parseTree;
}