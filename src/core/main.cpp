/**
 * @file main.cpp (训练模块)
 * @brief 模型训练主程序 / Model Training Main Program
 * @details 使用训练数据集训练SVM分类模型并保存
 * 
 * 训练流程：
 * 1. 加载并预处理训练数据
 * 2. 使用TF-IDF向量化文本
 * 3. 训练SVM分类器
 * 4. 在测试集上评估模型
 * 5. 保存训练好的模型
 * 
 * 所有路径配置通过 config/paths.json 管理
 * 
 * @author [原开发者]
 * @date 2024
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <chrono>
#include <iomanip>
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>

#define NOMINMAX
#include <Windows.h>

#include "types.hpp"
#include "Preprocessor.hpp"
#include "Tfidfvectorizer.hpp"
#include "onetoall.hpp"

#include "json/single_include/nlohmann/json.hpp"

using nlohmann::json;

/**
 * @brief 训练配置结构
 */
struct TrainConfig {
    std::string train_data_path;
    std::string test_data_path;
    std::string stopwords_path;
    std::string jieba_dict_path;
    std::string hmm_model_path;
    std::string user_dict_path;
    std::string idf_path;
    std::string model_save_path;
};

/**
 * @brief 加载配置文件
 * @param config_path 配置文件路径
 * @return 训练配置结构
 */
TrainConfig loadConfig(const std::string& config_path = "config/paths.json") {
    std::ifstream config_file(config_path);
    if (!config_file.is_open()) {
        throw std::runtime_error("无法打开配置文件: " + config_path);
    }
    
    json config;
    config_file >> config;
    config_file.close();
    
    TrainConfig cfg;
    cfg.train_data_path = config["train_data_path"].get<std::string>();
    cfg.test_data_path = config["test_data_path"].get<std::string>();
    cfg.stopwords_path = config["stopwords_path"].get<std::string>();
    cfg.jieba_dict_path = config["jieba_dict_path"].get<std::string>();
    cfg.hmm_model_path = config["hmm_model_path"].get<std::string>();
    cfg.user_dict_path = config["user_dict_path"].get<std::string>();
    cfg.idf_path = config["idf_path"].get<std::string>();
    cfg.model_save_path = config["svm_model_path"].get<std::string>();
    
    std::cout << "已加载配置文件: " << config_path << std::endl;
    std::cout << "训练数据: " << cfg.train_data_path << std::endl;
    std::cout << "测试数据: " << cfg.test_data_path << std::endl;
    std::cout << "模型保存: " << cfg.model_save_path << std::endl;
    
    return cfg;
}

/**
 * @brief 分类报告结构
 * @details 用于存储每个类别的分类统计信息
 */
struct ClassificationReport {
    std::string label_name;
    int correct_predictions = 0;
    int total_samples = 0;

    double get_accuracy() const {
        if (total_samples == 0) {
            return 0.0;
        }
        return (static_cast<double>(correct_predictions) / total_samples) * 100.0;
    }
};

/**
 * @brief 加载数据集并进行标签映射
 */
std::pair<ml::Corpus, std::vector<double>> load_and_map_data(
    const std::string& file_path,
    const ml::Preprocessor& preprocessor,
    std::unordered_map<std::string, double>& label_to_id_map,
    bool is_training = true)
{
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开数据文件: " + file_path);
    }

    char bom[3] = { 0 };
    file.read(bom, 3);
    if (static_cast<unsigned char>(bom[0]) != 0xEF || 
        static_cast<unsigned char>(bom[1]) != 0xBB || 
        static_cast<unsigned char>(bom[2]) != 0xBF) {
        file.seekg(0);
    }

    ml::Corpus corpus;
    std::vector<double> labels;
    std::string line;
    double next_label_id = static_cast<double>(label_to_id_map.size());

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        size_t tab_pos = line.find('\t');
        if (tab_pos == std::string::npos) continue;

        std::string label_str = line.substr(0, tab_pos);
        std::string text = line.substr(tab_pos + 1);
        std::vector<std::string> tokenized_text = preprocessor.tokenize(text);

        if (!tokenized_text.empty()) {
            double current_id;
            auto it = label_to_id_map.find(label_str);

            if (it == label_to_id_map.end()) {
                if (is_training) {
                    current_id = next_label_id;
                    label_to_id_map[label_str] = current_id;
                    std::cout << "  新标签: '" << label_str << "' -> ID " << current_id << std::endl;
                    next_label_id += 1.0;
                } else {
                    continue;
                }
            } else {
                current_id = it->second;
            }

            corpus.push_back(tokenized_text);
            labels.push_back(current_id);
        }
    }

    file.close();
    return { corpus, labels };
}

