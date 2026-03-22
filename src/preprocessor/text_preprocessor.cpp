/**
 * @file text_preprocessor.cpp
 * @brief 文本预处理器实现 / Text Preprocessor Implementation
 * @details 实现文本的清洗、分词和停用词过滤功能
 * 
 * 功能说明：
 * - 使用jieba进行中文分词
 * - 过滤HTML标签和特殊字符
 * - 移除停用词
 * 
 * 预处理流程：
 * 1. 文本清洗（cleaner_.clean）
 *    - 移除HTML标签
 *    - 移除特殊字符
 *    - 规范化空白字符
 * 2. 中文分词（tokenizer_.tokenize）
 *    - 使用jieba进行HMM分词
 * 3. 停用词过滤
 *    - 移除常见的停用词
 */

#include "text_preprocessor.hpp"
#include <fstream>
#include <algorithm>

namespace preprocessor {

/**
 * @brief 构造函数
 * @param jieba_dict_path jieba主词典路径
 * @param hmm_model_path HMM模型路径
 * @param user_dict_path 用户自定义词典路径
 * @param idf_path IDF词典路径
 * @param stopwords_path 停用词表路径
 */
TextPreprocessor::TextPreprocessor(const std::string& jieba_dict_path,
                                    const std::string& hmm_model_path,
                                    const std::string& user_dict_path,
                                    const std::string& idf_path,
                                    const std::string& stopwords_path)
    : jieba_(jieba_dict_path, hmm_model_path, user_dict_path, idf_path, stopwords_path),
      tokenizer_(jieba_),
      cleaner_()
{
    load_stopwords(stopwords_path);
}

/**
 * @brief 预处理文本
 * @param text 原始文本
 * @return 分词后的词向量
 * 
 * 处理步骤：
 * 1. 调用cleaner清理文本（移除HTML、特殊字符等）
 * 2. 调用tokenizer进行分词
 * 3. 过滤停用词和空字符串
 */
std::vector<std::string> TextPreprocessor::preprocess(const std::string& text) {
    std::string cleaned_text = cleaner_.clean(text);
    
    std::vector<std::string> tokens = tokenizer_.tokenize(cleaned_text);
    
    std::vector<std::string> filtered_tokens;
    for (const auto& token : tokens) {
        if (!is_stopword(token) && !token.empty()) {
            filtered_tokens.push_back(token);
        }
    }
    
    return filtered_tokens;
}

/**
 * @brief 加载停用词表
 * @param stopwords_path 停用词文件路径
 */
void TextPreprocessor::load_stopwords(const std::string& stopwords_path) {
    std::ifstream file(stopwords_path);
    if (!file.is_open()) {
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            stopwords_.push_back(line);
        }
    }
    file.close();
}

/**
 * @brief 检查是否为停用词
 * @param word 待检查的词
 * @return 是否为停用词
 */
bool TextPreprocessor::is_stopword(const std::string& word) const {
    return std::find(stopwords_.begin(), stopwords_.end(), word) != stopwords_.end();
}

}
