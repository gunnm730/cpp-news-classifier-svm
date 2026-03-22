/**
 * @file onetoall.hpp
 * @brief 一对多分类器头文件 / One-to-All Classifier Header
 * @details 定义用于多分类的一对多策略分类器
 * 
 * 算法原理：
 * 一对多（One-to-All）策略将多分类问题转化为多个二分类问题
 * 
 * 对于K分类问题：
 * 1. 创建K个二分类器，每个负责区分"属于类别i"和"不属于类别i"
 * 2. 预测时，对所有K个分类器计算决策函数值
 * 3. 选择得分最高的分类器对应的类别作为最终预测结果
 * 
 * 优点：
 * - 只需训练K个分类器
 * - 每个分类器可以使用标准的二分类SVM
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include "svm.hpp"
#include "types.hpp"
#include <unordered_set>
#include <limits>
#include <fstream>

namespace ml {

    /**
     * @brief 一对多分类器
     * @details 将多分类问题分解为多个二分类问题的分类器
     * 
     * 使用方式：
     * 1. 构造函数传入所有类别标签
     * 2. 调用train()训练所有二分类器
     * 3. 调用predict()进行预测
     * 
     * 成员变量说明：
     * - one_to_all_: SVM分类器向量，每个对应一个类别
     * - names_: 所有类别的标签集合
     */
    class my_cnews_trainer {
    private:
        std::vector<svm> one_to_all_;  ///< SVM分类器集合
        LabelSet names_;                ///< 所有类别标签集合

    public:
        /**
         * @brief 构造函数
         * @param names 所有类别的标签集合
         * @param max_features 最大特征维度（词汇表大小）
         * @param learning_rate 学习率，默认0.01
         * @param regularization_coefficient 正则化系数，默认0.0001
         */
        my_cnews_trainer(const LabelSet& names, size_t max_features = 50000, 
                        double learning_rate = 0.01, double regularization_coefficient = 0.0001);

        /**
         * @brief 训练所有分类器
         * @details 依次训练每个类别的二分类SVM
         * 
         * 训练流程：
         * 1. 进行多个epoch的训练
         * 2. 每个epoch中，遍历所有分类器
         * 3. 每个分类器调用其train方法
         * 
         * @param docs_vector 训练数据的特征向量集合
         * @param labels_vector 对应的标签集合
         */
        void train(const std::vector<sample_type>& docs_vector, const std::vector<double>& labels_vector);

        /**
         * @brief 预测类别
         * @details 使用投票策略选择最终类别
         * 
         * 预测流程：
         * 1. 对每个SVM分类器计算决策函数值
         * 2. 选择得分最高的分类器对应的类别
         * 
         * @param current_vector 待预测的特征向量
         * @return 预测的类别标签
         */
        double predict(const sample_type& current_vector);

        /**
         * @brief 使用指定模型进行预测
         * @details 与predict()类似，但使用传入的模型集合
         * 
         * @param current_vector 待预测的特征向量
         * @param model 指定的SVM模型集合
         * @return 预测的类别标签
         */
        double predict(const sample_type& current_vector, const std::vector<svm>& model);

        /**
         * @brief 保存完整模型
         * @details 将所有SVM分类器和标签集合序列化到文件
         * 
         * 保存格式（按顺序）：
         * 1. Magic标记 "SVM " (4字节)
         * 2. 版本号 version (4字节 uint32_t)
         * 3. 模型数量 num_models (8字节 uint64_t)
         * 4. 每个模型的序列化数据
         * 5. 标签数量 num_labels (8字节 uint64_t)
         * 6. 每个标签的值
         * 
         * @param os 输出流引用
         * @return 成功返回true，失败返回false
         */
        bool save(std::ostream& os) const;

        /**
         * @brief 加载完整模型
         * @details 从文件反序列化所有SVM分类器和标签集合
         * 
         * @param is 输入流引用
         * @return 成功返回true，失败返回false
         */
        bool load(std::istream& is);

        /**
         * @brief 获取所有SVM模型
         * @return SVM模型向量引用
         */
        const std::vector<svm>& get_models() const { return one_to_all_; }

        /**
         * @brief 获取所有标签
         * @return 标签集合引用
         */
        const LabelSet& get_labels() const { return names_; }

        /**
         * @brief 获取分类器数量
         * @return 分类器数量
         */
        size_t size() const { return one_to_all_.size(); }
    };

} // namespace ml
