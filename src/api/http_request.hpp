/**
 * @file http_request.hpp
 * @brief HTTP请求封装头文件 / HTTP Request Wrapper Header
 * @details 定义HTTP请求的数据结构和方法
 * 
 * 功能说明：
 * - 存储HTTP请求的所有信息
 * - 提供请求头获取接口
 * - 支持multipart/form-data解析
 * 
 * @see http_request.cpp - 实现
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace api {

/**
 * @brief HTTP请求封装类
 * @details 存储解析后的HTTP请求信息
 */
class HttpRequest {
public:
    std::string method;                                             ///< HTTP方法
    std::string uri;                                                ///< 请求URI
    std::string http_version;                                        ///< HTTP版本
    std::unordered_map<std::string, std::string> headers;           ///< 请求头
    std::string body;                                               ///< 请求体
    std::unordered_map<std::string, std::string> form_data;         ///< 表单数据
    std::string file_name;                                          ///< 上传文件名
    std::string file_content_type;                                  ///< 上传文件类型
    std::string boundary;                                            ///< Multipart边界
    std::vector<uint8_t> file_data;                                ///< 上传文件数据

    HttpRequest();

    /**
     * @brief 获取请求头
     * @param key 请求头名称
     * @return 请求头的值，不存在返回空字符串
     */
    std::string getHeader(const std::string& key) const;
    
    /**
     * @brief 检查是否有请求体
     * @return 有请求体返回true
     */
    bool hasBody() const;
    
    /**
     * @brief 检查是否为文件上传
     * @return 是文件上传返回true
     */
    bool isFileUpload() const;
    
    /**
     * @brief 解析multipart数据
     * @param boundary Multipart边界字符串
     */
    void parseMultipart(const std::string& boundary);
    
    /**
     * @brief 获取上传文件数据
     * @return 文件数据的字节向量
     */
    std::vector<uint8_t> getFileData() const;
    
    /**
     * @brief 获取上传文件名
     * @return 文件名
     */
    std::string getFileName() const;
};

} // namespace api
