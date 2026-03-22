/**
 * @file http_server.hpp
 * @brief HTTP服务器头文件 / HTTP Server Header
 * @details 定义基于httplib的HTTP服务器接口
 * 
 * 依赖：
 * - httplib: HTTP库
 * - api::ClassificationController: 分类控制器
 * 
 * @see http_server.cpp - 实现
 */

#pragma once

#include <memory>
#include <httplib.h>
#include "api/classification_controller.hpp"

namespace network {

/**
 * @brief HTTP服务器
 * @details 基于httplib的HTTP服务器封装
 */
class HttpServer {
public:
    /**
     * @brief 构造函数
     * @param port 监听端口，默认8080
     */
    explicit HttpServer(int port = 8080);
    ~HttpServer() = default;

    /**
     * @brief 设置控制器
     * @param controller 分类控制器指针
     */
    void setController(std::shared_ptr<api::ClassificationController> controller);
    
    /**
     * @brief 启动服务器
     */
    void start();
    
    /**
     * @brief 注册GET路由
     */
    template<typename... Args>
    void Get(Args&&... args) { svr_.Get(std::forward<Args>(args)...); }
    
    /**
     * @brief 注册POST路由
     */
    template<typename... Args>
    void Post(Args&&... args) { svr_.Post(std::forward<Args>(args)...); }

private:
    void handleClassify(const httplib::Request& req, httplib::Response& res);
    api::HttpRequest convertRequest(const httplib::Request& req);
    void convertResponse(const api::HttpResponse& apiResp, httplib::Response& res);

private:
    int port_;                                                        ///< 监听端口
    httplib::Server svr_;                                           ///< HTTP服务器
    std::shared_ptr<api::ClassificationController> controller_;       ///< 分类控制器
};

} // namespace network
