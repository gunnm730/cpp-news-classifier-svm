/**
 * @file txt_parser.hpp
 * @brief 纯文本文件解析器 / Plain Text Parser
 * @details 实现对.txt等纯文本文件的解析
 * 
 * 功能说明：
 * - 支持UTF-8编码的纯文本文件
 * - 自动处理BOM标记
 * - 移除\r换行符
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include "i_parser.hpp"
#include <memory>
#include <string>

namespace parser {

/**
 * @brief 纯文本文件解析器
 * @details 从纯文本文件中提取文本内容
 */
class TxtParser : public IParser {
public:
    TxtParser();
    
    /**
     * @brief 解析纯文本文件
     * @param data 文件的二进制数据
     * @return 包含文本内容的文档对象
     */
    common::Document parse(const std::vector<uint8_t>& data) override;
};

} // namespace parser
