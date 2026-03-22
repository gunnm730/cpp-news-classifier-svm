/**
 * @file docx_parser.cpp
 * @brief DOCX解析器实现 / DOCX Parser Implementation
 * @details 解析DOCX文件（基于ZIP格式），提取其中的文本内容
 * 
 * DOCX文件本质上是ZIP压缩包，包含XML格式的文档内容。
 * 本解析器使用miniz库解压并提取word/document.xml中的文本。
 * 
 * @see docx_parser.hpp - 头文件
 */

#define MINIZ_STATIC
#include "docx_parser.hpp"
#include <miniz/miniz.h>
#include <cstring>
#include <algorithm>
#include <stdexcept>

namespace parser {

DocxParser::DocxParser() {
}

/**
 * @brief 解析DOCX数据
 * @param data DOCX文件原始数据
 * @return 包含提取文本的Document对象
 * @throws std::runtime_error 如果ZIP结构无效或document.xml不存在
 */
common::Document DocxParser::parse(const std::vector<uint8_t>& data) {
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));

    if (!mz_zip_reader_init_mem(&zip, data.data(), data.size(), 0)) {
        throw std::runtime_error("无法解析 DOCX 文件：无效的 ZIP 结构");
    }

    int file_index = mz_zip_reader_locate_file(&zip, "word/document.xml", nullptr, 0);
    if (file_index < 0) {
        mz_zip_reader_end(&zip);
        throw std::runtime_error("无法在 DOCX 中找到 word/document.xml");
    }

    size_t xml_size;
    void* xml_data = mz_zip_reader_extract_to_heap(&zip, file_index, &xml_size, 0);
    mz_zip_reader_end(&zip);

    if (!xml_data) {
        throw std::runtime_error("无法从 DOCX 中提取 document.xml");
    }

    std::string xml_content(static_cast<char*>(xml_data), xml_size);
    free(xml_data);

    std::string text = extractTextFromXml(xml_content);
    return common::Document(text);
}

/**
 * @brief 从XML中提取文本内容
 * @param xml_content XML内容
 * @return 提取的纯文本
 */
std::string DocxParser::extractTextFromXml(const std::string& xml_content) {
    std::string result;
    std::string tag = "<w:t";
    size_t pos = 0;

    while ((pos = xml_content.find(tag, pos)) != std::string::npos) {
        size_t end_tag = xml_content.find('>', pos);
        if (end_tag == std::string::npos) break;

        size_t content_start = xml_content.find('>', end_tag) + 1;
        size_t content_end = xml_content.find("</w:t>", content_start);
        if (content_end == std::string::npos) break;

        result += xml_content.substr(content_start, content_end - content_start);
        pos = content_end;
    }

    return result;
}

} // namespace parser
