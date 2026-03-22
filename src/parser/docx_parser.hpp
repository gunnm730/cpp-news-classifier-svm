/**
 * @file docx_parser.hpp
 * @brief Word文档解析器 / DOCX Parser
 * @details 实现对.docx文件的解析和文本提取
 * 
 * 功能说明：
 * - .docx文件本质上是ZIP压缩包
 * - 文本内容存储在 word/document.xml 中
 * - 提取XML中的<w:t>标签内容
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
 * @brief Word文档解析器
 * @details 从.docx文件中提取文本内容
 */
class DocxParser : public IParser {
public:
    DocxParser();
    
    /**
     * @brief 解析Word文档
     * @param data docx文件的二进制数据
     * @return 包含文本内容的文档对象
     */
    common::Document parse(const std::vector<uint8_t>& data) override;

private:
    /**
     * @brief 从XML内容中提取文本
     * @param xml_content XML格式的文档内容
     * @return 提取的纯文本
     */
    std::string extractTextFromXml(const std::string& xml_content);
};

} // namespace parser
