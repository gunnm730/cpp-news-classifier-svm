/**
 * @file i_parser.hpp
 * @brief 解析器接口定义 / Parser Interface Definition
 * @details 定义文件解析的统一接口，支持多种文件格式的文本提取
 * 
 * 设计模式：策略模式 (Strategy Pattern)
 * - 提供统一的解析接口
 * - 各具体解析器实现特定文件格式的解析逻辑
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include <memory>
#include <vector>
#include "common/types.hpp"

namespace parser {

/**
 * @brief 解析器抽象接口
 * @details 所有具体解析器必须实现的基类
 * 
 * 使用方式：
 * 1. 根据文件类型创建对应解析器
 * 2. 调用parse()方法提取文本内容
 */
class IParser {
public:
    virtual ~IParser() = default;
    
    /**
     * @brief 解析文件数据
     * @param data 文件的二进制数据
     * @return 解析后的文档对象，包含提取的文本内容
     */
    virtual common::Document parse(const std::vector<uint8_t>& data) = 0;
};

} // namespace parser
