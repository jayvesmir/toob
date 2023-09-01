#pragma once
#include <format>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace parser {
    struct ParseTreeBase {
        std::vector<std::unique_ptr<ParseTreeBase>> children;

        virtual ~ParseTreeBase() = default;
        virtual std::string str() { return "ParseTreeBase"; }
    };

    struct VariableDeclTreeNode : public ParseTreeBase {
        std::string name, type;

        std::string str() { return std::format("{} [{}]", name, type); }
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