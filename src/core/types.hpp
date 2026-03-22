/**
 * @file types.hpp
 * @brief 核心模块数据类型定义 / Core module type definitions
 * @details 定义SVM训练和推理所需的基本数据类型
 * 
 * 主要类型说明：
 * - sample_type: 稀疏特征向量表示 (特征ID, TF-IDF值)
 * - Corpus: 语料库类型 (文档集合)
 * - LabelSet: 标签集合类型
 */

#pragma once

#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <unordered_set>

namespace ml {
    
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
     * @brief 标签集合类型
     * @details 使用无序集合存储唯一标签ID，用于一对多分类器
     */
    using LabelSet = std::unordered_set<double>;

} // namespace ml
