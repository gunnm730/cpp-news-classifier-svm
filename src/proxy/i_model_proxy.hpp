/**
 * @file i_model_proxy.hpp
 * @brief 模型预测代理接口 / Model Prediction Proxy Interface
 * @details 定义模型预测的统一接口，支持多种模型类型
 * 
 * 设计模式：策略模式 (Strategy Pattern)
 * - 提供统一的预测接口
 * - 各具体代理实现特定模型的预测逻辑
 * 
 * 使用方式：
 * 1. 根据需求创建对应类型的代理实例
 * 2. 调用predict()方法进行预测
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include <string>

namespace proxy {

/**
 * @brief 模型预测代理抽象接口
 * @details 所有具体模型代理必须实现的基类
 */
class IModelProxy {
public:
    virtual ~IModelProxy() = default;
    
    /**
     * @brief 执行预测
     * @param text 输入文本
     * @param model_id 模型标识符
     * @return 预测的类别标签
     */
    virtual std::string predict(const std::string& text, const std::string& model_id) = 0;
};

} // namespace proxy
