/**
 * @file Preprocessor.hpp
 * @brief 训练文本预处理器头文件 / Training Text Preprocessor Header
 * @details 定义训练模块使用的文本预处理功能
 * 
 * 功能说明：
 * - 使用jieba分词器对中文文本进行分词
 * - 过滤停用词
 * - 仅保留长度>=6的词语（训练时）
 * 
 * @author [原开发者]
 * @date 2024
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include "cppjieba/Jieba.hpp"
#include "types.hpp"


namespace ml {

    /**
     * @brief 训练文本预处理器
     * @details 对中文文本进行分词和过滤操作
     * 
     * 处理流程：
     * 1. 使用jieba分词器进行中文分词
     * 2. 移除BOM和无效字符
     * 3. 过滤停用词
     * 4. 仅保留长度>=6的词语
     */
	class Preprocessor {
	public:
        /**
         * @brief 构造函数
         * @param JIEBA_DICT_PATH jieba词典路径
         * @param HMM_MODEL_PATH HMM模型路径
         * @param USER_DICT_PATH 用户词典路径
         * @param IDF_PATH IDF文件路径
         * @param STOP_WORD_PATH 停用词表路径
         */
		Preprocessor(const std::string JIEBA_DICT_PATH, const std::string HMM_MODEL_PATH, const std::string USER_DICT_PATH, const std::string IDF_PATH, const std::string STOP_WORD_PATH);

        /**
         * @brief 分词处理
         * @details 对输入文本进行分词、清洗和过滤
         * @param text 待处理的文本
         * @return 词语向量
         */
		std::vector<std::string> tokenize(const std::string& text) const;
	private:
		cppjieba::Jieba jieba_;                      ///< jieba分词器实例
		std::unordered_set<std::string> stopwords_; ///< 停用词集合

        /**
         * @brief 加载停用词表
         * @param STOP_WORD_PATH 停用词表文件路径
         */
		void load_stopwords(const std::string& STOP_WORD_PATH);

	};
}