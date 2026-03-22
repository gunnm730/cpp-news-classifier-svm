/**
 * @file svm.hpp
 * @brief 线性SVM分类器头文件 / Linear SVM Classifier Header
 * @details 定义线性支持向量机分类器的核心结构和算法
 * 
 * 算法原理：
 * 线性SVM通过寻找一个超平面 w·x + b = 0 来分类数据，使得两类样本之间的间隔最大化
 * 
 * 目标函数：min (1/2)||w||² + Σ cost(w,b)
 * 其中 cost(w,b) = max(0, 1 - y*(w·x + b))
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include "types.hpp"
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <iostream>
#include <fstream>

namespace ml {
    
    /**
     * @brief 线性支持向量机分类器
     * @details 实现一分类别的SVM训练和预测
     * 
     * 成员变量说明：
     * - name_: 当前SVM负责分类的类别标签
     * - judgment_vector_: 权重向量 w (稀疏表示)
     * - offset_: 偏置项 b
     * - learning_rate_: 学习率 η
     * - regularization_coefficient_: 正则化系数 λ
     */
    class svm {
    public:
        /**
         * @brief 构造函数
         * @param name_ SVM负责分类的类别名称/ID
         * @param max_features_ 最大特征维度（词汇表大小）
         * @param learning_rate_ 学习率，默认0.01
         * @param regularization_coefficient_ 正则化系数，默认0.0001
         */
        svm(double name_, size_t max_features_, double learning_rate_ = 0.01, double regularization_coefficient_ = 0.0001);

        /**
         * @brief 获取SVM负责的类别名称
         * @return 类别名称/ID
         */
        double name() const;

        /**
         * @brief 获取权重向量（判定向量）
         * @return 权重向量引用
         */
        const sample_type& judgment_vector() const;

        /**
         * @brief 获取偏置项
         * @return 偏置值 b
         */
        const double offset() const;

        /**
         * @brief 获取预测分数
         * @details 计算给定特征向量的决策函数值：w·x + b
         * @param current_vector 输入的特征向量
         * @return 决策函数值，大于0倾向于正类
         */
        double get_score(const sample_type& current_vector) const;

        /**
         * @brief 训练SVM模型
         * @details 使用随机梯度下降法训练模型
         * - 遍历所有样本，计算决策函数值
         * - 根据是否分类正确更新权重向量和偏置项
         * 
         * @param docs_vector 训练文档的特征向量集合
         * @param labels_vector 对应的标签集合
         */
        void train(const std::vector<sample_type>& docs_vector, const std::vector<double>& labels_vector);

        /**
         * @brief 保存模型到输出流
         * @details 序列化模型参数到二进制流
         * 
         * 保存格式（按顺序）：
         * 1. Magic标记 "SVM1" (4字节)
         * 2. name_ (8字节 double)
         * 3. offset_ (8字节 double)
         * 4. learning_rate_ (8字节 double)
         * 5. regularization_coefficient_ (8字节 double)
         * 6. max_features_ (8字节 size_t)
         * 7. judgment_vector_ 大小 (8字节 uint64_t)
         * 8. judgment_vector_ 内容 (pairs: size_t + double)
         * 
         * @param os 输出流引用
         * @return 成功返回true，失败返回false
         */
        bool save(std::ostream& os) const;

        /**
         * @brief 从输入流加载模型
         * @details 从二进制流反序列化模型参数
         * 
         * 加载流程：
         * 1. 读取并验证Magic标记
         * 2. 依次读取所有参数
         * 3. 验证数据完整性
         * 
         * @param is 输入流引用
         * @return 成功返回true，失败返回false
         */
        bool load(std::istream& is);

    private:
        /**
         * @brief 类型标签结构体（用于编译时多态）
         * @details 用于区分分类正确和错误两种情况
         */
        struct true_type_ {};  ///< 分类正确时的处理标记
        struct false_type_ {}; ///< 分类错误时的处理标记

        /**
         * @brief 更新权重向量（分类正确情况）
         * @details 当样本分类正确时，仅进行正则化约束
         * w_new = w_old - η * λ * w_old
         * @param current_vector 当前特征向量
         * @param y 标签值 (+1 或 -1)
         */
        void new_judgment_vector_(const sample_type& current_vector, double y, true_type_);

        /**
         * @brief 更新权重向量（分类错误情况）
         * @details 当样本分类错误时，进行权重更新
         * w_new = w_old + η * y * x - η * λ * w_old
         * @param current_vector 当前特征向量
         * @param y 标签值 (+1 或 -1)
         */
        void new_judgment_vector_(const sample_type& current_vector, double y, false_type_);

        /**
         * @brief 更新偏置项（分类正确情况）
         * @details 正确分类时偏置项不变
         * @param current_vector 当前特征向量
         * @param y 标签值 (+1 或 -1)
         */
        void new_offset_(const sample_type& current_vector, double y, true_type_);

        /**
         * @brief 更新偏置项（分类错误情况）
         * @details 错误分类时更新偏置项
         * b_new = b_old - η * (-y)
         * @param current_vector 当前特征向量
         * @param y 标签值 (+1 或 -1)
         */
        void new_offset_(const sample_type& current_vector, double y, false_type_);

        // ==================== 成员变量 ====================
        
        double name_;                                     ///< SVM负责的类别名称/ID
        sample_type judgment_vector_;                     ///< 权重向量 w（稀疏表示）
        double offset_;                                   ///< 偏置项 b
        double learning_rate_;                            ///< 学习率 η
        double regularization_coefficient_;               ///< 正则化系数 λ
        size_t max_features_;                             ///< 最大特征维度

    };
} // namespace ml
