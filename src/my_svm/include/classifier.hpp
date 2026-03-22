/**
 * @file classifier.hpp
 * @brief SVM 分类器封装头文件（推理模块）/ SVM Classifier Wrapper Header (Inference Module)
 * @details 提供高层次的SVM模型加载和预测接口
 * 
 * @note 此模块仅用于模型推理，不包含训练功能
 *       训练功能位于 core 模块
 * 
 * 功能说明：
 * - 从文件加载预训练的SVM模型
 * - 将文本分词转换为特征向量
 * - 执行分类预测
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include "types.hpp"
#include "svm.hpp"
#include "tfidf_vectorizer.hpp"
#include <string>
#include <unordered_map>
#include <fstream>

namespace my_svm {

    /**
     * @brief SVM 分类器封装类
     * @details 整合TF-IDF向量化和SVM分类器，提供简化的预测接口
     * 
     * 使用流程：
     * 1. 构造对象
     * 2. 调用 loadModel() 加载预训练模型
     * 3. 调用 predict() 进行预测
     */
    class SvmClassifier {
    public:
        /**
         * @brief 默认构造函数
         */
        SvmClassifier();

        /**
         * @brief 加载模型（完整路径）
         * @details 自动查找同目录下的标签映射文件
         * 
         * 期望文件：
         * - {model_path}: SVM模型文件
         * - {model_path}_labels.bin: 标签映射文件
         * 
         * @param model_path 模型文件路径
         * @throws std::runtime_error 如果文件无法打开或格式错误
         */
        void loadModel(const std::string& model_path);

        /**
         * @brief 预测分类
         * @param tokens 分词后的词语列表
         * @return 预测的类别标签名称
         */
        std::string predict(const std::vector<std::string>& tokens);

        /**
         * @brief 根据ID预测分类
         * @param label_id 类别ID
         * @return 类别标签名称
         */
        std::string predictWithId(double label_id);

        /**
         * @brief 从独立文件加载模型
         * @param model_path SVM模型文件路径
         * @param label_map_path 标签映射文件路径
         * @return 成功返回true
         */
        bool loadModelFromFiles(const std::string& model_path, const std::string& label_map_path);

        /**
         * @brief 设置ID到标签的映射
         */
        void setLabelMap(const std::unordered_map<double, std::string>& label_map);

        /**
         * @brief 设置标签到ID的反向映射
         */
        void setReverseLabelMap(const std::unordered_map<std::string, double>& reverse_map);

    private:
        TfidfVectorizer vectorizer_;                              ///< TF-IDF向量化器
        OneToAllClassifier classifier_;                           ///< SVM分类器
        std::unordered_map<double, std::string> id_to_label_;     ///< ID到标签的映射
        std::unordered_map<std::string, double> label_to_id_;   ///< 标签到ID的映射
        bool loaded_;                                            ///< 模型是否已加载
    };

} // namespace my_svm
