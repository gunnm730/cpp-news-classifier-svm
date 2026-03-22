/**
 * @file cleaner.hpp
 * @brief 文本清洗器头文件 / Text Cleaner Header
 * @details 定义文本清洗的接口
 * 
 * 功能说明：
 * - 移除HTML标签
 * - 过滤特殊字符
 * - 规范化空白字符
 * 
 * @see cleaner.cpp - 实现
 */

#pragma once

#include <string>
#include <regex>

namespace preprocessor {

namespace cleaner {

/**
 * @brief 文本清洗器
 * @details 提供HTML移除、特殊字符过滤和空白规范化功能
 */
class Cleaner {
public:
    /**
     * @brief 构造函数
     * @details 初始化正则表达式模式
     */
    Cleaner();
    
    /**
     * @brief 清洗文本
     * @param text 原始文本
     * @return 清洗后的文本
     */
    std::string clean(const std::string& text);
    
    /**
     * @brief 移除HTML标签
     * @param text 输入文本
     * @return 移除HTML后的文本
     */
    std::string remove_html(const std::string& text);
    
    /**
     * @brief 移除特殊字符
     * @param text 输入文本
     * @return 移除特殊字符后的文本
     */
    std::string remove_special_chars(const std::string& text);
    
    /**
     * @brief 规范化空白字符
     * @param text 输入文本
     * @return 规范化后的文本
     */
    std::string normalize_whitespace(const std::string& text);
    
    /**
     * @brief 保留中文字符和字母数字
     * @param text 输入文本
     * @return 仅包含中文和字母数字的文本
     */
    std::string keep_chinese_and_alphanumeric(const std::string& text);

private:
    std::regex html_tag_pattern_;     ///< HTML标签正则表达式
    std::regex special_char_pattern_;  ///< 特殊字符正则（未使用）
    std::regex whitespace_pattern_;    ///< 空白字符正则表达式
};

}

}
