/**
 * @file tokenizer.hpp
 * @brief 分词器头文件 / Tokenizer Header
 * @details 定义基于jieba的中文分词接口
 * 
 * 功能说明：
 * - 使用jieba进行中文分词
 * - 支持HMM分词模式
 * 
 * 依赖：
 * - cppjieba: 中文分词库
 * 
 * @see tokenizer.cpp - 实现
 */

#pragma once

#include <string>
#include <vector>
#include "cppjieba/Jieba.hpp"

namespace preprocessor {

namespace tokenizer {

/**
 * @brief 分词器
 * @details 基于jieba的中文分词封装
 */
class Tokenizer {
public:
    /**
     * @brief 构造函数
     * @param jieba jieba分词器引用
     */
    explicit Tokenizer(cppjieba::Jieba& jieba);
    
    /**
     * @brief 分词
     * @param text 输入文本
     * @return 分词结果列表
     */
    std::vector<std::string> tokenize(const std::string& text);
    
    /**
     * @brief 分词（可控制HMM）
     * @param text 输入文本
     * @param use_hmm 是否使用HMM分词
     * @return 分词结果列表
     */
    std::vector<std::string> tokenize_with_hmm(const std::string& text, bool use_hmm = true);

private:
    cppjieba::Jieba& jieba_;  ///< jieba分词器引用
};

}

}
