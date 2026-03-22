/**
 * @file txt_parser.cpp
 * @brief 文本解析器实现 / Text Parser Implementation
 * @details 解析纯文本文件，将字节数据转换为Document对象
 * 
 * @see txt_parser.hpp - 头文件
 */

#include "txt_parser.hpp"
#include <stdexcept>

namespace parser {

TxtParser::TxtParser() {
}

/**
 * @brief 解析文本数据
 * @param data 原始字节数据
 * @return 解析后的Document对象
 */
common::Document TxtParser::parse(const std::vector<uint8_t>& data) {
    std::string text(data.begin(), data.end());
    return common::Document(text);
}

} // namespace parser
