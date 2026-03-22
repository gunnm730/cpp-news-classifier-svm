/**
 * @file classifier.cpp
 * @brief SVM 分类器封装实现（推理模块）/ SVM Classifier Wrapper Implementation (Inference Module)
 * @details 实现SVM模型的加载和预测功能
 * 
 * @note 此模块仅用于模型推理，不包含训练功能
 *       训练功能位于 core 模块
 */

#include "classifier.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace my_svm {

/**
 * @brief 默认构造函数
 */
SvmClassifier::SvmClassifier() : loaded_(false) {
    LabelSet empty_labels;
    classifier_ = OneToAllClassifier(empty_labels, 50000, 0.01, 0.005);
}

/**
 * @brief 加载模型
 * @param model_path 模型文件路径，如 "model/svm_model.bin"
 * 
 * 期望文件：
 * - {base_path}: SVM模型，如 "model/svm_model.bin"
 * - {prefix}_tfidf.bin: TF-IDF向量化器，如 "model/svm_model_tfidf.bin"
 * - {prefix}_labels.bin: 标签映射，如 "model/svm_model_labels.bin"
 */
void SvmClassifier::loadModel(const std::string& model_path) {
    size_t last_dot = model_path.find_last_of('.');
    std::string prefix = (last_dot != std::string::npos) 
        ? model_path.substr(0, last_dot) 
        : model_path;
    
    std::string tfidf_path = prefix + "_tfidf.bin";
    std::string svm_path = model_path;
    std::string label_path = prefix + "_labels.bin";
    
    std::cout << "[my_svm] 开始加载模型..." << std::endl;
    std::cout << "[my_svm] SVM模型: " << svm_path << std::endl;
    std::cout << "[my_svm] TF-IDF模型: " << tfidf_path << std::endl;
    std::cout << "[my_svm] 标签映射: " << label_path << std::endl;
    
    std::ifstream tfidf_file(tfidf_path, std::ios::binary);
    if (!tfidf_file.is_open()) {
        throw std::runtime_error("无法打开TF-IDF文件: " + tfidf_path);
    }
    if (!vectorizer_.load(tfidf_file)) {
        throw std::runtime_error("TF-IDF文件格式错误: " + tfidf_path);
    }
    tfidf_file.close();
    std::cout << "[my_svm] TF-IDF模型加载成功" << std::endl;

    std::ifstream model_file(svm_path, std::ios::binary);
    if (!model_file.is_open()) {
        throw std::runtime_error("无法打开模型文件: " + svm_path);
    }
    if (!classifier_.load(model_file)) {
        throw std::runtime_error("SVM模型格式错误: " + svm_path);
    }
    model_file.close();
    std::cout << "[my_svm] SVM模型加载成功" << std::endl;

    std::ifstream label_file(label_path, std::ios::binary);
    if (label_file.is_open()) {
        char magic[4];
        label_file.read(magic, 4);
        
        uint32_t version;
        label_file.read(reinterpret_cast<char*>(&version), sizeof(version));
        
        uint64_t size;
        label_file.read(reinterpret_cast<char*>(&size), sizeof(size));
        
        id_to_label_.clear();
        std::cout << "[my_svm] 正在加载 " << size << " 个标签映射..." << std::endl;
        
        for (uint64_t i = 0; i < size; ++i) {
            double id;
            uint64_t label_len;
            label_file.read(reinterpret_cast<char*>(&id), sizeof(id));
            label_file.read(reinterpret_cast<char*>(&label_len), sizeof(label_len));
            std::string label;
            label.resize(label_len);
            label_file.read(&label[0], static_cast<std::streamsize>(label_len));
            id_to_label_[id] = label;
            label_to_id_[label] = id;
            std::cout << "[my_svm]   标签 " << i << ": ID=" << id << ", Name=\"" << label << "\"" << std::endl;
        }
        label_file.close();
        std::cout << "[my_svm] 标签映射加载成功，共 " << id_to_label_.size() << " 个标签" << std::endl;
    } else {
        std::cerr << "[my_svm] 警告: 无法打开标签映射文件: " << label_path << std::endl;
    }

    loaded_ = true;
    std::cout << "[my_svm] 模型加载完成" << std::endl;
}

/**
 * @brief 预测分类
 * @param tokens 分词后的词向量
 * @return 预测的类别标签名称
 * 
 * 预测流程：
 * 1. 检查模型是否已加载
 * 2. 检查输入tokens是否为空
 * 3. 使用TF-IDF向量化器将tokens转换为特征向量
 * 4. 使用SVM分类器进行预测
 * 5. 根据预测的类别ID查找对应的标签名称
 */