/**
 * @brief 保存训练好的模型
 */
bool save_model(
    const ml::TfidfVectorizer& tfidf_vectorizer,
    const ml::my_cnews_trainer& classifier,
    const std::unordered_map<double, std::string>& id_to_label_map,
    const std::string& base_path)
{
    bool all_success = true;
    
    size_t last_dot = base_path.find_last_of('.');
    std::string prefix = base_path.substr(0, last_dot);
    
    std::string tfidf_path = prefix + "_tfidf.bin";
    std::ofstream tfidf_file(tfidf_path, std::ios::binary);
    if (tfidf_file.is_open()) {
        if (!tfidf_vectorizer.save(tfidf_file)) {
            std::cerr << "TF-IDF向量化器保存失败" << std::endl;
            all_success = false;
        }
        tfidf_file.close();
    } else {
        std::cerr << "无法创建TF-IDF文件: " << tfidf_path << std::endl;
        all_success = false;
    }

    std::string svm_path = base_path;
    std::ofstream svm_file(svm_path, std::ios::binary);
    if (svm_file.is_open()) {
        if (!classifier.save(svm_file)) {
            std::cerr << "SVM分类器保存失败" << std::endl;
            all_success = false;
        }
        svm_file.close();
    } else {
        std::cerr << "无法创建SVM文件: " << svm_path << std::endl;
        all_success = false;
    }

    std::string label_path = prefix + "_labels.bin";
    std::ofstream label_file(label_path, std::ios::binary);
    if (label_file.is_open()) {
        const char magic[] = "LBMP";
        label_file.write(magic, 4);
        
        uint32_t version = 1;
        label_file.write(reinterpret_cast<const char*>(&version), sizeof(version));
        
        uint64_t size = id_to_label_map.size();
        label_file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        
        for (const auto& pair : id_to_label_map) {
            double id = pair.first;
            label_file.write(reinterpret_cast<const char*>(&id), sizeof(id));
            
            uint64_t label_len = pair.second.size();
            label_file.write(reinterpret_cast<const char*>(&label_len), sizeof(label_len));
            label_file.write(pair.second.c_str(), static_cast<std::streamsize>(label_len));
        }
        
        label_file.close();
    } else {
        std::cerr << "无法创建标签文件: " << label_path << std::endl;
        all_success = false;
    }

    if (all_success) {
        std::cout << "\n模型已保存:" << std::endl;
        std::cout << "  - " << svm_path << std::endl;
        std::cout << "  - " << tfidf_path << std::endl;
        std::cout << "  - " << label_path << std::endl;
    }

    return all_success;
}

/**
 * @brief 运行完整的训练和评估流程
 */
