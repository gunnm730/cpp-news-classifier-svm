/**
 * @file csv_generator.hpp
 * @brief CSV生成器 / CSV generator
 */

#pragma once

#include <string>
#include <vector>
#include "common/types.hpp"

namespace response {

class CsvGenerator {
public:
    CsvGenerator();
    std::string toCsv(const common::Result& result);

private:
    std::string escapeField(const std::string& field) const;
    std::vector<std::string> columns_;
};

} // namespace response
