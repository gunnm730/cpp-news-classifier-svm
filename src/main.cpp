/**
 * @file main.cpp
 * @brief 程序入口 / Program entry point
 * @details Web文本分类系统主程序入口 / Web text classification system main program entry point
 */

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>

#define httplib_epastatic
#include "cpp-httplib/httplib.h"

#include "common/types.hpp"
#include "api/http_request.hpp"
#include "api/http_response.hpp"
#include "api/classification_controller.hpp"
#include "service/task_coordinator.hpp"
#include "parser/parser_factory.hpp"
#include "proxy/proxy_manager.hpp"
#include "response/formatter.hpp"
#include "response/csv_generator.hpp"
#include "network/http_server.hpp"

using namespace common;
using namespace api;
using namespace service;
using namespace parser;
using namespace proxy;
using namespace response;
using namespace network;

std::string loadHtmlFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << path << ", using fallback HTML" << std::endl;
        return "<!DOCTYPE html><html><body><h1>Error: Could not load index.html</h1><p>Please ensure src/web/index.html exists</p></body></html>";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void initializeServer(HttpServer& server, std::shared_ptr<ClassificationController> controller) {
    server.setController(controller);
    
    std::string htmlContent = loadHtmlFile("src/web/index.html");
    
    server.Get("/", [htmlContent](const httplib::Request& req, httplib::Response& res) {
        res.set_content(htmlContent, "text/html; charset=utf-8");
    });
}

int main(int argc, char* argv[]) {
    std::cout << "=== Web Text Classification System ===" << std::endl;
    std::cout << ">>> DEBUG MODE ENABLED <<<" << std::endl;
    std::cout << "Initializing..." << std::endl;
    std::cout << std::flush;
    std::cerr << "[MAIN] Debug mode enabled" << std::endl << std::flush;
    
    try {
        auto coordinator = std::make_shared<service::TaskCoordinator>();
        auto proxyManager = std::make_shared<proxy::ProxyManager>();
        coordinator->setModelProxy(proxyManager);
        
        auto controller = std::make_shared<api::ClassificationController>(coordinator);
        
        network::HttpServer server(8080);
        initializeServer(server, controller);
        
        std::cout << "System initialized successfully" << std::endl;
        std::cout << "Server starting on http://localhost:8080" << std::endl;
        std::cout << "API endpoint: POST /api/classify" << std::endl;
        std::cout << "Press Ctrl+C to exit" << std::endl;
        
        server.start();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
