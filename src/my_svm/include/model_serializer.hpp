/**
 * @file model_serializer.hpp
 * @brief 模型序列化工具头文件 / Model Serializer Header
 * @details 提供SVM模型、TF-IDF向量化器和标签映射的保存与加载接口
 * 
 * 功能说明：
 * - 独立的静态方法，无状态
 * - 支持组件级别的保存/加载
 * - 支持一站式完整模型保存/加载
 * 
 * @see model_serializer.cpp - 实现
 */

#pragma once

#include "types.hpp"
#include "svm.hpp"
#include "tfidf_vectorizer.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

namespace my_svm {

/**
 * @brief 模型序列化工具类
 * @details 提供模型组件的保存和加载功能
 */
class ModelSerializer {
public:
    /**
     * @brief 保存TF-IDF向量化器
     * @param vectorizer 要保存的向量化器
     * @param path 保存路径
     * @return 成功返回true
     */
    static bool saveTfidfVectorizer(const TfidfVectorizer& vectorizer, const std::string& path);
    
    /**
     * @brief 加载TF-IDF向量化器
     * @param vectorizer 要加载到的对象
     * @param path 文件路径
     * @return 成功返回true
     */
    static bool loadTfidfVectorizer(TfidfVectorizer& vectorizer, const std::string& path);

    /**
     * @brief 保存SVM模型
     * @param classifier 要保存的分类器
     * @param path 保存路径
     * @return 成功返回true
     */
    static bool saveSvmModel(const OneToAllClassifier& classifier, const std::string& path);
    
    /**
     * @brief 加载SVM模型
     * @param classifier 要加载到的对象
     * @param path 文件路径
     * @return 成功返回true
     */
    static bool loadSvmModel(OneToAllClassifier& classifier, const std::string& path);

    /**
     * @brief 保存标签映射
     * @param id_to_label ID到标签的映射
     * @param path 保存路径
     * @return 成功返回true
     */
    static bool saveLabelMap(const std::unordered_map<double, std::string>& id_to_label,
                            const std::string& path);
    
    /**
     * @brief 加载标签映射
     * @param id_to_label 要加载到的容器
     * @param path 文件路径
     * @return 成功返回true
     */
    static bool loadLabelMap(std::unordered_map<double, std::string>& id_to_label,
                            const std::string& path);

    /**
     * @brief 一站式保存完整模型
     * @param vectorizer TF-IDF向量化器
     * @param classifier SVM分类器
     * @param id_to_label 标签映射
     * @param base_path 基础路径
     * @return 全部成功返回true
     */
    static bool saveFullModel(const TfidfVectorizer& vectorizer,
                              const OneToAllClassifier& classifier,
                              const std::unordered_map<double, std::string>& id_to_label,
                              const std::string& base_path);
    
    /**
     * @brief 一站式加载完整模型
     * @param vectorizer TF-IDF向量化器（引用）
     * @param classifier SVM分类器（引用）
     * @param id_to_label 标签映射（引用）
     * @param base_path 基础路径
     * @return 全部成功返回true
     */
    static bool loadFullModel(TfidfVectorizer& vectorizer,
                              OneToAllClassifier& classifier,
                              std::unordered_map<double, std::string>& id_to_label,
                              const std::string& base_path);
};

}
