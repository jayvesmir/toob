#include "file.hpp"
#include "parser.hpp"
#include <format>
#include <iostream>
#include <stdexcept>

int32_t main(int argc, char** argv) {
    try {
        std::string fileContents = file::readEntire(argv[1]);
        parser::generateParseTree(fileContents);
    } catch (const std::exception& e) {
        std::cout << std::format("[exception] {}", e.what());
        return -1;
    }

    return 0;
}