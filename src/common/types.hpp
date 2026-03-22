/**
 * @file types.hpp
 * @brief 公共类型定义 / Common type definitions
 */

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace common {

enum class Category {
    NEWS = 0,
    MEDICAL = 1,
    TECH = 2
};

enum class FileType {
    UNKNOWN = -1,
    TXT = 0,
    DOC = 1,
    DOCX = 2,
    ZIP = 3
};

struct Result {
    std::string label;
    Result() = default;
    Result(const std::string& lbl) : label(lbl) {}
};

struct Document {
    std::string text;
    std::vector<std::string> tokens;
    Document() = default;
    explicit Document(const std::string& txt) : text(txt) {}
};

struct ClassificationTask {
    Category category;
    std::string model_id;
    bool is_file_mode;
    FileType file_type;
    std::string raw_text;
    std::vector<uint8_t> raw_data;
};

} // namespace common
