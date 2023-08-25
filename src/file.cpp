#include "file.hpp"
#include <fstream>
#include <iterator>

std::string file::readEntire(const std::string& _filename) {
    std::ifstream stream(_filename);
    return std::string(std::istreambuf_iterator<char>(stream),
                       std::istreambuf_iterator<char>());
}