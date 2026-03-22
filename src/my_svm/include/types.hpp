/**
 * @file types.hpp
 * @brief my_svm 模块数据类型定义 / my_svm Module Type Definitions
 * @details 定义SVM推理模块所需的基本数据类型
 * 
 * @note 此模块仅用于模型加载和推理，不包含训练功能
 *       训练功能位于 core 模块
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <unordered_set>

namespace my_svm {

    /**
     * @brief 稀疏特征向量类型
     * @details 使用 (特征ID, 权重值) 对的向量表示稀疏矩阵
     * 
     * 示例：
     * @code
     * // 表示一个包含3个非零元素的向量
     * sample_type vec = {{0, 0.5}, {5, 1.2}, {99, 0.8}};
     * @endcode
     */
    using sample_type = std::vector<std::pair<size_t, double>>;

    /**
     * @brief 语料库类型
     * @details 二维向量：第一维是文档列表，第二维是文档分词后的词语列表
     * 
     * 结构示例：
     * @code
     * Corpus corpus = {
     *     {"体育", "女排", "奥运"},   // 文档1
     *     {"财经", "股市", "投资"}    // 文档2
     * };
     * @endcode
     */
    using Corpus = std::vector<std::vector<std::string>>;

    /**
     * @brief 词汇表类型
     * @details 词语到ID的映射
     */
    using Vocabulary = std::unordered_map<std::string, size_t>;

    /**
     * @brief 标签集合类型
     * @details 使用无序集合存储唯一标签ID
     */
    using LabelSet = std::unordered_set<double>;

} // namespace my_svm
