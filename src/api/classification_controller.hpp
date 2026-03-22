/**
 * @file classification_controller.hpp
 * @brief 分类控制器 / Classification controller
 */

#pragma once

#include <memory>
#include <vector>
#include "http_request.hpp"
#include "http_response.hpp"
#include "common/types.hpp"
#include "service/task_coordinator.hpp"

namespace api {

class ClassificationController
{
public:
    explicit ClassificationController(std::shared_ptr<service::TaskCoordinator> task_coordinator = nullptr);
    virtual ~ClassificationController() = default;
    virtual HttpResponse onPostClassification(HttpRequest &request);
    virtual bool parseRequestParams(
        HttpRequest &request,
        int &category,
        std::string &model_id,
        int &format,
        std::string &data,
        bool &is_file_mode,
        common::FileType &file_type);

private:
    std::shared_ptr<service::TaskCoordinator> task_coordinator_;
    common::FileType getFileTypeFromName(const std::string &filename) const;
    bool parseJsonBody(
        const std::string &json_body,
        int &category,
        std::string &model_id,
        int &format,
        std::string &data,
        bool &is_file_mode,
        common::FileType &file_type);
    bool parseFormBody(
        HttpRequest &request,
        int &category,
        std::string &model_id,
        int &format,
        bool &is_file_mode,
        common::FileType &file_type);
    HttpResponse buildResponse(const common::Result &result, int format);
    virtual bool validateParams(int category, const std::string &model_id) const;
};

} // namespace api
