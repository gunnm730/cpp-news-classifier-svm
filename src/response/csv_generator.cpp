/**
 * @file csv_generator.cpp
 * @brief CSV生成器实现 / CSV Generator Implementation
 * @details 将分类结果转换为CSV格式
 * 
 * @see csv_generator.hpp - 头文件
 */

#include "csv_generator.hpp"
#include <sstream>

namespace response {

CsvGenerator::CsvGenerator() {
    columns_ = {"label"};
}

/**
 * @brief 转换为CSV格式
 * @param result 分类结果
 * @return CSV字符串
 */
std::string CsvGenerator::toCsv(const common::Result& result) {
    std::ostringstream oss;
    for (size_t i = 0; i < columns_.size(); ++i) {
        if (i > 0) oss << ",";
        oss << escapeField(columns_[i]);
    }
    oss << "\n";
    oss << escapeField(result.label);
    return oss.str();
}

/**
 * @brief 转义CSV字段
 * @param field 原始字段
 * @return 转义后的字段
 */
std::string CsvGenerator::escapeField(const std::string& field) const {
    if (field.find(',') != std::string::npos ||
        field.find('"') != std::string::npos ||
        field.find('\n') != std::string::npos) {
        std::string escaped = field;
        size_t pos = 0;
        while ((pos = escaped.find('"', pos)) != std::string::npos) {
            escaped.insert(pos, "\"");
            pos += 2;
        }
        return "\"" + escaped + "\"";
    }
    return field;
}

} // namespace response