void run_pipeline(const TrainConfig& config) {
    std::cout << "========== SVM 文本分类训练流程启动 ==========" << std::endl;
    std::cout << std::endl;

    ml::Preprocessor preprocessor(
        config.jieba_dict_path,
        config.hmm_model_path,
        config.user_dict_path,
        config.idf_path,
        config.stopwords_path
    );
    
    ml::TfidfVectorizer tfidf_vectorizer(50000);
    std::unordered_map<std::string, double> label_to_id_map;

    auto training_start_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "[阶段 1] 加载并预处理训练数据..." << std::endl;
    auto [train_corpus, train_labels] = load_and_map_data(
        config.train_data_path, preprocessor, label_to_id_map, true);

    if (train_corpus.empty()) {
        throw std::runtime_error("错误: 未能从训练数据文件加载任何文档。");
    }
    std::cout << "训练数据已加载: " << train_corpus.size() << " 篇文档" << std::endl;
    std::cout << "发现 " << label_to_id_map.size() << " 个类别" << std::endl;

    std::cout << "\n[阶段 2] 训练TF-IDF向量化器..." << std::endl;
    std::vector<ml::sample_type> train_vectors = tfidf_vectorizer.fit_transform(train_corpus);
    std::cout << "TF-IDF向量化器训练完成" << std::endl;

    std::cout << "\n[阶段 3] 训练SVM模型..." << std::endl;
    std::unordered_set<double> unique_labels(train_labels.begin(), train_labels.end());
    ml::my_cnews_trainer trainer(unique_labels, 50000, 0.01, 0.005);
    trainer.train(train_vectors, train_labels);

    auto training_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> training_duration = training_end_time - training_start_time;
    std::cout << "模型训练完成，耗时 " << std::fixed << std::setprecision(2) 
              << training_duration.count() << " 秒" << std::endl;

    std::cout << "\n[阶段 4] 保存模型..." << std::endl;
    std::unordered_map<double, std::string> id_to_label_map;
    for (const auto& pair : label_to_id_map) {
        id_to_label_map[pair.second] = pair.first;
    }
    save_model(tfidf_vectorizer, trainer, id_to_label_map, config.model_save_path);

    auto eval_start_time = std::chrono::high_resolution_clock::now();
    std::cout << "\n[阶段 5] 在测试集上评估..." << std::endl;
    
    auto [test_corpus, test_labels] = load_and_map_data(
        config.test_data_path, preprocessor, label_to_id_map, false);
    if (test_corpus.empty()) {
        std::cerr << "警告: 未能从测试数据文件加载任何文档进行评估" << std::endl;
        return;
    }
    std::cout << "测试数据已加载: " << test_corpus.size() << " 篇文档" << std::endl;

    std::vector<ml::sample_type> test_vectors = tfidf_vectorizer.transform(test_corpus);

    std::unordered_map<double, ClassificationReport> report_map;
    for (const auto& pair : id_to_label_map) {
        report_map[pair.first].label_name = pair.second;
    }

    int total_correct_predictions = 0;
    for (size_t i = 0; i < test_vectors.size(); ++i) {
        double true_label_id = test_labels[i];
        report_map[true_label_id].total_samples++;

        if (!test_vectors[i].empty()) {
            double predicted_label_id = trainer.predict(test_vectors[i]);
            if (predicted_label_id == true_label_id) {
                report_map[true_label_id].correct_predictions++;
                total_correct_predictions++;
            }
        }
    }

    double overall_accuracy = 0.0;
    if (!test_labels.empty()) {
        overall_accuracy = (static_cast<double>(total_correct_predictions) / test_labels.size()) * 100.0;
    }
    auto eval_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> eval_duration = eval_end_time - eval_start_time;

    std::cout << "\n========== 评估完成 ==========" << std::endl;
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\n【总体性能】" << std::endl;
    std::cout << "测试样本总数: " << test_labels.size() << std::endl;
    std::cout << "正确预测数: " << total_correct_predictions << std::endl;
    std::cout << "总体准确率: " << overall_accuracy << "%" << std::endl;
    std::cout << "评估耗时: " << eval_duration.count() << " 秒" << std::endl;

    std::cout << "\n【各类别准确率】" << std::endl;
    std::cout << std::left << std::setw(15) << "类别"
              << std::setw(15) << "准确率"
              << std::setw(20) << "正确/总数" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    std::vector<std::pair<double, ClassificationReport>> sorted_report(report_map.begin(), report_map.end());
    std::sort(sorted_report.begin(), sorted_report.end(),
        [](const auto& a, const auto& b) {
            return a.first < b.first;
        }
    );

    for (const auto& pair : sorted_report) {
        const auto& report = pair.second;
        std::string stats = std::to_string(report.correct_predictions) + "/" + std::to_string(report.total_samples);
        std::cout << std::left << std::setw(15) << report.label_name
                  << std::setw(15) << report.get_accuracy() << "%"
                  << std::setw(20) << stats << std::endl;
    }
}

/**
 * @brief 主函数
 */
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    try {
        TrainConfig config = loadConfig();
        run_pipeline(config);
    }
    catch (const std::exception& e) {
        std::cerr << "\n发生未处理的异常: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
