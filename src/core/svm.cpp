/**
 * @file svm.cpp
 * @brief 线性SVM分类器实现 / Linear SVM Classifier Implementation
 * @details 实现线性SVM的训练算法和序列化功能
 */

#include "svm.hpp"

// ==================== 构造函数 ====================

/**
 * @brief 构造函数实现
 * @details 初始化权重向量和偏置项
 */
ml::svm::svm(double name_, size_t max_features_, double learning_rate_, double regularization_coefficient_)
    : name_(name_),
      max_features_(max_features_),
      learning_rate_(learning_rate_),
      regularization_coefficient_(regularization_coefficient_),
      offset_(0.0)
{
    // 预分配权重向量空间
    judgment_vector_.reserve(max_features_);
    for (size_t i = 0; i < max_features_; ++i) {
        judgment_vector_.emplace_back(i, 0.0);
    }
}

// ==================== 核心方法 ====================

/**
 * @brief 获取预测分数
 * @details 计算决策函数值 f(x) = w·x + b
 * 
 * @param current_vector 输入特征向量
 * @return 决策函数值
 */
double ml::svm::get_score(const sample_type& current_vector) const {
    double res = offset_;
    
    // 遍历特征向量的非零元素
    for (const auto& dimension : current_vector) {
        const size_t id = dimension.first;
        // 确保索引在有效范围内
        if (id < judgment_vector_.size()) {
            res += judgment_vector_[id].second * dimension.second;
        }
    }
    
    return res;
}

/**
 * @brief 获取类别名称
 */
double ml::svm::name() const {
    return name_;
}

/**
 * @brief 获取权重向量
 */
const ml::sample_type& ml::svm::judgment_vector() const {
    return judgment_vector_;
}

/**
 * @brief 获取偏置项
 */
const double ml::svm::offset() const {
    return offset_;
}

// ==================== 训练方法 ====================

/**
 * @brief 训练SVM模型
 * @details 使用随机梯度下降法进行训练
 * 
 * 训练流程：
 * 1. 创建样本索引数组并随机打乱（固定种子保证可复现性）
 * 2. 遍历每个样本，计算决策函数值
 * 3. 根据分类是否正确更新权重向量和偏置项
 * 
 * @param docs_vector 训练数据的特征向量集合
 * @param labels_vector 对应的标签集合
 */
void ml::svm::train(const std::vector<sample_type>& docs_vector, const std::vector<double>& labels_vector) {
    // Step 1: 创建样本索引数组
    size_t total_samples = docs_vector.size();
    std::vector<size_t> indices(total_samples);
    std::iota(indices.begin(), indices.end(), 0);

    // Step 2: 使用固定种子随机打乱，保证实验可复现
    unsigned int random_seed = 42;
    std::mt19937 random_engine(random_seed);
    std::shuffle(indices.begin(), indices.end(), random_engine);

    // Step 3: 遍历每个样本进行训练
    for (size_t index : indices) {
        const sample_type& current_vector = docs_vector[index];
        
        // 确定标签：匹配当前SVM负责的类别为+1，否则为-1
        double y = (labels_vector[index] == name()) ? 1.0 : -1.0;
        
        // 计算决策函数值
        double distance = y * get_score(current_vector);
        
        // Step 4: 根据分类情况更新参数
        if (distance >= 1.0) {
            // 分类正确：只进行正则化约束
            new_judgment_vector_(current_vector, y, true_type_());
            new_offset_(current_vector, y, true_type_());
        } else {
            // 分类错误：更新权重向量和偏置项
            new_judgment_vector_(current_vector, y, false_type_());
            new_offset_(current_vector, y, false_type_());
        }
    }
}

// ==================== 权重更新方法 ====================

/**
 * @brief 分类正确时的权重更新（仅正则化）
 * @details 正确分类时：w_new = w_old - η * λ * w_old
 * 为提高效率，本实现省略了此步骤
 */
void ml::svm::new_judgment_vector_(const sample_type& current_vector, double y, true_type_) {
    // 为保证效率，正确分类时不更新权重向量
    // 理论上应执行：w = w - learning_rate_ * regularization_coefficient_ * w
}

