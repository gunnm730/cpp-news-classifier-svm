/**
 * @file cleaner.cpp
 * @brief 文本清洗器实现 / Text Cleaner Implementation
 * @details 实现HTML移除、特殊字符过滤和空白字符规范化
 * 
 * 功能说明：
 * - 移除HTML标签
 * - 过滤特殊字符，保留中文、英文、数字和基本空白字符
 * - 规范化空白字符（多个空格合并为一个）
 * 
 * 字符保留规则：
 * - 中文：UTF-8编码下0x81-0xFE区间的字符
 * - 英文：a-z, A-Z
 * - 数字：0-9
 * - 空白：空格、制表符、换行符、回车符
 * - 其他ASCII字符：替换为空格
 * - 其他字符：保留
 */

#include "cleaner.hpp"

namespace preprocessor {

namespace cleaner {

/**
 * @brief 构造函数
 * @details 初始化正则表达式模式
 */
Cleaner::Cleaner()
    : html_tag_pattern_("<[^>]+>"),
      special_char_pattern_(""),
      whitespace_pattern_("\\s+") {
}

/**
 * @brief 清洗文本
 * @param text 原始文本
 * @return 清洗后的文本
 * 
 * 清洗流程：
 * 1. 移除HTML标签
 * 2. 移除特殊字符
 * 3. 规范化空白字符
 */
std::string Cleaner::clean(const std::string& text) {
    std::string result = text;
    result = remove_html(result);
    result = remove_special_chars(result);
    result = normalize_whitespace(result);
    return result;
}

/**
 * @brief 移除HTML标签
 * @param text 输入文本
 * @return 移除HTML标签后的文本
 */
std::string Cleaner::remove_html(const std::string& text) {
    return std::regex_replace(text, html_tag_pattern_, " ");
}

/**
 * @brief 移除特殊字符
 * @param text 输入文本
 * @return 移除特殊字符后的文本
 * 
 * 简化版本：保留所有字符，让jieba分词器自行处理
 * 原版本有UTF-8处理bug，导致中文字符被破坏
 */
std::string Cleaner::remove_special_chars(const std::string& text) {
    std::string result;
    result.reserve(text.size());
    
    for (size_t i = 0; i < text.length();) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        
        if (c >= 0xE0 && c <= 0xEF) {
            // UTF-8 3-byte Chinese character (0xE0-0xEF followed by 2 bytes)
            if (i + 2 < text.length()) {
                result += text.substr(i, 3);
                i += 3;
            } else {
                i++;
            }
        } else if (c >= 0xC0 && c <= 0xDF) {
            // UTF-8 2-byte character
            if (i + 1 < text.length()) {
                result += text.substr(i, 2);
                i += 2;
            } else {
                i++;
            }
        } else if (c >= 0xF0 && c <= 0xF7) {
            // UTF-8 4-byte character
            if (i + 3 < text.length()) {
                result += text.substr(i, 4);
                i += 4;
            } else {
                i++;
            }
        } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            result += c;
            i++;
        } else if (c < 128 && std::isalnum(c)) {
            // ASCII alphanumeric
            result += c;
            i++;
        } else if (c < 128) {
            // Other ASCII - replace with space
            result += ' ';
            i++;
        } else {
            // Pass through other bytes
            result += c;
            i++;
        }
    }
    
    return result;
}

/**
 * @brief 规范化空白字符
 * @param text 输入文本
 * @return 规范化后的文本
 * 
 * 将连续的空白字符替换为单个空格，并移除末尾空格
 */
std::string Cleaner::normalize_whitespace(const std::string& text) {
    std::string result = std::regex_replace(text, whitespace_pattern_, " ");
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    return result;
}

/**
 * @brief 保留中文字符和字母数字
 * @param text 输入文本
 * @return 只包含中文和字母数字的文本
 * 
 * 修复UTF-8处理：正确保留多字节中文字符
 */
std::string Cleaner::keep_chinese_and_alphanumeric(const std::string& text) {
    std::string result;
    result.reserve(text.size());
    
    for (size_t i = 0; i < text.length();) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        
        if (c >= 0xE0 && c <= 0xEF) {
            // UTF-8 3-byte Chinese character
            if (i + 2 < text.length()) {
                result += text.substr(i, 3);
                i += 3;
            } else {
                i++;
            }
        } else if (c >= 0xC0 && c <= 0xDF) {
            // UTF-8 2-byte character
            if (i + 1 < text.length()) {
                result += text.substr(i, 2);
                i += 2;
            } else {
                i++;
            }
        } else if (c >= 'a' && c <= 'z') {
            result += c;
            i++;
        } else if (c >= 'A' && c <= 'Z') {
            result += c;
            i++;
        } else if (c >= '0' && c <= '9') {
            result += c;
            i++;
        } else {
            i++;
        }
    }
    return result;
}

}

}
