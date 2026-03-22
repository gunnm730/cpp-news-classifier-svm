/**
 * @file text_preprocessor.hpp
 * @brief 文本预处理器头文件 / Text Preprocessor Header
 * @details 定义文本预处理的核心接口
 * 
 * 功能说明：
 * - 整合Cleaner、Tokenizer和停用词过滤
 * - 提供统一的文本预处理接口
 * 
 * 依赖模块：
 * - Cleaner: 文本清洗
 * - Tokenizer: 中文分词
 * - cppjieba: 分词库
 * 
 * @see text_preprocessor.cpp - 实现
 */

#pragma once

#include <string>
#include <vector>
#include "cppjieba/Jieba.hpp"
#include "tokenizer.hpp"
#include "cleaner.hpp"

namespace preprocessor {

/**
 * @brief 文本预处理器
 * @details 整合文本清洗、分词和停用词过滤功能
 * 
 * 处理流程：
 * 1. 使用Cleaner清洗文本
 * 2. 使用jieba进行分词
 * 3. 过滤停用词
 */
class TextPreprocessor {
public:
    /**
     * @brief 构造函数
     * @param jieba_dict_path jieba主词典路径
     * @param hmm_model_path HMM模型路径
     * @param user_dict_path 用户自定义词典路径
     * @param idf_path IDF词典路径
     * @param stopwords_path 停用词表路径
     */
    TextPreprocessor(const std::string& jieba_dict_path,
                    const std::string& hmm_model_path,
                    const std::string& user_dict_path,
                    const std::string& idf_path,
                    const std::string& stopwords_path);
    
    /**
     * @brief 预处理文本
     * @param text 原始文本
     * @return 分词后的词向量
     */
    std::vector<std::string> preprocess(const std::string& text);

private:
    cppjieba::Jieba jieba_;                    ///< jieba分词器
    tokenizer::Tokenizer tokenizer_;            ///< 分词器
    cleaner::Cleaner cleaner_;                ///< 文本清洗器
    std::vector<std::string> stopwords_;       ///< 停用词表
    
    /**
     * @brief 加载停用词表
     * @param stopwords_path 停用词文件路径
     */
    void load_stopwords(const std::string& stopwords_path);
    
    /**
     * @brief 检查是否为停用词
     * @param word 待检查的词
     * @return 是否为停用词
     */
    bool is_stopword(const std::string& word) const;
};

}
