/**
 * @file http_request.cpp
 * @brief HTTP请求实现 / HTTP request implementation
 */

#include "http_request.hpp"
#include <algorithm>

namespace api {

HttpRequest::HttpRequest()
    : method("GET"), uri("/"), http_version("HTTP/1.1") {
}

std::string HttpRequest::getHeader(const std::string& key) const {
    auto it = headers.find(key);
    if (it != headers.end()) {
        return it->second;
    }
    auto it_lower = headers.find(key);
    for (const auto& pair : headers) {
        std::string lower_key = pair.first;
        std::transform(lower_key.begin(), lower_key.end(), lower_key.begin(), ::tolower);
        std::string search_key = key;
        std::transform(search_key.begin(), search_key.end(), search_key.begin(), ::tolower);
        if (lower_key == search_key) {
            return pair.second;
        }
    }
    return "";
}

bool HttpRequest::hasBody() const {
    return !body.empty();
}

bool HttpRequest::isFileUpload() const {
    return !file_data.empty();
}

void HttpRequest::parseMultipart(const std::string& boundary_param) {
    if (body.empty() || boundary_param.empty()) {
        return;
    }
    
    boundary = boundary_param;
    std::string sep = "--" + boundary;
    std::string end_sep = sep + "--";
    
    size_t pos = 0;
    while (pos < body.size()) {
        size_t sep_pos = body.find(sep, pos);
        if (sep_pos == std::string::npos) break;
        
        size_t next_sep_pos = body.find(sep, sep_pos + sep.length());
        if (next_sep_pos == std::string::npos) break;
        
        size_t content_start = body.find("\r\n\r\n", sep_pos);
        if (content_start == std::string::npos || content_start > next_sep_pos) {
            pos = next_sep_pos;
            continue;
        }
        content_start += 4;
        
        std::string part = body.substr(content_start, next_sep_pos - content_start);
        if (!part.empty() && part.back() == '\n') {
            part.pop_back();
        }
        if (!part.empty() && part.back() == '\r') {
            part.pop_back();
        }
        
        if (part.find("Content-Disposition: form-data") != std::string::npos) {
            size_t name_pos = part.find("name=\"");
            size_t filename_pos = part.find("filename=\"");
            
            if (filename_pos != std::string::npos) {
                size_t fname_start = filename_pos + 10;
                size_t fname_end = part.find("\"", fname_start);
                file_name = part.substr(fname_start, fname_end - fname_start);
                size_t data_start = part.find("\r\n", fname_end);
                if (data_start != std::string::npos && data_start + 2 < part.size()) {
                    file_data = std::vector<uint8_t>(part.begin() + data_start + 2, part.end());
                }
            } else if (name_pos != std::string::npos) {
                size_t name_start = name_pos + 6;
                size_t name_end = part.find("\"", name_start);
                std::string field_name = part.substr(name_start, name_end - name_start);
                
                size_t data_start = part.find("\r\n\r\n");
                if (data_start != std::string::npos && data_start + 4 < part.size()) {
                    std::string field_value = part.substr(data_start + 4);
                    form_data[field_name] = field_value;
                }
            }
        }
        
        pos = next_sep_pos;
    }
}

std::vector<uint8_t> HttpRequest::getFileData() const {
    return file_data;
}

std::string HttpRequest::getFileName() const {
    return file_name;
}

} // namespace api
