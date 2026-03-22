/**
 * @file status_code.hpp
 * @brief HTTP状态码定义 / HTTP status code definitions
 */

#pragma once

namespace common {

enum class StatusCode {
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    INTERNAL_ERROR = 500
};

} // namespace common
