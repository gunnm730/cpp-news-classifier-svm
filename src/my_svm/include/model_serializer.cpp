/**
 * @file model_serializer.cpp
 * @brief 模型序列化工具实现 / Model Serializer Implementation
 * @details 提供SVM模型、TF-IDF向量化器和标签映射的保存与加载功能
 * 
 * 功能说明：
 * - 保存和加载TF-IDF向量化器
 * - 保存和加载SVM分类器
 * - 保存和加载标签映射（ID到名称的对应关系）
 * - 一站式保存/加载完整模型
 * 
 * 文件格式：
 * - TF-IDF文件：使用"TFID"魔数开头
 * - SVM文件：使用"SVM "魔数开头
 * - 标签映射文件：使用"LBMP"魔数开头
 * 
 * @see tfidf_vectorizer.cpp - TF-IDF向量化器
 * @see svm.cpp - SVM分类器
 */

#include "model_serializer.hpp"
#include <fstream>
#include <stdexcept>

namespace my_svm {

/**
 * @brief 保存TF-IDF向量化器
 * @param vectorizer 要保存的向量化器
 * @param path 保存路径
 * @return 成功返回true
 */
bool ModelSerializer::saveTfidfVectorizer(const TfidfVectorizer& vectorizer, const std::string& path) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    return vectorizer.save(file);
}

/**
 * @brief 加载TF-IDF向量化器
 * @param vectorizer 要加载到的向量化器对象
 * @param path 文件路径
 * @return 成功返回true
 */
bool ModelSerializer::loadTfidfVectorizer(TfidfVectorizer& vectorizer, const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    return vectorizer.load(file);
}

/**
 * @brief 保存SVM模型
 * @param classifier 要保存的分类器
 * @param path 保存路径
 * @return 成功返回true
 */
bool ModelSerializer::saveSvmModel(const OneToAllClassifier& classifier, const std::string& path) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    return classifier.save(file);
}

/**
 * @brief 加载SVM模型
 * @param classifier 要加载到的分类器对象
 * @param path 文件路径
 * @return 成功返回true
 */
bool ModelSerializer::loadSvmModel(OneToAllClassifier& classifier, const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    return classifier.load(file);
}

/**
 * @brief 保存标签映射
 * @param id_to_label ID到标签名称的映射
 * @param path 保存路径
 * @return 成功返回true
 * 
 * 文件格式：
 * - Magic: "LBMP" (4字节)
 * - Version: uint32_t (4字节)
 * - Size: uint64_t (8字节) - 映射数量
 * - 对于每个映射：
 *   - ID: double (8字节)
 *   - 标签长度: uint64_t (8字节)
 *   - 标签内容: 变长字符串
 */
bool ModelSerializer::saveLabelMap(const std::unordered_map<double, std::string>& id_to_label,
                                   const std::string& path) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    const char magic[] = "LBMP";
    file.write(magic, 4);

    uint32_t version = 1;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));

    uint64_t size = id_to_label.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

    for (const auto& pair : id_to_label) {
        double id = pair.first;
        file.write(reinterpret_cast<const char*>(&id), sizeof(id));

        uint64_t label_len = pair.second.size();
        file.write(reinterpret_cast<const char*>(&label_len), sizeof(label_len));
        file.write(pair.second.c_str(), static_cast<std::streamsize>(label_len));
    }

    return file.good();
}

/**
 * @brief 加载标签映射
 * @param id_to_label 加载到的映射容器
 * @param path 文件路径
 * @return 成功返回true
 */
bool ModelSerializer::loadLabelMap(std::unordered_map<double, std::string>& id_to_label,
                                   const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    char magic[4];
    file.read(magic, 4);
    if (!file || std::string(magic, 4) != "LBMP") {
        return false;
    }

    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (!file || version != 1) {
        return false;
    }

    uint64_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!file) {
        return false;
    }

    id_to_label.clear();
    for (uint64_t i = 0; i < size; ++i) {
        double id;
        uint64_t label_len;
        file.read(reinterpret_cast<char*>(&id), sizeof(id));
        file.read(reinterpret_cast<char*>(&label_len), sizeof(label_len));
        if (!file) return false;

        std::string label;
        label.resize(label_len);
        file.read(&label[0], static_cast<std::streamsize>(label_len));
        if (!file) return false;

        id_to_label[id] = label;
    }

    return true;
}

/**
 * @brief 一站式保存完整模型
 * @param vectorizer TF-IDF向量化器
 * @param classifier SVM分类器
 * @param id_to_label 标签映射
 * @param base_path 基础路径，将生成以下文件：
 *   - {base}_tfidf.bin: TF-IDF向量化器
 *   - {base}_svm.bin: SVM分类器
 *   - {base}_labels.bin: 标签映射
 * @return 全部保存成功返回true
 */
bool ModelSerializer::saveFullModel(const TfidfVectorizer& vectorizer,
                                     const OneToAllClassifier& classifier,
                                     const std::unordered_map<double, std::string>& id_to_label,
                                     const std::string& base_path) {
    std::string tfidf_path = base_path;
    std::string svm_path = base_path;
    std::string label_path = base_path;

    size_t dot_pos = base_path.find_last_of('.');
    if (dot_pos != std::string::npos) {
        std::string base = base_path.substr(0, dot_pos);
        std::string ext = base_path.substr(dot_pos);
        tfidf_path = base + "_tfidf" + ext;
        svm_path = base + "_svm" + ext;
        label_path = base + "_labels" + ext;
    } else {
        tfidf_path = base_path + "_tfidf.bin";
        svm_path = base_path + "_svm.bin";
        label_path = base_path + "_labels.bin";
    }

    return saveTfidfVectorizer(vectorizer, tfidf_path) &&
           saveSvmModel(classifier, svm_path) &&
           saveLabelMap(id_to_label, label_path);
}

/**
 * @brief 一站式加载完整模型
 * @param vectorizer TF-IDF向量化器（引用）
 * @param classifier SVM分类器（引用）
 * @param id_to_label 标签映射（引用）
 * @param base_path 基础路径
 * @return 全部加载成功返回true
 */
bool ModelSerializer::loadFullModel(TfidfVectorizer& vectorizer,
                                     OneToAllClassifier& classifier,
                                     std::unordered_map<double, std::string>& id_to_label,
                                     const std::string& base_path) {
    std::string tfidf_path = base_path;
    std::string svm_path = base_path;
    std::string label_path = base_path;

    size_t dot_pos = base_path.find_last_of('.');
    if (dot_pos != std::string::npos) {
        std::string base = base_path.substr(0, dot_pos);
        std::string ext = base_path.substr(dot_pos);
        tfidf_path = base + "_tfidf" + ext;
        svm_path = base + "_svm" + ext;
        label_path = base + "_labels" + ext;
    } else {
        tfidf_path = base_path + "_tfidf.bin";
        svm_path = base_path + "_svm.bin";
        label_path = base_path + "_labels.bin";
    }

    return loadTfidfVectorizer(vectorizer, tfidf_path) &&
           loadSvmModel(classifier, svm_path) &&
           loadLabelMap(id_to_label, label_path);
}

}
