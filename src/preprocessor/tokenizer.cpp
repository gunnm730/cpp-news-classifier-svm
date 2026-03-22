#include "tokenizer.hpp"

namespace preprocessor {

namespace tokenizer {

Tokenizer::Tokenizer(cppjieba::Jieba& jieba)
    : jieba_(jieba) {
}

std::vector<std::string> Tokenizer::tokenize(const std::string& text) {
    return tokenize_with_hmm(text, true);
}

std::vector<std::string> Tokenizer::tokenize_with_hmm(const std::string& text, bool use_hmm) {
    std::vector<std::string> words;
    jieba_.Cut(text, words, use_hmm);
    return words;
}

}

}
