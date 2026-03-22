/**
 * @file Tfidfvectorizer.cpp
 * @brief TF-IDF文本向量化器实现 / TF-IDF Text Vectorizer Implementation
 * @details 实现TF-IDF向量化的核心算法和序列化功能
 */

#include "Tfidfvectorizer.hpp"

// ==================== 构造函数 ====================

/**
 * @brief 构造函数实现
 * @param max_features 最大特征维度
 */
ml::TfidfVectorizer::TfidfVectorizer(size_t max_features) : max_features_(max_features) {}

// ==================== fit 方法 ====================

/**
 * @brief 学习阶段实现
 * @details 从训练语料库学习词汇表和IDF值
 * 
 * 学习步骤：
 * 1. 遍历所有文档，统计词频和文档频率
 * 2. 按词频降序排序
 * 3. 取前max_features个词构建词汇表
 * 4. 计算每个词的IDF值
 * 
 * @param training_corpus 训练语料库
 */
void ml::TfidfVectorizer::fit(const Corpus& training_corpus) {
    // Step 1: 统计词频
    // word_counts: 记录每个词出现的总次数
    std::unordered_map<std::string, size_t> word_counts;
    
    // word_counts_doc: 记录每个词出现在多少个文档中（去重）
    std::unordered_map<std::string, size_t> word_counts_doc;

    // 遍历每个文档
    for (const auto& doc : training_corpus) {
        // 记录当前文档中已统计过的词（避免同一文档中重复计数）
        std::unordered_set<std::string> counted_in_doc;
        
        for (const auto& word : doc) {
            // 累加词频
            word_counts[word]++;

            // 如果这个词在这个文档中还没统计过
            if (counted_in_doc.find(word) == counted_in_doc.end()) {
                // 增加文档频率计数
                word_counts_doc[word]++;
                // 标记已统计
                counted_in_doc.insert(word);
            }
        }
    }

    // Step 2: 按词频降序排序
    std::vector<std::pair<std::string, size_t>> vec(word_counts.begin(), word_counts.end());

    std::sort(vec.begin(), vec.end(), [](const std::pair<std::string, size_t>& a, 
                                         const std::pair<std::string, size_t>& b) {
        // 首先按词频降序，若词频相同则按词语升序（保证确定性排序）
        if (a.second != b.second)
            return a.second > b.second;
        else
            return a.first < b.first;
    });

    // Step 3: 预分配空间并选择前max_features个词
    size_t num_features = std::min(max_features_, vec.size());
    
    vocabulary_.clear();
    vocabulary_.reserve(num_features);
    
    id_to_word_.assign(num_features, "");
    idf_vector_.assign(num_features, 0.0);

    double total_docs = static_cast<double>(training_corpus.size());

    // Step 4: 构建词汇表并计算IDF
    for (size_t i = 0; i < num_features; ++i) {
        const std::string& word = vec[i].first;

        // 建立词语到ID的映射
        vocabulary_[word] = i;
        id_to_word_[i] = word;

        // 计算IDF值
        // IDF = log(总文档数 / 包含该词的文档数) + 1
        // 加1是为了防止除零，同时确保所有IDF值至少为1
        size_t doc_count = word_counts_doc.count(word) ? word_counts_doc[word] : 0;
        double idf = log((total_docs + 1.0) / (doc_count + 1.0)) + 1.0;
        idf_vector_[i] = idf;
    }
}

// ==================== transform 方法 ====================

/**
 * @brief 转换阶段实现
 * @details 将文档转换为TF-IDF特征向量
 * 
 * 转换步骤：
 * 1. 统计文档中每个词的词频TF
 * 2. 查找每个词在词汇表中的ID
 * 3. 计算 TF-IDF = TF × IDF
 * 
 * @param corpus 待转换的语料库
 * @return 特征向量集合
 */
std::vector<ml::sample_type> ml::TfidfVectorizer::transform(const Corpus& corpus) const {
    std::vector<ml::sample_type> res;
    res.reserve(corpus.size());

    // 遍历每个文档
    for (const std::vector<std::string>& doc : corpus) {
        // 统计当前文档中每个词的词频
        std::unordered_map<std::string, size_t> word_counts_doc;
        for (const std::string& word : doc) {
            word_counts_doc[word]++;
        }

        // 转换为特征向量
        sample_type res_doc;
        for (auto& wcd : word_counts_doc) {
            // 查找词是否在词汇表中
            auto it = vocabulary_.find(wcd.first);
            
            // 只处理在训练时学到的词汇
            if (it != vocabulary_.end()) {
                size_t word_id = it->second;
                
                // 计算TF
                double tf = static_cast<double>(wcd.second) / static_cast<double>(doc.size());
                
                // 计算TF-IDF
                double tf_idf = tf * idf_vector_[word_id];
                
                // 添加到特征向量
                res_doc.push_back(std::make_pair(word_id, tf_idf));
            }
        }
        
        res.push_back(res_doc);
    }
    
    return res;
}

// ==================== fit_transform 方法 ====================

