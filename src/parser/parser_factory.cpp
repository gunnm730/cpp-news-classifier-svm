/**
 * @file parser_factory.cpp
 * @brief 解析器工厂实现 / Parser factory implementation
 */

#include "parser_factory.hpp"
#include "txt_parser.hpp"
#include "docx_parser.hpp"
#include "zip_parser.hpp"

namespace parser {

ParserFactory::ParserFactory() {
    initializeDefaultParsers();
}

void ParserFactory::initializeDefaultParsers() {
    creators_[common::FileType::TXT] = []() { return std::make_shared<TxtParser>(); };
    creators_[common::FileType::DOC] = []() { return std::make_shared<DocxParser>(); };
    creators_[common::FileType::DOCX] = []() { return std::make_shared<DocxParser>(); };
    creators_[common::FileType::ZIP] = []() { return std::make_shared<ZipParser>(); };
}

std::shared_ptr<IParser> ParserFactory::createParser(common::FileType file_type) {
    auto it = creators_.find(file_type);
    if (it != creators_.end()) {
        return it->second();
    }
    return nullptr;
}

Registry& Registry::instance() {
    static Registry instance;
    return instance;
}

void Registry::registerParser(common::FileType type, Creator creator) {
    creators_[type] = creator;
}

std::shared_ptr<IParser> Registry::create(common::FileType type) {
    auto it = creators_.find(type);
    if (it != creators_.end()) {
        return it->second();
    }
    return nullptr;
}

} // namespace parser
