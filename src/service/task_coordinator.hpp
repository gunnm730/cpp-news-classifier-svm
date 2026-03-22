/**
 * @file task_coordinator.hpp
 * @brief 任务协调器头文件 / Task Coordinator Header
 * @details 定义任务协调的核心接口
 * 
 * 功能说明：
 * - 协调文本解析和模型预测
 * - 管理解析器工厂
 * - 处理不同类型的输入（文本、文件）
 * 
 * @see task_coordinator.cpp - 实现
 */

#pragma once

#include <memory>
#include "common/types.hpp"
#include "parser/parser_factory.hpp"
#include "proxy/i_model_proxy.hpp"

namespace service {

/**
 * @brief 任务协调器
 * @details 协调文本解析、模型预测的流程
 */
class TaskCoordinator {
public:
    TaskCoordinator();
    
    /**
     * @brief 处理分类任务
     * @param task 分类任务
     * @return 分类结果
     */
    common::Result process(const common::ClassificationTask& task);
    
    /**
     * @brief 设置模型代理
     * @param proxy 模型代理指针
     */
    void setModelProxy(std::shared_ptr<proxy::IModelProxy> proxy);

private:
    void initialize();
    parser::ParserFactory parser_factory_;                     ///< 解析器工厂
    std::shared_ptr<proxy::IModelProxy> model_proxy_;          ///< 模型代理
};

} // namespace service
