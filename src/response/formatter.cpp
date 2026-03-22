/**
 * @file formatter.cpp
 * @brief JSON格式化器实现 / JSON Formatter Implementation
 * @details 将分类结果转换为JSON格式
 * 
 * @see formatter.hpp - 头文件
 */

#include "formatter.hpp"
#include "json/single_include/nlohmann/json.hpp"

namespace response {

Formatter::Formatter() {
}

/**
 * @brief 转换为JSON格式
 * @param result 分类结果
 * @return JSON字符串
 */
std::string Formatter::toJson(const common::Result& result) {
    nlohmann::json j;
    j["label"] = result.label;
    return j.dump();
}

} // namespace response
