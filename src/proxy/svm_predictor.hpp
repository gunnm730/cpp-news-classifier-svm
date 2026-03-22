/**
 * @file svm_predictor.hpp
 * @brief SVM预测器 / SVM predictor
 */

#pragma once

#include "i_model_proxy.hpp"
#include <string>
#include <memory>
#include "../my_svm/include/classifier.hpp"
#include "../preprocessor/text_preprocessor.hpp"

namespace proxy {

class SvmPredictor : public IModelProxy {
public:
    SvmPredictor(const std::string& model_path);
    std::string predict(const std::string& text, const std::string& model_id) override;

private:
    void loadConfig();
    std::string model_path_;
    my_svm::SvmClassifier classifier_;
    std::unique_ptr<preprocessor::TextPreprocessor> preprocessor_;
};

} // namespace proxy
