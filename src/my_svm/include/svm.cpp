/**
 * @file svm.cpp
 * @brief SVM 分类器实现（推理模块）/ SVM Classifier Implementation (Inference Module)
 * @details 实现SVM模型的加载和推理功能
 * 
 * @note 此模块仅用于模型推理，不包含训练功能
 *       训练功能位于 core 模块
 */

#include "svm.hpp"

namespace my_svm {

// ==================== SVM 类实现 ====================

/**
 * @brief 构造函数
 */
SVM::SVM(double name, size_t max_features, 
         double learning_rate, 
         double regularization_coefficient)
    : name_(name), 
      max_features_(max_features),
      learning_rate_(learning_rate), 
      regularization_coefficient_(regularization_coefficient),
      offset_(0.0)
{
    // 预分配权重向量空间
    judgment_vector_.reserve(max_features);
    for (size_t i = 0; i < max_features; ++i) {
        judgment_vector_.emplace_back(i, 0.0);
    }
}

/**
 * @brief 获取预测分数
 */
double SVM::get_score(const sample_type& current_vector) const {
    double res = offset_;
    for (const auto& dimension : current_vector) {
        size_t id = dimension.first;
        if (id < judgment_vector_.size()) {
            res += judgment_vector_[id].second * dimension.second;
        }
    }
    return res;
}

/**
 * @brief 保留的空实现（用于编译兼容性）
 */
void SVM::new_judgment_vector_(const sample_type&, double, true_type_) {
}

/**
 * @brief 保留的空实现（用于编译兼容性）
 */
void SVM::new_judgment_vector_(const sample_type& current_vector, double y, false_type_) {
    // 推理模块不执行训练，此方法保留但不使用
}

/**
 * @brief 保留的空实现（用于编译兼容性）
 */
void SVM::new_offset_(const sample_type&, double, true_type_) {
}

/**
 * @brief 保留的空实现（用于编译兼容性）
 */
void SVM::new_offset_(const sample_type&, double y, false_type_) {
    // 推理模块不执行训练，此方法保留但不使用
}

// ==================== 序列化方法 ====================

/**
 * @brief 保存模型到输出流
 */
bool SVM::save(std::ostream& os) const {
    const char magic[] = "SVM1";
    os.write(magic, 4);
    
    os.write(reinterpret_cast<const char*>(&name_), sizeof(name_));
    os.write(reinterpret_cast<const char*>(&offset_), sizeof(offset_));
    os.write(reinterpret_cast<const char*>(&learning_rate_), sizeof(learning_rate_));
    os.write(reinterpret_cast<const char*>(&regularization_coefficient_), sizeof(regularization_coefficient_));
    os.write(reinterpret_cast<const char*>(&max_features_), sizeof(max_features_));
    
    uint64_t vec_size = judgment_vector_.size();
    os.write(reinterpret_cast<const char*>(&vec_size), sizeof(vec_size));
    for (const auto& p : judgment_vector_) {
        os.write(reinterpret_cast<const char*>(&p.first), sizeof(p.first));
        os.write(reinterpret_cast<const char*>(&p.second), sizeof(p.second));
    }
    
    return os.good();
}

/**
 * @brief 从输入流加载模型
 */
bool SVM::load(std::istream& is) {
    char magic[4];
    is.read(magic, 4);
    if (!is || std::string(magic, 4) != "SVM1") {
        return false;
    }
    
    is.read(reinterpret_cast<char*>(&name_), sizeof(name_));
    is.read(reinterpret_cast<char*>(&offset_), sizeof(offset_));
    is.read(reinterpret_cast<char*>(&learning_rate_), sizeof(learning_rate_));
    is.read(reinterpret_cast<char*>(&regularization_coefficient_), sizeof(regularization_coefficient_));
    is.read(reinterpret_cast<char*>(&max_features_), sizeof(max_features_));
    if (!is) return false;
    
    uint64_t vec_size;
    is.read(reinterpret_cast<char*>(&vec_size), sizeof(vec_size));
    if (!is) return false;
    
    judgment_vector_.resize(vec_size);
    for (uint64_t i = 0; i < vec_size; ++i) {
        is.read(reinterpret_cast<char*>(&judgment_vector_[i].first), sizeof(size_t));
        is.read(reinterpret_cast<char*>(&judgment_vector_[i].second), sizeof(double));
    }
    
    return is.good();
}

// ==================== OneToAllClassifier 类实现 ====================

/**
 * @brief 默认构造函数
 */
OneToAllClassifier::OneToAllClassifier() {
}

/**
 * @brief 构造函数
 */
OneToAllClassifier::OneToAllClassifier(const LabelSet& names, size_t max_features,
                                      double learning_rate, 
                                      double regularization_coefficient)
    : labels_(names)
{
    models_.reserve(names.size());
    for (double name : names) {
        models_.emplace_back(name, max_features, learning_rate, regularization_coefficient);
    }
}

/**
 * @brief 预测类别
 */
double OneToAllClassifier::predict(const sample_type& current_vector) const {
    double final_name = -1.0;
    double max_score = -std::numeric_limits<double>::max();
    for (const auto& model : models_) {
        double current_score = model.get_score(current_vector);
        if (max_score < current_score) {
            final_name = model.name();
            max_score = current_score;
        }
    }
    return final_name;
}

/**
 * @brief 保存模型
 */
bool OneToAllClassifier::save(std::ostream& os) const {
    const char magic[] = "SVM ";
    os.write(magic, 4);
    
    uint32_t version = 1;
    os.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    uint64_t num_models = models_.size();
    os.write(reinterpret_cast<const char*>(&num_models), sizeof(num_models));
    
    for (const auto& model : models_) {
        model.save(os);
    }
    
    uint64_t num_labels = labels_.size();
    os.write(reinterpret_cast<const char*>(&num_labels), sizeof(num_labels));
    for (double label : labels_) {
        os.write(reinterpret_cast<const char*>(&label), sizeof(label));
    }
    
    return os.good();
}

/**
 * @brief 加载模型
 */
bool OneToAllClassifier::load(std::istream& is) {
    char magic[4];
    is.read(magic, 4);
    if (!is || std::string(magic, 4) != "SVM ") {
        return false;
    }
    
    uint32_t version;
    is.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (!is || version != 1) {
        return false;
    }
    
    uint64_t num_models;
    is.read(reinterpret_cast<char*>(&num_models), sizeof(num_models));
    if (!is) return false;
    
    models_.clear();
    models_.reserve(num_models);
    for (uint64_t i = 0; i < num_models; ++i) {
        models_.emplace_back(0.0, 0);
        if (!models_.back().load(is)) {
            return false;
        }
    }
    
    uint64_t num_labels;
    is.read(reinterpret_cast<char*>(&num_labels), sizeof(num_labels));
    if (!is) return false;
    
    labels_.clear();
    for (uint64_t i = 0; i < num_labels; ++i) {
        double label;
        is.read(reinterpret_cast<char*>(&label), sizeof(label));
        if (!is) return false;
        labels_.insert(label);
    }
    
    return true;
}

} // namespace my_svm
