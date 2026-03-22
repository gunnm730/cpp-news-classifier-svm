/**
 * @file zip_parser.hpp
 * @brief ZIP压缩包解析器 / ZIP Archive Parser
 * @details 实现对ZIP压缩包的解析，支持递归处理其中的文件
 * 
 * 功能说明：
 * - 解析ZIP压缩包格式
 * - 递归处理其中的.txt和.docx文件
 * - 合并所有提取的文本内容
 * 
 * 支持的文件类型：
 * - .txt (纯文本)
 * - .docx (Word文档)
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include "i_parser.hpp"
#include <memory>
#include <string>
#include <vector>

namespace parser {

/**
 * @brief ZIP压缩包解析器
 * @details 从ZIP文件中提取文本内容，支持递归处理嵌套文件
 */
class ZipParser : public IParser {
public:
    ZipParser();
    
    /**
     * @brief 解析ZIP压缩包
     * @param data zip文件的二进制数据
     * @return 包含合并后文本内容的文档对象
     */
    common::Document parse(const std::vector<uint8_t>& data) override;

private:
    /**
     * @brief 判断文件是否应该被处理
     * @param filename 文件名
     * @return 是否支持处理该文件
     */
    bool shouldProcessFile(const std::string& filename) const;
    
    std::vector<std::string> supported_exts_;  ///< 支持的文件扩展名列表
};

} // namespace parser
