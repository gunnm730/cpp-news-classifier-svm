/**
 * @file svm_predictor.cpp
 * @brief SVM预测器实现 / SVM Predictor Implementation
 * @details 封装文本预处理和SVM分类功能，提供统一的预测接口
 * 
 * 功能说明：
 * - 从配置文件加载jieba分词器参数
 * - 使用jieba对输入文本进行中文分词
 * - 调用SVM分类器进行文本分类预测
 * 
 * 依赖模块：
 * - TextPreprocessor: 文本预处理（分词、停用词过滤）
 * - SvmClassifier: SVM分类器（TF-IDF向量化和分类）
 * 
 * @see proxy_manager.cpp - 代理管理器
 * @see classifier.cpp - SVM分类器
 */

#include "svm_predictor.hpp"
#include <fstream>
#include <iostream>
#include "json/single_include/nlohmann/json.hpp"

namespace proxy {

/**
 * @brief 构造函数
 * @param model_path SVM模型文件路径
 * 
 * 初始化流程：
 * 1. 从config/paths.json加载分词器配置
 * 2. 创建TextPreprocessor实例
 * 3. 加载SVM模型文件
 */
SvmPredictor::SvmPredictor(const std::string& model_path)
    : model_path_(model_path) {
    loadConfig();
    classifier_.loadModel(model_path_);
}

/**
 * @brief 加载配置文件
 * @throws std::runtime_error 配置文件不存在或格式错误
 * 
 * 从config/paths.json读取jieba分词器所需的各种词典路径：
 * - jieba_dict_path: jieba主词典路径
 * - hmm_model_path: HMM模型路径
 * - user_dict_path: 用户自定义词典路径
 * - idf_path: IDF词典路径
 * - stopwords_path: 停用词表路径
 */
void SvmPredictor::loadConfig() {
    std::ifstream config_file("config/paths.json");
    if (!config_file.is_open()) {
        throw std::runtime_error("Cannot open config file: config/paths.json");
    }
    
    nlohmann::json config;
    config_file >> config;
    
    std::string jieba_dict_path = config["jieba_dict_path"].get<std::string>();
    std::string hmm_model_path = config["hmm_model_path"].get<std::string>();
    std::string user_dict_path = config["user_dict_path"].get<std::string>();
    std::string idf_path = config["idf_path"].get<std::string>();
    std::string stopwords_path = config["stopwords_path"].get<std::string>();

    preprocessor_ = std::make_unique<preprocessor::TextPreprocessor>(
        jieba_dict_path,
        hmm_model_path,
        user_dict_path,
        idf_path,
        stopwords_path
    );
}

/**
 * @brief 预测文本类别
 * @param text 待分类的文本
 * @param model_id 模型标识符（预留参数，当前未使用）
 * @return 预测的类别标签名称
 * 
 * 预测流程：
 * 1. 调用TextPreprocessor对文本进行预处理（分词、去停用词）
 * 2. 调用SvmClassifier对分词结果进行分类
 * 3. 返回分类结果
 * 
 * @note 当前只支持中文文本分类
 */
std::string SvmPredictor::predict(const std::string& text, const std::string& model_id) {
    auto tokens = preprocessor_->preprocess(text);
    
    // ========== DEBUG OUTPUT ==========
    std::cerr << "[DEBUG] Input text length: " << text.length() << std::endl << std::flush;
    std::cerr << "[DEBUG] Tokens count: " << tokens.size() << std::endl << std::flush;
    if (tokens.empty()) {
        std::cerr << "[DEBUG] WARNING: No tokens generated!" << std::endl << std::flush;
    } else {
        std::cerr << "[DEBUG] First 10 tokens: ";
        for (size_t i = 0; i < std::min(tokens.size(), size_t(10)); ++i) {
            std::cerr << tokens[i] << " ";
        }
        std::cerr << std::endl << std::flush;
    }
    // ========== END DEBUG ==========
    
    return classifier_.predict(tokens);
}

} // namespace proxy
