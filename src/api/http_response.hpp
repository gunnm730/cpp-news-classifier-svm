/**
 * @file http_response.hpp
 * @brief HTTP响应封装头文件 / HTTP Response Wrapper Header
 * @details 定义HTTP响应的数据结构和方法
 * 
 * 功能说明：
 * - 构建HTTP响应
 * - 设置状态码和消息
 * - 设置响应头和响应体
 * 
 * @see http_response.cpp - 实现
 */

#pragma once

#include <string>
#include <unordered_map>

namespace api {

/**
 * @brief HTTP响应封装类
 * @details 用于构建HTTP响应
 */
class HttpResponse {
public:
    std::string http_version;                            ///< HTTP版本
    int status_code;                                     ///< 状态码
    std::string status_message;                         ///< 状态消息
    std::unordered_map<std::string, std::string> headers;///< 响应头
    std::string body;                                   ///< 响应体

    HttpResponse();

    /**
     * @brief 创建错误响应
     * @param code HTTP状态码
     * @param message 错误消息
     * @return 错误响应对象
     */
    static HttpResponse Error(int code, const std::string& message);
    
    /**
     * @brief 设置响应头
     * @param key 头名称
     * @param value 头值
     */
    void setHeader(const std::string& key, const std::string& value);
    
    /**
     * @brief 设置响应体
     * @param content 响应内容
     */
    void setBody(const std::string& content);
    
    /**
     * @brief 设置JSON响应体
     * @param json_content JSON字符串
     */
    void setJsonBody(const std::string& json_content);
};

} // namespace api
