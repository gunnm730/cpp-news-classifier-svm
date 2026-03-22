/**
 * @file parser_factory.hpp
 * @brief 解析器工厂模式实现 / Parser Factory Implementation
 * @details 提供统一的解析器创建接口，支持文件类型到解析器的映射
 * 
 * 设计模式：
 * - 工厂模式 (Factory Pattern)
 * - 注册模式 (Registry Pattern)
 * 
 * 使用方式：
 * 1. 使用ParserFactory创建指定类型的解析器
 * 2. 或使用Registry进行解析器注册和获取
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include <memory>
#include <functional>
#include "i_parser.hpp"
#include "common/types.hpp"

namespace parser {

/**
 * @brief 解析器工厂类
 * @details 根据文件类型创建对应的解析器实例
 */
class ParserFactory {
public:
    ParserFactory();
    
    /**
     * @brief 初始化默认解析器
     * @details 注册所有支持的文件类型及其对应的解析器
     */
    void initializeDefaultParsers();
    
    /**
     * @brief 创建解析器实例
     * @param file_type 文件类型
     * @return 对应类型的解析器指针，未注册则返回nullptr
     */
    std::shared_ptr<IParser> createParser(common::FileType file_type);

private:
    std::unordered_map<common::FileType, std::function<std::shared_ptr<IParser>()>> creators_;  ///< 解析器创建函数映射表
};

/**
 * @brief 解析器注册器类型别名
 */
using Creator = std::function<std::shared_ptr<IParser>()>;

/**
 * @brief 解析器注册表（单例模式）
 * @details 提供解析器的全局注册和获取功能
 */
class Registry {
public:
    /**
     * @brief 获取单例实例
     * @return Registry引用
     */
    static Registry& instance();
    
    /**
     * @brief 注册解析器
     * @param type 文件类型
     * @param creator 解析器创建函数
     */
    void registerParser(common::FileType type, Creator creator);
    
    /**
     * @brief 创建指定类型的解析器
     * @param type 文件类型
     * @return 解析器指针
     */
    std::shared_ptr<IParser> create(common::FileType type);

private:
    Registry() = default;
    std::unordered_map<common::FileType, Creator> creators_;  ///< 注册的解析器创建函数
};

} // namespace parser
