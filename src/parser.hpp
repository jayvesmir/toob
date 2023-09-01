#pragma once
#include <format>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// TODO: make proper formatters

namespace parser {
    struct ParseTreeBase {
        std::vector<std::unique_ptr<ParseTreeBase>> children;

        virtual ~ParseTreeBase() = default;
        virtual std::string str() { return "ParseTreeBase"; }
    };

    enum LiteralType {
        LT_Empty = -1,
        LT_Number,
    };

    struct LiteralTreeNode : public ParseTreeBase {
        std::string literal;
        LiteralType type;

        LiteralTreeNode() = default;
        LiteralTreeNode(const std::string& literal, LiteralType type)
            : literal(literal), type(type) {}

        std::string str() {
            return type == LT_Empty ? "" : std::format("'{}'", literal);
        }
    };

    struct VariableDeclTreeNode : public ParseTreeBase {
        std::string name, type;

        VariableDeclTreeNode() = default;
        VariableDeclTreeNode(const std::string& name, const std::string& type)
            : name(name), type(type) {}

        std::string str() {
            return type == "" ? std::format("{}", name)
                              : std::format("{} [{}]", name, type);
        }
    };

    struct BinaryExprTreeNode : public ParseTreeBase {
        std::string op;
        std::unique_ptr<ParseTreeBase> left, right;

        std::string str() {
            return std::format("{} {} {}", left->str(), op, right->str());
        }
    };

    struct FuncCallTreeNode : public ParseTreeBase {
        std::string name;
        std::vector<std::unique_ptr<ParseTreeBase>> args;

        FuncCallTreeNode() = default;
        FuncCallTreeNode(const std::string& name,
                         std::vector<std::unique_ptr<ParseTreeBase>>&& args)
            : name(name), args(std::move(args)) {}

        std::string str() {
            std::stringstream argss;
            for (const auto& arg : args) {
                argss << arg->str() << ", ";
            }
            argss.seekp(-2, std::ios::end);
            argss << '\0' << '\0';
            return std::format("{}({})", name, argss.str());
        }
    };

    struct FuncTreeNode : public ParseTreeBase {
        struct Signature {
            std::string name;
            std::string returnType;
            std::vector<std::unique_ptr<VariableDeclTreeNode>> args;
        } signature;

        std::string str() {
            std::stringstream argss;
            for (const auto& arg : signature.args) {
                argss << arg->str() << ", ";
            }
            argss.seekp(-2, std::ios::end);
            argss << '\0' << '\0';
            return std::format("function: {}({}) [{}]", signature.name,
                               argss.str(), signature.returnType);
        }
    };

    std::unique_ptr<parser::ParseTreeBase>
    generateParseTree(const std::string& source);
} // namespace parser