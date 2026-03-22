/**
 * @file http_server.cpp
 * @brief HTTP服务器实现 / HTTP Server Implementation
 * @details 基于cpp-httplib实现REST API服务器
 * 
 * 功能说明：
 * - 提供HTTP服务，监听指定端口
 * - 处理POST /api/classify请求
 * - 转换请求/响应格式
 * 
 * 依赖：
 * - cpp-httplib: 轻量级HTTP库
 * 
 * @author [原开发者]
 * @date 2024
 */

#include "http_server.hpp"
#include <iostream>

namespace network {

/**
 * @brief 构造函数
 * @param port 服务器监听端口
 */
HttpServer::HttpServer(int port)
    : port_(port) {
}

/**
 * @brief 设置控制器
 * @param controller 分类控制器实例
 * @details 注册API路由和请求处理函数
 */
void HttpServer::setController(std::shared_ptr<api::ClassificationController> controller) {
    controller_ = controller;
    
    // 注册分类接口路由
    svr_.Post("/api/classify", [this](const httplib::Request& req, httplib::Response& res) {
        // 转换请求格式
        auto apiReq = convertRequest(req);
        // 调用控制器处理
        auto apiResp = controller_->onPostClassification(apiReq);
        // 转换响应格式
        convertResponse(apiResp, res);
    });
}

/**
 * @brief 启动服务器
 * @details 开始监听指定端口，处理HTTP请求
 */
void HttpServer::start() {
    std::cout << "HTTP Server starting on port " << port_ << "..." << std::endl;
    svr_.listen("0.0.0.0", port_);
}

/**
 * @brief 转换httplib请求为内部请求格式
 * @param req httplib请求对象
 * @return 内部HttpRequest对象
 */
api::HttpRequest HttpServer::convertRequest(const httplib::Request& req) {
    api::HttpRequest result;
    result.method = req.method;
    result.uri = req.path;
    result.http_version = req.version;
    result.body = req.body;
    
    // 复制HTTP头部
    for (const auto& header : req.headers) {
        result.headers[header.first] = header.second;
    }
    
    return result;
}

/**
 * @brief 转换内部响应为httplib响应
 * @param apiResp 内部响应对象
 * @param res httplib响应对象
 */
void HttpServer::convertResponse(const api::HttpResponse& apiResp, httplib::Response& res) {
    res.status = apiResp.status_code;
    res.set_content(apiResp.body, "application/json");
    
    // 设置响应头部
    for (const auto& header : apiResp.headers) {
        res.set_header(header.first, header.second);
    }
}

} // namespace network
