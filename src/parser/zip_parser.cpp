/**
 * @file zip_parser.cpp
 * @brief ZIP解析器实现 / ZIP Parser Implementation
 * @details 解析ZIP压缩包，提取其中的文本文件内容
 * 
 * 支持的文件类型：
 * - .txt: 纯文本文件
 * - .doc, .docx: Word文档
 * 
 * 注意：ZIP包中应只包含一个文本文件，否则会抛出异常。
 * 
 * @see zip_parser.hpp - 头文件
 */

#define MINIZ_STATIC
#include "zip_parser.hpp"
#include "txt_parser.hpp"
#include "docx_parser.hpp"
#include <miniz/miniz.h>
#include <cstring>
#include <stdexcept>

namespace parser {

ZipParser::ZipParser() {
    supported_exts_ = { ".txt", ".doc", ".docx" };
}

/**
 * @brief 解析ZIP数据
 * @param data ZIP文件原始数据
 * @return 包含提取文本的Document对象
 * @throws std::runtime_error 如果包含多个文件
 */
common::Document ZipParser::parse(const std::vector<uint8_t>& data) {
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));

    if (!mz_zip_reader_init_mem(&zip, data.data(), data.size(), 0)) {
        throw std::runtime_error("无法解析 ZIP 文件：无效的 ZIP 结构");
    }

    int num_files = mz_zip_reader_get_num_files(&zip);
    std::string combined_text;
    int processed_count = 0;

    for (int i = 0; i < num_files; ++i) {
        mz_zip_archive_file_stat stat;
        if (!mz_zip_reader_file_stat(&zip, i, &stat)) {
            continue;
        }

        std::string filename(stat.m_filename);

        if (mz_zip_reader_is_file_a_directory(&zip, i) || !shouldProcessFile(filename)) {
            continue;
        }

        size_t file_size;
        void* file_data = mz_zip_reader_extract_to_heap(&zip, i, &file_size, 0);
        if (!file_data) {
            continue;
        }

        std::vector<uint8_t> file_bytes(static_cast<uint8_t*>(file_data), 
                                        static_cast<uint8_t*>(file_data) + file_size);
        free(file_data);

        std::shared_ptr<IParser> sub_parser;
        if (filename.find(".txt") != std::string::npos) {
            sub_parser = std::make_shared<TxtParser>();
        } else if (filename.find(".docx") != std::string::npos) {
            sub_parser = std::make_shared<DocxParser>();
        } else if (filename.find(".doc") != std::string::npos) {
            sub_parser = std::make_shared<DocxParser>();
        }

        if (sub_parser) {
            common::Document doc = sub_parser->parse(file_bytes);
            combined_text += doc.text + "\n";
            processed_count++;
        }
    }

    mz_zip_reader_end(&zip);

    if (processed_count > 1) {
        throw std::runtime_error("ZIP包包含多个文件，请单独上传每个文件");
    }

    return common::Document(combined_text);
}

/**
 * @brief 检查是否应该处理该文件
 * @param filename 文件名
 * @return 是否应该处理
 */
bool ZipParser::shouldProcessFile(const std::string& filename) const {
    for (const auto& ext : supported_exts_) {
        if (filename.length() >= ext.length() &&
            filename.compare(filename.length() - ext.length(), ext.length(), ext) == 0) {
            return true;
        }
    }
    return false;
}

} // namespace parser
