/**
 * @file Preprocessor.cpp
 * @brief 训练文本预处理器实现 / Training Text Preprocessor Implementation
 * @details 实现训练模块使用的文本预处理功能
 */

#include "Preprocessor.hpp"

namespace ml {

    /**
     * @brief 构造函数实现
     * @details 初始化jieba分词器并加载停用词表
     */
	Preprocessor::Preprocessor(const std::string JIEBA_DICT_PATH, const std::string HMM_MODEL_PATH, const std::string USER_DICT_PATH, const std::string IDF_PATH, const std::string STOP_WORD_PATH)
		: jieba_(JIEBA_DICT_PATH, HMM_MODEL_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)
	{
        // 加载停用词表
		load_stopwords(STOP_WORD_PATH);
	}

    /**
     * @brief 分词处理实现
     * @details 对文本进行预处理：
     * 1. 移除UTF-8 BOM和ASCII字符
     * 2. 使用jieba进行分词
     * 3. 过滤停用词
     * 4. 仅保留长度>=6的词语
     * 
     * @param text 待处理的文本
     * @return 过滤后的词语向量
     */
	std::vector<std::string> Preprocessor::tokenize(const std::string& text) const
	{
        // Step 1: 移除非中文字符
        // 仅保留UTF-8编码的中文字符（3字节）和扩展汉字（4字节）
		std::string cleaned_text;
		for (size_t i = 0; i < text.length();) {
			int len = 1;
			if ((text[i] & 0xF0) == 0xE0) len = 3;      // 3字节UTF-8字符
			else if ((text[i] & 0xF8) == 0xF0) len = 4; // 4字节UTF-8字符
			if (len == 3 || len == 4) { 
                cleaned_text += text.substr(i, len); 
            }
			i += len;
		}
        
        // Step 2: 使用jieba分词
		std::vector<std::string> words;
		jieba_.Cut(cleaned_text, words, true);
        
        // Step 3: 过滤停用词和短词语
		std::vector<std::string> filtered_words;
		for (const auto& word : words) {
            // 仅保留非停用词且长度>=6的词语
			if (stopwords_.find(word) == stopwords_.end() && word.length() >= 6) {
				filtered_words.push_back(word);
			}
		}
		return filtered_words;
	}

    /**
     * @brief 加载停用词表
     * @param STOP_WORD_PATH 停用词表文件路径
     */
	void Preprocessor::load_stopwords(const std::string& STOP_WORD_PATH)
	{
		std::ifstream stopword_file(STOP_WORD_PATH);
		std::string line;
		while (std::getline(stopword_file, line)) {
			stopwords_.insert(line);
		}
	}


}