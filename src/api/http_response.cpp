/**
 * @file http_response.cpp
 * @brief HTTP响应实现 / HTTP response implementation
 */

#include "http_response.hpp"

namespace api {

namespace {
std::string getMessageForCode(int code) {
    switch (code) {
        case 200: return "OK";
        case 400: return "Bad Request";
        case 500: return "Internal Server Error";
        default:  return "Unknown";
    }
}
}

HttpResponse::HttpResponse() 
    : http_version("HTTP/1.1"), status_code(200), status_message("OK") {
}

HttpResponse HttpResponse::Error(int code, const std::string& message) {
    HttpResponse resp;
    resp.status_code = code;
    resp.status_message = getMessageForCode(code);
    resp.body = message;
    resp.setHeader("Content-Type", "text/plain; charset=utf-8");
    return resp;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void HttpResponse::setBody(const std::string& content) {
    body = content;
}

void HttpResponse::setJsonBody(const std::string& json_content) {
    status_code = 200;
    status_message = "OK";
    body = json_content;
    setHeader("Content-Type", "application/json; charset=utf-8");
}

} // namespace api
