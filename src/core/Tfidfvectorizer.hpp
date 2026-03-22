/**
 * @file Tfidfvectorizer.hpp
 * @brief TF-IDF文本向量化器头文件 / TF-IDF Text Vectorizer Header
 * @details 定义TF-IDF文本向量化的核心结构和算法
 * 
 * 算法原理：
 * TF-IDF (Term Frequency-Inverse Document Frequency) 是一种用于信息检索的加权方法
 * 
 * - TF (词频): 衡量一个词在文档中的重要程度
 *   TF = 某词在文档中出现次数 / 文档总词数
 * 
 * - IDF (逆文档频率): 衡量一个词在整个语料库中的普遍性
 *   IDF = log(文档总数 + 1) / (包含该词的文档数 + 1) + 1
 * 
 * - TF-IDF = TF × IDF
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <string>
#include <iostream>
#include <utility>
#include <algorithm>
#include <math.h>
#include <fstream>
#include "types.hpp"

namespace ml {

    /**
     * @brief TF-IDF文本向量化器
     * @details 将文本转换为TF-IDF加权的稀疏特征向量
     * 
     * 使用流程：
     * 1. fit(): 根据训练语料库学习词汇表和IDF值
     * 2. transform(): 将新文档转换为特征向量
     * 3. fit_transform(): 合并fit和transform操作
     * 
     * 成员变量说明：
     * - vocabulary_: 词汇表，词语到ID的映射
     * - idf_vector_: IDF值向量，按ID索引
     * - id_to_word_: 反向词汇表，ID到词语的映射（用于调试）
     * - max_features_: 最大特征数限制
     */
    class TfidfVectorizer {
    public:
        /**
         * @brief 构造函数
         * @param max_features 最大特征维度（词汇表大小上限），默认50000
         */
        explicit TfidfVectorizer(size_t max_features = 50000);

        /**
         * @brief 学习阶段
         * @details 根据训练语料库学习词汇表和IDF值
         * 
         * 学习内容：
         * 1. 统计每个词在所有文档中的出现次数
         * 2. 统计每个词出现在多少个文档中
         * 3. 按出现次数排序，选取前max_features个词
         * 4. 计算每个词的IDF值
         * 
         * @param training_corpus 训练语料库
         */
        void fit(const Corpus& training_corpus);

        /**
         * @brief 转换阶段
         * @details 使用学习好的词汇表，将文档转换为TF-IDF特征向量
         * 
         * 转换步骤：
         * 1. 对文档进行分词
         * 2. 统计每个词的词频TF
         * 3. 查找每个词的IDF值
         * 4. 计算TF-IDF = TF × IDF
         * 
         * @param corpus 待转换的语料库
         * @return 特征向量集合
         */
        std::vector<sample_type> transform(const Corpus& corpus) const;

        /**
         * @brief 学习并转换
         * @details 先执行fit再执行transform的便捷方法
         * 
         * @param training_corpus 训练语料库
         * @return 特征向量集合
         */
        std::vector<sample_type> fit_transform(const Corpus& training_corpus);

        /**
         * @brief 保存模型到输出流
         * @details 序列化词汇表和IDF向量到二进制流
         * 
         * 保存格式（按顺序）：
         * 1. Magic标记 "TFID" (4字节)
         * 2. 版本号 version (4字节 uint32_t)
         * 3. 词汇表大小 (8字节 uint64_t)
         * 4. 词汇表内容 (词语长度 + 词语 + ID)
         * 5. IDF向量大小 (8字节 uint64_t)
         * 6. IDF向量数据 (double数组)
         * 7. ID到词语映射大小 (8字节 uint64_t)
         * 8. ID到词语映射内容 (词语长度 + 词语)
         * 9. max_features (8字节 uint64_t)
         * 
         * @param os 输出流引用
         * @return 成功返回true，失败返回false
         */
        bool save(std::ostream& os) const;

        /**
         * @brief 从输入流加载模型
         * @details 从二进制流反序列化词汇表和IDF向量
         * 
         * 加载流程：
         * 1. 读取并验证Magic标记
         * 2. 读取版本号
         * 3. 依次读取词汇表、IDF向量、ID到词语映射
         * 4. 验证数据完整性
         * 
         * @param is 输入流引用
         * @return 成功返回true，失败返回false
         */
        bool load(std::istream& is);

        /**
         * @brief 获取词汇表
         * @return 词汇表引用
         */
        const std::unordered_map<std::string, size_t>& get_vocabulary() const { return vocabulary_; }

        /**
         * @brief 获取IDF向量
         * @return IDF向量引用
         */
        const std::vector<double>& get_idf_vector() const { return idf_vector_; }

        /**
         * @brief 获取最大特征数
         * @return 最大特征数
         */
        size_t get_max_features() const { return max_features_; }

    private:
        std::unordered_map<std::string, size_t> vocabulary_;  ///< 词汇表: 词语 -> 唯一ID
        std::vector<double> idf_vector_;                     ///< IDF向量: ID -> IDF值（向量下标即ID）
        std::vector<std::string> id_to_word_;                ///< ID到词语的反向映射（用于调试）
        size_t max_features_;                                 ///< 最大特征数限制

    };

} // namespace ml