/**
 * @brief 分类错误时的权重更新
 * @details 错误分类时：w_new = w_old + η * y * x - η * λ * w_old
 * 
 * 推导来源：
 * - 目标函数 cost(w,b) = max(0, 1-y*(w·x+b)) + (λ/2)||w||²
 * - 错误分类时，对 w 求导得：∇cost(w) = -y*x + λ*w
 * - 梯度下降：w_new = w_old - η * ∇cost(w) = w_old + η*y*x - η*λ*w_old
 */
void ml::svm::new_judgment_vector_(const sample_type& current_vector, double y, false_type_) {
    for (const auto& dimension : current_vector) {
        const size_t id = dimension.first;
        
        // 跳过超出权重向量范围的索引
        if (id >= judgment_vector_.size()) continue;

        double x = dimension.second;
        auto& current_w = judgment_vector_[id].second;
        
        // 应用梯度下降更新公式
        current_w = current_w 
                  + learning_rate_ * y * x 
                  - learning_rate_ * regularization_coefficient_ * current_w;
    }
}

/**
 * @brief 分类正确时的偏置更新（无操作）
 */
void ml::svm::new_offset_(const sample_type& current_vector, double y, true_type_) {
    // 正确分类时偏置项不变
}

/**
 * @brief 分类错误时的偏置更新
 * @details 错误分类时：b_new = b_old - η * (-y)
 * 
 * 推导来源：
 * - 对 b 求导得：∇cost(b) = -y
 * - 梯度下降：b_new = b_old - η * (-y) = b_old + η*y
 */
void ml::svm::new_offset_(const sample_type& current_vector, double y, false_type_) {
    offset_ = offset_ - learning_rate_ * (-y);
}

// ==================== 序列化方法 ====================

/**
 * @brief 保存模型到输出流
 * @details 将模型参数序列化为二进制格式
 * 
 * @param os 输出流引用
 * @return 成功返回true，失败返回false
 */
bool ml::svm::save(std::ostream& os) const {
    try {
        // 写入Magic标记，用于验证文件格式
        const char magic[] = "SVM1";
        os.write(magic, 4);
        
        // 写入模型参数
        os.write(reinterpret_cast<const char*>(&name_), sizeof(name_));
        os.write(reinterpret_cast<const char*>(&offset_), sizeof(offset_));
        os.write(reinterpret_cast<const char*>(&learning_rate_), sizeof(learning_rate_));
        os.write(reinterpret_cast<const char*>(&regularization_coefficient_), sizeof(regularization_coefficient_));
        os.write(reinterpret_cast<const char*>(&max_features_), sizeof(max_features_));
        
        // 写入权重向量
        uint64_t vec_size = judgment_vector_.size();
        os.write(reinterpret_cast<const char*>(&vec_size), sizeof(vec_size));
        
        for (const auto& p : judgment_vector_) {
            os.write(reinterpret_cast<const char*>(&p.first), sizeof(p.first));
            os.write(reinterpret_cast<const char*>(&p.second), sizeof(p.second));
        }
        
        return os.good();
    } catch (const std::exception&) {
        return false;
    }
}

/**
 * @brief 从输入流加载模型
 * @details 从二进制流反序列化模型参数
 * 
 * @param is 输入流引用
 * @return 成功返回true，失败返回false
 */
bool ml::svm::load(std::istream& is) {
    try {
        // 读取并验证Magic标记
        char magic[4];
        is.read(magic, 4);
        if (!is || std::string(magic, 4) != "SVM1") {
            return false;
        }
        
        // 读取模型参数
        is.read(reinterpret_cast<char*>(&name_), sizeof(name_));
        is.read(reinterpret_cast<char*>(&offset_), sizeof(offset_));
        is.read(reinterpret_cast<char*>(&learning_rate_), sizeof(learning_rate_));
        is.read(reinterpret_cast<char*>(&regularization_coefficient_), sizeof(regularization_coefficient_));
        is.read(reinterpret_cast<char*>(&max_features_), sizeof(max_features_));
        
        if (!is) return false;
        
        // 读取权重向量
        uint64_t vec_size;
        is.read(reinterpret_cast<char*>(&vec_size), sizeof(vec_size));
        if (!is) return false;
        
        judgment_vector_.resize(vec_size);
        for (uint64_t i = 0; i < vec_size; ++i) {
            is.read(reinterpret_cast<char*>(&judgment_vector_[i].first), sizeof(size_t));
            is.read(reinterpret_cast<char*>(&judgment_vector_[i].second), sizeof(double));
        }
        
        return is.good();
    } catch (const std::exception&) {
        return false;
    }
}
