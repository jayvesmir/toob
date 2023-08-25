#include "file.hpp"
#include "parser.hpp"
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int32_t main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << std::format("usage:\n\t{} <input_filename>\n", argv[0]);
        return -1;
    }
    std::vector<std::string> args(argv, argv + argc);

    try {
        // assume first argument to be the filename
        std::string fileContents = file::readEntire(args[1]);
        parser::generateParseTree(fileContents);
    } catch (const std::exception& e) {
        std::cout << std::format("[exception] {}", e.what());
        return -1;
    }

    return 0;
}