std::string SvmClassifier::predict(const std::vector<std::string>& tokens) {
    if (!loaded_) {
        throw std::runtime_error("模型未加载");
    }

    if (tokens.empty()) {
        // ========== DEBUG OUTPUT ==========
        std::cerr << "[DEBUG] Tokens empty, returning 'unknown'" << std::endl << std::flush;
        // ========== END DEBUG ==========
        return "unknown";
    }
    
    // ========== DEBUG OUTPUT ==========
    std::cerr << "[DEBUG] Transforming " << tokens.size() << " tokens to TF-IDF vector..." << std::endl << std::flush;
    // ========== END DEBUG ==========
    
    Corpus corpus = {tokens};
    std::vector<sample_type> vectors = vectorizer_.transform(corpus);

    // ========== DEBUG OUTPUT ==========
    std::cerr << "[DEBUG] TF-IDF vectors count: " << vectors.size() << std::endl << std::flush;
    if (!vectors.empty()) {
        std::cerr << "[DEBUG] First vector size: " << vectors[0].size() << std::endl << std::flush;
        if (vectors[0].empty()) {
            std::cerr << "[DEBUG] WARNING: TF-IDF vector is EMPTY!" << std::endl << std::flush;
        }
    }
    // ========== END DEBUG ==========

    if (vectors.empty() || vectors[0].empty()) {
        // ========== DEBUG OUTPUT ==========
        std::cerr << "[DEBUG] TF-IDF vector empty, returning 'unknown'" << std::endl << std::flush;
        // ========== END DEBUG ==========
        return "unknown";
    }

    double predicted_id = classifier_.predict(vectors[0]);
    
    // ========== DEBUG OUTPUT ==========
    std::cerr << "[DEBUG] Predicted ID: " << predicted_id << std::endl << std::flush;
    // ========== END DEBUG ==========
    
    return predictWithId(predicted_id);
}

/**
 * @brief 根据ID获取标签
 */
std::string SvmClassifier::predictWithId(double label_id) {
    // ========== DEBUG OUTPUT ==========
    std::cerr << "[DEBUG] Looking up label for ID: " << label_id << std::endl << std::flush;
    std::cerr << "[DEBUG] Total labels in map: " << id_to_label_.size() << std::endl << std::flush;
    // ========== END DEBUG ==========
    
    auto it = id_to_label_.find(label_id);
    if (it != id_to_label_.end()) {
        // ========== DEBUG OUTPUT ==========
        std::cerr << "[DEBUG] Found label: " << it->second << std::endl << std::flush;
        // ========== END DEBUG ==========
        return it->second;
    }
    std::cerr << "[my_svm] 警告: 未找到ID=" << label_id << " 对应的标签" << std::endl << std::flush;
    return "unknown";
}

/**
 * @brief 从独立文件加载模型
 */
bool SvmClassifier::loadModelFromFiles(const std::string& model_path, const std::string& label_map_path) {
    std::ifstream model_file(model_path, std::ios::binary);
    if (!model_file.is_open()) {
        return false;
    }

    if (!classifier_.load(model_file)) {
        return false;
    }

    std::ifstream label_file(label_map_path, std::ios::binary);
    if (label_file.is_open()) {
        char magic[4];
        label_file.read(magic, 4);
        
        uint32_t version;
        label_file.read(reinterpret_cast<char*>(&version), sizeof(version));
        
        uint64_t size;
        label_file.read(reinterpret_cast<char*>(&size), sizeof(size));
        
        id_to_label_.clear();
        for (uint64_t i = 0; i < size; ++i) {
            double id;
            uint64_t label_len;
            label_file.read(reinterpret_cast<char*>(&id), sizeof(id));
            label_file.read(reinterpret_cast<char*>(&label_len), sizeof(label_len));
            std::string label;
            label.resize(label_len);
            label_file.read(&label[0], static_cast<std::streamsize>(label_len));
            id_to_label_[id] = label;
            label_to_id_[label] = id;
        }
    }

    loaded_ = true;
    return true;
}

/**
 * @brief 设置标签映射
 */
void SvmClassifier::setLabelMap(const std::unordered_map<double, std::string>& label_map) {
    id_to_label_ = label_map;
    label_to_id_.clear();
    for (const auto& pair : label_map) {
        label_to_id_[pair.second] = pair.first;
    }
}

/**
 * @brief 设置反向标签映射
 */
void SvmClassifier::setReverseLabelMap(const std::unordered_map<std::string, double>& reverse_map) {
    label_to_id_ = reverse_map;
    id_to_label_.clear();
    for (const auto& pair : reverse_map) {
        id_to_label_[pair.second] = pair.first;
    }
}

} // namespace my_svm
