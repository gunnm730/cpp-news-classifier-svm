/**
 * @file svm.hpp
 * @brief SVM 分类器头文件（推理模块）/ SVM Classifier Header (Inference Module)
 * @details 定义用于加载预训练模型并进行推理的SVM分类器
 * 
 * @note 此模块仅用于模型推理，不包含训练功能
 *       训练功能位于 core 模块
 * 
 * 功能说明：
 * - 从文件加载预训练的SVM模型
 * - 对输入特征向量进行分类预测
 * - 支持一对一多分类策略
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include "types.hpp"
#include <vector>
#include <algorithm>
#include <random>
#include <numeric>
#include <limits>
#include <fstream>

namespace my_svm {

    /**
     * @brief 单类别SVM分类器
     * @details 负责判断输入是否属于特定类别
     * 
     * @note 此为简化版本，仅包含推理功能
     */
    class SVM {
    public:
    /**
     * @brief 默认构造函数
     * @details 用于从文件加载模型时的临时构造
     */
    SVM() : name_(0.0), max_features_(0), learning_rate_(0.01), regularization_coefficient_(0.0001), offset_(0.0) {}

        /**
         * @brief 构造函数（已弃用）
         * @details 训练参数不再需要，模型从文件加载
         */
        SVM(double name, size_t max_features, 
            double learning_rate = 0.01, 
            double regularization_coefficient = 0.0001);

        /**
         * @brief 获取类别名称
         */
        double name() const { return name_; }

        /**
         * @brief 获取权重向量
         */
        const sample_type& judgment_vector() const { return judgment_vector_; }

        /**
         * @brief 获取偏置项
         */
        double offset() const { return offset_; }

        /**
         * @brief 获取预测分数
         * @details 计算给定特征向量的决策函数值
         * @param current_vector 输入的特征向量
         * @return 决策函数值
         */
        double get_score(const sample_type& current_vector) const;

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

    private:
        /**
         * @brief 类型标签（保留接口兼容性）
         */
        struct true_type_ {};
        struct false_type_ {};

        /**
         * @brief 权重更新（保留空实现）
         */
        void new_judgment_vector_(const sample_type& current_vector, double y, true_type_);
        void new_judgment_vector_(const sample_type& current_vector, double y, false_type_);
        void new_offset_(const sample_type& current_vector, double y, true_type_);
        void new_offset_(const sample_type& current_vector, double y, false_type_);

        double name_;                                     ///< 类别名称
        sample_type judgment_vector_;                     ///< 权重向量
        double offset_;                                   ///< 偏置项
        double learning_rate_;                            ///< 学习率（保留但不使用）
        double regularization_coefficient_;               ///< 正则化系数（保留但不使用）
        size_t max_features_;                             ///< 最大特征维度
    };

    /**
     * @brief 一对多分类器（推理版本）
     * @details 加载预训练模型并进行多分类预测
     * 
     * @note 此为简化版本，仅包含推理功能
     */
    class OneToAllClassifier {
    public:
        /**
         * @brief 默认构造函数
         */
        OneToAllClassifier();

        /**
         * @brief 构造函数
         * @param names 标签集合
         * @param max_features 最大特征维度
         */
        OneToAllClassifier(const LabelSet& names, size_t max_features = 50000,
                           double learning_rate = 0.01, 
                           double regularization_coefficient = 0.0001);

        /**
         * @brief 预测类别
         * @param current_vector 特征向量
         * @return 预测的类别标签
         */
        double predict(const sample_type& current_vector) const;

        /**
         * @brief 保存模型
         * @param os 输出流
         * @return 成功返回true
         */
        bool save(std::ostream& os) const;

        /**
         * @brief 加载模型
         * @param is 输入流
         * @return 成功返回true
         */
        bool load(std::istream& is);
        
        /**
         * @brief 获取所有模型
         */
        const std::vector<SVM>& get_models() const { return models_; }

        /**
         * @brief 获取标签集合
         */
        const LabelSet& get_labels() const { return labels_; }

    private:
        std::vector<SVM> models_;  ///< SVM模型集合
        LabelSet labels_;           ///< 标签集合
    };

} // namespace my_svm
