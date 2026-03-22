/**
 * @file formatter.hpp
 * @brief JSON格式化器 / JSON formatter
 */

#pragma once

#include <string>
#include "common/types.hpp"

namespace response {

class Formatter {
public:
    Formatter();
    std::string toJson(const common::Result& result);
};

} // namespace response
