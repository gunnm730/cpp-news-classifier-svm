/**
 * @file tfidf_vectorizer.hpp
 * @brief TF-IDF 向量化器头文件（推理模块）/ TF-IDF Vectorizer Header (Inference Module)
 * @details 定义用于加载预训练TF-IDF模型并进行文本向量化的类
 * 
 * @note 此模块仅用于模型推理，不包含训练功能
 *       训练功能位于 core 模块
 * 
 * 功能说明：
 * - 从文件加载预训练的TF-IDF模型
 * - 将文本转换为特征向量
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include "types.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <fstream>

namespace my_svm {

    /**
     * @brief TF-IDF 文本向量化器（推理版本）
     * @details 加载预训练模型并执行文本向量化
     * 
     * @note 此为简化版本，仅包含推理功能
     */
    class TfidfVectorizer {
    public:
    /**
     * @brief 默认构造函数
     */
    TfidfVectorizer() : max_features_(50000) {}

        /**
         * @brief 构造函数
         * @param max_features 最大特征维度
         */
        explicit TfidfVectorizer(size_t max_features);

        /**
         * @brief 转换文本为特征向量
         * @param corpus 待转换的语料库
         * @return 特征向量集合
         */
        std::vector<sample_type> transform(const Corpus& corpus) const;

        /**
         * @brief 保存模型到输出流
         * @param os 输出流引用
         * @return 成功返回true
         */
        bool save(std::ostream& os) const;

        /**
         * @brief 从输入流加载模型
         * @param is 输入流引用
         * @return 成功返回true
         */
        bool load(std::istream& is);

        /**
         * @brief 获取词汇表
         */
        const Vocabulary& get_vocabulary() const { return vocabulary_; }

        /**
         * @brief 获取IDF向量
         */
        const std::vector<double>& get_idf_vector() const { return idf_vector_; }

        /**
         * @brief 获取最大特征数
         */
        size_t get_max_features() const { return max_features_; }

    private:
        Vocabulary vocabulary_;         ///< 词汇表: 词语 -> 唯一ID
        std::vector<double> idf_vector_;///< IDF向量: ID -> IDF值
        std::vector<std::string> id_to_word_;  ///< ID -> 词语的反向映射
        size_t max_features_;          ///< 最大特征维度
    };

} // namespace my_svm
