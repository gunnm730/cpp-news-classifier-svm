/**
 * @file proxy_manager.cpp
 * @brief 代理管理器实现 / Proxy Manager Implementation
 * @details 管理系统中的多个预测器，根据model_id路由到对应的预测器
 * 
 * 功能说明：
 * - 管理多个IModelProxy实现（SvmPredictor等）
 * - 根据model_id路由预测请求
 * - 提供默认模型fallback机制
 * 
 * 架构说明：
 * ProxyManager是请求路由层，它将API层的请求分发到具体的预测器。
 * 这种设计允许系统轻松扩展新的模型类型。
 * 
 * 当前支持的模型：
 * - svm_news: 基于SVM的新闻分类模型
 * 
 * @see svm_predictor.cpp - SVM预测器实现
 * @see i_proxy.hpp - 预测器接口定义
 */

#include "proxy_manager.hpp"
#include "svm_predictor.hpp"
#include <iostream>

namespace proxy {

/**
 * @brief 构造函数
 * @details 初始化所有可用的预测器
 * 
 * 当前初始化的预测器：
 * - "svm_news": SvmPredictor，使用model/svm_model.bin
 */
ProxyManager::ProxyManager() {
    proxies_["svm_news"] = std::make_shared<SvmPredictor>("model/svm_model.bin");
}

/**
 * @brief 预测文本类别
 * @param text 待分类的文本
 * @param model_id 模型标识符
 * @return 预测的类别标签名称
 * 
 * 路由逻辑：
 * 1. 首先查找是否存在指定model_id的预测器
 * 2. 如果找到，使用该预测器进行预测
 * 3. 如果未找到，使用默认预测器"svm_news"
 * 4. 如果默认预测器也不存在，返回"unknown"
 */
std::string ProxyManager::predict(const std::string& text, const std::string& model_id) {
    // ========== DEBUG OUTPUT ==========
    std::cerr << "[DEBUG] ProxyManager::predict called with model_id: " << model_id << std::endl << std::flush;
    std::cerr << "[DEBUG] Text length: " << text.length() << std::endl << std::flush;
    std::cerr << "[DEBUG] Available proxies count: " << proxies_.size() << std::endl << std::flush;
    // ========== END DEBUG ==========
    
    auto it = proxies_.find(model_id);
    if (it != proxies_.end()) {
        // ========== DEBUG OUTPUT ==========
        std::cerr << "[DEBUG] Found proxy for model_id: " << model_id << std::endl << std::flush;
        // ========== END DEBUG ==========
        return it->second->predict(text, model_id);
    }
    
    // ========== DEBUG OUTPUT ==========
    std::cerr << "[DEBUG] Model not found: " << model_id << ", trying default svm_news" << std::endl << std::flush;
    // ========== END DEBUG ==========
    
    auto default_it = proxies_.find("svm_news");
    if (default_it != proxies_.end()) {
        // ========== DEBUG OUTPUT ==========
        std::cerr << "[DEBUG] Using default svm_news proxy" << std::endl << std::flush;
        // ========== END DEBUG ==========
        return default_it->second->predict(text, model_id);
    }
    
    // ========== DEBUG OUTPUT ==========
    std::cerr << "[DEBUG] No proxy found, returning 'unknown'" << std::endl << std::flush;
    // ========== END DEBUG ==========
    return "unknown";
}

} // namespace proxy
