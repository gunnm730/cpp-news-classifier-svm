/**
 * @file tfidf_vectorizer.cpp
 * @brief TF-IDF 向量化器实现（推理模块）/ TF-IDF Vectorizer Implementation (Inference Module)
 * @details 实现TF-IDF模型的加载和文本向量化功能
 * 
 * @note 此模块仅用于模型推理，不包含训练功能
 *       训练功能位于 core 模块
 */

#include "tfidf_vectorizer.hpp"

namespace my_svm {

/**
 * @brief 构造函数
 */
TfidfVectorizer::TfidfVectorizer(size_t max_features) : max_features_(max_features) {
}

/**
 * @brief 转换文本为特征向量
 */
std::vector<sample_type> TfidfVectorizer::transform(const Corpus& corpus) const {
    std::vector<sample_type> res;
    res.reserve(corpus.size());
    
    for (const auto& doc : corpus) {
        std::unordered_map<std::string, size_t> word_counts_doc;
        for (const auto& word : doc) {
            word_counts_doc[word]++;
        }

        sample_type res_doc;
        for (auto& wcd : word_counts_doc) {
            auto it = vocabulary_.find(wcd.first);
            if (it != vocabulary_.end()) {
                size_t word_id = it->second;
                double tf = static_cast<double>(wcd.second) / static_cast<double>(doc.size());
                double tf_idf = tf * idf_vector_[word_id];
                res_doc.emplace_back(word_id, tf_idf);
            }
        }
        res.push_back(res_doc);
    }
    
    return res;
}

/**
 * @brief 保存模型到输出流
 */
bool TfidfVectorizer::save(std::ostream& os) const {
    const char magic[] = "TFID";
    os.write(magic, 4);
    
    uint32_t version = 1;
    os.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    uint64_t vocab_size = vocabulary_.size();
    os.write(reinterpret_cast<const char*>(&vocab_size), sizeof(vocab_size));
    
    for (const auto& pair : vocabulary_) {
        uint64_t word_len = pair.first.size();
        os.write(reinterpret_cast<const char*>(&word_len), sizeof(word_len));
        os.write(pair.first.c_str(), static_cast<std::streamsize>(word_len));
        uint64_t word_id = pair.second;
        os.write(reinterpret_cast<const char*>(&word_id), sizeof(word_id));
    }
    
    uint64_t idf_size = idf_vector_.size();
    os.write(reinterpret_cast<const char*>(&idf_size), sizeof(idf_size));
    os.write(reinterpret_cast<const char*>(idf_vector_.data()), 
             static_cast<std::streamsize>(idf_size * sizeof(double)));
    
    uint64_t id2word_size = id_to_word_.size();
    os.write(reinterpret_cast<const char*>(&id2word_size), sizeof(id2word_size));
    for (const auto& word : id_to_word_) {
        uint64_t word_len = word.size();
        os.write(reinterpret_cast<const char*>(&word_len), sizeof(word_len));
        os.write(word.c_str(), static_cast<std::streamsize>(word_len));
    }
    
    uint64_t max_feat = max_features_;
    os.write(reinterpret_cast<const char*>(&max_feat), sizeof(max_feat));
    
    return os.good();
}

/**
 * @brief 从输入流加载模型
 */
bool TfidfVectorizer::load(std::istream& is) {
    char magic[4];
    is.read(magic, 4);
    if (!is || std::string(magic, 4) != "TFID") {
        return false;
    }
    
    uint32_t version;
    is.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (!is || version != 1) {
        return false;
    }
    
    uint64_t vocab_size;
    is.read(reinterpret_cast<char*>(&vocab_size), sizeof(vocab_size));
    if (!is) return false;
    
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
    
    uint64_t idf_size;
    is.read(reinterpret_cast<char*>(&idf_size), sizeof(idf_size));
    if (!is) return false;
    
    idf_vector_.resize(idf_size);
    is.read(reinterpret_cast<char*>(idf_vector_.data()),
            static_cast<std::streamsize>(idf_size * sizeof(double)));
    if (!is) return false;
    
    uint64_t id2word_size;
    is.read(reinterpret_cast<char*>(&id2word_size), sizeof(id2word_size));
    if (!is) return false;
    
    id_to_word_.resize(id2word_size);
    for (uint64_t i = 0; i < id2word_size; ++i) {
        uint64_t word_len;
        is.read(reinterpret_cast<char*>(&word_len), sizeof(word_len));
        id_to_word_[i].resize(word_len);
        is.read(&id_to_word_[i][0], static_cast<std::streamsize>(word_len));
    }
    if (!is) return false;
    
    uint64_t max_feat;
    is.read(reinterpret_cast<char*>(&max_feat), sizeof(max_feat));
    if (!is) return false;
    max_features_ = max_feat;
    
    return true;
}

} // namespace my_svm