/**
 * @brief 学习并转换
 * @details 先执行fit学习词汇表，再执行transform转换
 * 
 * @param training_corpus 训练语料库
 * @return 特征向量集合
 */
std::vector<ml::sample_type> ml::TfidfVectorizer::fit_transform(const Corpus& training_corpus) {
    fit(training_corpus);
    return transform(training_corpus);
}

// ==================== 序列化方法 ====================

/**
 * @brief 保存模型到输出流
 * @details 将词汇表和IDF向量序列化为二进制格式
 * 
 * @param os 输出流引用
 * @return 成功返回true，失败返回false
 */
bool ml::TfidfVectorizer::save(std::ostream& os) const {
    try {
        // 写入Magic标记
        const char magic[] = "TFID";
        os.write(magic, 4);
        
        // 写入版本号
        uint32_t version = 1;
        os.write(reinterpret_cast<const char*>(&version), sizeof(version));
        
        // 写入词汇表
        uint64_t vocab_size = vocabulary_.size();
        os.write(reinterpret_cast<const char*>(&vocab_size), sizeof(vocab_size));
        
        for (const auto& pair : vocabulary_) {
            // 写入词语长度
            uint64_t word_len = pair.first.size();
            os.write(reinterpret_cast<const char*>(&word_len), sizeof(word_len));
            // 写入词语内容
            os.write(pair.first.c_str(), static_cast<std::streamsize>(word_len));
            // 写入对应的ID
            uint64_t word_id = pair.second;
            os.write(reinterpret_cast<const char*>(&word_id), sizeof(word_id));
        }
        
        // 写入IDF向量
        uint64_t idf_size = idf_vector_.size();
        os.write(reinterpret_cast<const char*>(&idf_size), sizeof(idf_size));
        os.write(reinterpret_cast<const char*>(idf_vector_.data()), 
                 static_cast<std::streamsize>(idf_size * sizeof(double)));
        
        // 写入ID到词语的反向映射
        uint64_t id2word_size = id_to_word_.size();
        os.write(reinterpret_cast<const char*>(&id2word_size), sizeof(id2word_size));
        for (const auto& word : id_to_word_) {
            uint64_t word_len = word.size();
            os.write(reinterpret_cast<const char*>(&word_len), sizeof(word_len));
            os.write(word.c_str(), static_cast<std::streamsize>(word_len));
        }
        
        // 写入最大特征数
        uint64_t max_feat = max_features_;
        os.write(reinterpret_cast<const char*>(&max_feat), sizeof(max_feat));
        
        return os.good();
    } catch (const std::exception&) {
        return false;
    }
}

/**
 * @brief 从输入流加载模型
 * @details 从二进制流反序列化词汇表和IDF向量
 * 
 * @param is 输入流引用
 * @return 成功返回true，失败返回false
 */
bool ml::TfidfVectorizer::load(std::istream& is) {
    try {
        // 读取并验证Magic标记
        char magic[4];
        is.read(magic, 4);
        if (!is || std::string(magic, 4) != "TFID") {
            return false;
        }
        
        // 读取版本号
        uint32_t version;
        is.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (!is || version != 1) {
            return false;
        }
        
        // 读取词汇表大小
        uint64_t vocab_size;
        is.read(reinterpret_cast<char*>(&vocab_size), sizeof(vocab_size));
        if (!is) return false;
        
        // 读取词汇表
        vocabulary_.clear();
        vocabulary_.reserve(vocab_size);
        for (uint64_t i = 0; i < vocab_size; ++i) {
            uint64_t word_len;
            is.read(reinterpret_cast<char*>(&word_len), sizeof(word_len));
            
            std::string word;
            word.resize(word_len);
            is.read(&word[0], static_cast<std::streamsize>(word_len));
            
            uint64_t word_id;
            is.read(reinterpret_cast<char*>(&word_id), sizeof(word_id));
            
            if (!is) return false;
            vocabulary_[word] = word_id;
        }
        
        // 读取IDF向量大小
        uint64_t idf_size;
        is.read(reinterpret_cast<char*>(&idf_size), sizeof(idf_size));
        if (!is) return false;
        
        // 读取IDF向量
        idf_vector_.resize(idf_size);
        is.read(reinterpret_cast<char*>(idf_vector_.data()),
                static_cast<std::streamsize>(idf_size * sizeof(double)));
        if (!is) return false;
        
        // 读取ID到词语映射大小
        uint64_t id2word_size;
        is.read(reinterpret_cast<char*>(&id2word_size), sizeof(id2word_size));
        if (!is) return false;
        
        // 读取ID到词语映射
        id_to_word_.resize(id2word_size);
        for (uint64_t i = 0; i < id2word_size; ++i) {
            uint64_t word_len;
            is.read(reinterpret_cast<char*>(&word_len), sizeof(word_len));
            id_to_word_[i].resize(word_len);
            is.read(&id_to_word_[i][0], static_cast<std::streamsize>(word_len));
        }
        if (!is) return false;
        
        // 读取最大特征数
        uint64_t max_feat;
        is.read(reinterpret_cast<char*>(&max_feat), sizeof(max_feat));
        if (!is) return false;
        max_features_ = max_feat;
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}
