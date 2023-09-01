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

std::unique_ptr<parser::VariableDeclTreeNode>
parseVariableDecl(const std::string& _src, int64_t& _pos) {
    std::unique_ptr<ps::VariableDeclTreeNode> var =
        std::make_unique<ps::VariableDeclTreeNode>();

    // parse name

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_Identifier);

    // parse type

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_OpeningBracket);

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_Identifier);

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_ClosingBracket);

    var->name = tokenBuf[3].content;
    var->type = tokenBuf[1].content;
    return var;
}

std::unique_ptr<parser::BinaryExprTreeNode>
parseBinaryExpr(const std::string& _src, int64_t& _pos) {
    std::unique_ptr<ps::BinaryExprTreeNode> expr =
        std::make_unique<ps::BinaryExprTreeNode>();

    // parse left-hand side

    // FIXME: actually parse an expression instead of just hacking it (lol)

    const lx::Token& leftToken = tokenBuf.push(lx::getToken(_src, _pos));
    _pos = leftToken.pos; // TODO: make it work without re-lexing
    switch (leftToken.type) {
    case lx::TT_Return: {
        expr->left = std::make_unique<ps::LiteralTreeNode>("", ps::LT_Empty);
        expr->op   = leftToken.content;
        _pos += leftToken.content.length();
        expr->right = parseBinaryExpr(_src, _pos);
        return expr;
    } break;
    case lx::TT_Identifier: {
        // try parsing a variable declaration
        try {
            expr->left = parseVariableDecl(_src, _pos);
        } catch (...) {
            _pos = leftToken.pos + leftToken.content.length();
            // function call
            if (tokenBuf.push(lx::getToken(_src, _pos)).type ==
                lx::TT_OpeningParen) {

                if (tokenBuf.push(lx::getToken(_src, _pos)).type ==
                    lx::TT_ClosingParen) {
                    expr->left = std::make_unique<ps::FuncCallTreeNode>(
                        std::string{leftToken.content},
                        std::vector<std::unique_ptr<ps::ParseTreeBase>>{});
                    expr->op = "call";
                    expr->right =
                        std::make_unique<ps::LiteralTreeNode>("", ps::LT_Empty);
                    return expr;
                }
                _pos = tokenBuf[1].pos + 1;

                std::unique_ptr<ps::FuncCallTreeNode> call =
                    std::make_unique<ps::FuncCallTreeNode>();

                call->name = leftToken.content;

                while (true) {
                    call->args.emplace_back(parseBinaryExpr(_src, _pos));

                    tokenBuf.push(lx::getToken(_src, _pos));
                    if (tokenBuf[0].type == lx::TT_ClosingParen)
                        break;

                    expectToken(tokenBuf[0], lx::TT_Comma);
                    continue;
                }

                expr->left = std::move(call);
                break;
            }

            _pos = tokenBuf[0].pos;

            // TODO: make safer
            expr->left = std::make_unique<ps::VariableDeclTreeNode>(
                std::string{leftToken.content}, "");
        }
    } break;
    case lx::TT_NumberLiteral: {
        expr->left = std::make_unique<ps::LiteralTreeNode>(
            std::string{tokenBuf[0].content}, ps::LT_Number);
        _pos += tokenBuf[0].content.length();
    }
    }

    // parse operator

    tokenBuf.push(lx::getToken(_src, _pos));
    if (tokenBuf[0].isOperator())
        expr->op = tokenBuf[0].content;
    else {
        _pos        = tokenBuf[0].pos;
        expr->right = std::make_unique<ps::LiteralTreeNode>("", ps::LT_Empty);
        return expr;
    }

    // parse right-hand side

    tokenBuf.push(lx::getToken(_src, _pos));
    switch (tokenBuf[0].type) {
    case lx::TT_Identifier: {
        expr->right = std::make_unique<ps::VariableDeclTreeNode>(
            std::string{tokenBuf[0].content}, "");
        _pos = tokenBuf[0].pos + tokenBuf[0].content.length();
    } break;
    case lx::TT_NumberLiteral: {
        expr->right = std::make_unique<ps::LiteralTreeNode>(
            std::string{tokenBuf[0].content}, ps::LT_Number);
    } break;
    }

    return expr;
}

std::unique_ptr<parser::FuncTreeNode> parseFunction(const std::string& _src,
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

    if (tokenBuf.push(lx::getToken(_src, _pos)).type != lx::TT_ClosingParen) {
        _pos = tokenBuf[0].pos;
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

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_ClosingBracket);

    func->signature.returnType = tokenBuf[1].content;
    // parse body

    tokenBuf.push(lx::getToken(_src, _pos));
    expectToken(tokenBuf[0], lx::TT_OpeningCurly);

    std::cout << std::format("{}\n", func->str());

    while (true) {
        func->children.emplace_back(parseBinaryExpr(_src, _pos));

        std::cout << std::format(
            "{};\n", func->children[func->children.size() - 1]->str());

        tokenBuf.push(lx::getToken(_src, _pos));
        if (tokenBuf[0].type != lx::TT_Semicolon)
            throw std::runtime_error(
                std::format("missing ';' at {}",
                            tokenBuf[1].pos + tokenBuf[1].content.length()));

        tokenBuf.push(lx::getToken(_src, _pos));
        if (tokenBuf[0].type == lx::TT_ClosingCurly)
            break;

        _pos = tokenBuf[0].pos;
        continue;
    }

    return func;
}

std::unique_ptr<parser::ParseTreeBase>
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
            parseTree->children.emplace_back(parseFunction(_src, pos));
        } break;
        default:
            unexpected(tok);
        }
    }

    return parseTree;
}