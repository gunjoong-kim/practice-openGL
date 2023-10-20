#include "common.hpp"
#include <fstream>
#include <sstream>

std::optional<std::string> LoadTextFile(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        SPDLOG_ERROR("failed to open file: {}", filename);
        return {}; // nothing
    }
    std::stringstream text;
    text << fin.rdbuf();
    return text.str(); // 전체 내용이 들어가 있음
}