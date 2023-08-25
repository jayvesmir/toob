#include "file.hpp"
#include <format>
#include <fstream>
#include <iterator>

std::string file::readEntire(const std::string& _filename) {
    std::ifstream stream(_filename);
    if (!stream)
        throw std::runtime_error(std::format("couldn't open {}", _filename));
    return std::string(std::istreambuf_iterator<char>(stream),
                       std::istreambuf_iterator<char>());
}