/**
 * @file onetoall.cpp
 * @brief 一对多分类器实现 / One-to-All Classifier Implementation
 * @details 实现多分类训练和序列化功能
 */

#include "onetoall.hpp"

// ==================== 构造函数 ====================

/**
 * @brief 构造函数实现
 * @details 为每个类别创建一个SVM分类器
 */
ml::my_cnews_trainer::my_cnews_trainer(const LabelSet& names, size_t max_features, 
                                       double learning_rate, double regularization_coefficient)
    : names_(names)
{
    // 为每个类别标签创建一个SVM分类器
    one_to_all_.reserve(names.size());
    for (double name : names) {
        one_to_all_.emplace_back(name, max_features, learning_rate, regularization_coefficient);
    }
}

// ==================== 训练方法 ====================

/**
 * @brief 训练所有分类器
 * @details 进行多个epoch的训练以提高准确率
 * 
 * @param docs_vector 训练数据的特征向量集合
 * @param labels_vector 对应的标签集合
 */
void ml::my_cnews_trainer::train(const std::vector<sample_type>& docs_vector, 
                                  const std::vector<double>& labels_vector) {
    // 进行3个epoch的训练
    for (int i = 0; i < 3; i++) {
        // 遍历每个分类器进行训练
        for (auto& current_svm : one_to_all_) {
            current_svm.train(docs_vector, labels_vector);
        }
    }
}

// ==================== 预测方法 ====================

/**
 * @brief 预测类别
 * @details 使用所有模型进行预测，选择得分最高的类别
 */
double ml::my_cnews_trainer::predict(const sample_type& current_vector) {
    return predict(current_vector, one_to_all_);
}

/**
 * @brief 使用指定模型进行预测
 * @details 对每个模型计算得分，选择最高得分对应的类别
 * 
 * @param current_vector 待预测的特征向量
 * @param model 使用的SVM模型集合
 * @return 预测的类别标签
 */
double ml::my_cnews_trainer::predict(const sample_type& current_vector, const std::vector<svm>& model) {
    double final_name = -1.0;
    double max_score = -std::numeric_limits<double>::max();
    
    // 遍历所有分类器
    for (const auto& current_svm : model) {
        // 计算当前分类器的决策函数值
        double current_score = current_svm.get_score(current_vector);
        
        // 如果得分更高，更新预测结果
        if (max_score < current_score) {
            final_name = current_svm.name();
            max_score = current_score;
        }
    }
    
    return final_name;
}

// ==================== 序列化方法 ====================

/**
 * @brief 保存完整模型
 * @details 将所有SVM分类器和标签集合序列化到输出流
 * 
 * @param os 输出流引用
 * @return 成功返回true，失败返回false
 */
bool ml::my_cnews_trainer::save(std::ostream& os) const {
    try {
        // 写入Magic标记
        const char magic[] = "SVM ";
        os.write(magic, 4);
        
        // 写入版本号
        uint32_t version = 1;
        os.write(reinterpret_cast<const char*>(&version), sizeof(version));
        
        // 写入模型数量
        uint64_t num_models = one_to_all_.size();
        os.write(reinterpret_cast<const char*>(&num_models), sizeof(num_models));
        
        // 写入每个SVM模型
        for (const auto& model : one_to_all_) {
            if (!model.save(os)) {
                return false;
            }
        }
        
        // 写入标签数量
        uint64_t num_labels = names_.size();
        os.write(reinterpret_cast<const char*>(&num_labels), sizeof(num_labels));
        
        // 写入每个标签
        for (double label : names_) {
            os.write(reinterpret_cast<const char*>(&label), sizeof(label));
        }
        
        return os.good();
    } catch (const std::exception&) {
        return false;
    }
}

/**
 * @brief 加载完整模型
 * @details 从输入流反序列化所有SVM分类器和标签集合
 * 
 * @param is 输入流引用
 * @return 成功返回true，失败返回false
 */
bool ml::my_cnews_trainer::load(std::istream& is) {
    try {
        // 读取并验证Magic标记
        char magic[4];
        is.read(magic, 4);
        if (!is || std::string(magic, 4) != "SVM ") {
            return false;
        }
        
        // 读取版本号
        uint32_t version;
        is.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (!is || version != 1) {
            return false;
        }
        
        // 读取模型数量
        uint64_t num_models;
        is.read(reinterpret_cast<char*>(&num_models), sizeof(num_models));
        if (!is) return false;
        
        // 清空并重新加载所有模型
        one_to_all_.clear();
        one_to_all_.reserve(num_models);
        
        for (uint64_t i = 0; i < num_models; ++i) {
            // 创建临时SVM对象（使用默认参数，后续会覆盖）
            one_to_all_.emplace_back(0.0, 0);
            if (!one_to_all_.back().load(is)) {
                return false;
            }
        }
        
        // 读取标签数量
        uint64_t num_labels;
        is.read(reinterpret_cast<char*>(&num_labels), sizeof(num_labels));
        if (!is) return false;
        
        // 读取所有标签
        names_.clear();
        for (uint64_t i = 0; i < num_labels; ++i) {
            double label;
            is.read(reinterpret_cast<char*>(&label), sizeof(label));
            if (!is) return false;
            names_.insert(label);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}
