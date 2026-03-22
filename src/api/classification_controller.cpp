/**
 * @file classification_controller.cpp
 * @brief 分类控制器实现 / Classification controller implementation
 */

#include "api/classification_controller.hpp"
#include "service/task_coordinator.hpp"
#include "response/formatter.hpp"
#include "response/csv_generator.hpp"
#include "json/single_include/nlohmann/json.hpp"

#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace api
{

    common::FileType parseFileTypeString(const std::string& str) {
        std::string lower;
        for (char c : str) lower += std::tolower(c);
        
        if (lower == "txt" || lower == "0") return common::FileType::TXT;
        if (lower == "doc" || lower == "1") return common::FileType::DOC;
        if (lower == "docx" || lower == "2") return common::FileType::DOCX;
        if (lower == "zip" || lower == "3") return common::FileType::ZIP;
        return common::FileType::UNKNOWN;
    }

    common::FileType inferFileType(const std::string& filename) {
        if (filename.find(".txt") != std::string::npos) return common::FileType::TXT;
        if (filename.find(".doc") != std::string::npos) return common::FileType::DOC;
        if (filename.find(".docx") != std::string::npos) return common::FileType::DOCX;
        if (filename.find(".zip") != std::string::npos) return common::FileType::ZIP;
        return common::FileType::UNKNOWN;
    }

    ClassificationController::ClassificationController(std::shared_ptr<service::TaskCoordinator> task_coordinator)
        : task_coordinator_(task_coordinator)
    {
    }

    HttpResponse ClassificationController::onPostClassification(HttpRequest &request)
    {
        int category = 0;
        std::string model_id;
        int format = 0;
        std::string data;
        bool is_file_mode = false;
        common::FileType file_type = common::FileType::UNKNOWN;

        if (!parseRequestParams(request, category, model_id, format, data, is_file_mode, file_type))
        {
            return HttpResponse::Error(400, "请求参数解析失败");
        }

        if (!validateParams(category, model_id))
        {
            return HttpResponse::Error(400, "参数无效");
        }

        common::ClassificationTask task;
        task.category = static_cast<common::Category>(category);
        task.model_id = model_id;
        task.is_file_mode = is_file_mode;
        task.file_type = file_type;

        if (is_file_mode)
        {
            task.raw_data = request.getFileData();
        }
        else
        {
            task.raw_text = data;
        }

        if (!task_coordinator_)
        {
            return HttpResponse::Error(500, "服务未初始化");
        }

        common::Result result;
        try
        {
            result = task_coordinator_->process(task);
        }
        catch (const std::exception &e)
        {
            return HttpResponse::Error(500, std::string("处理失败: ") + e.what());
        }

        return buildResponse(result, format);
    }

    bool ClassificationController::parseRequestParams(
        HttpRequest &request,
        int &category,
        std::string &model_id,
        int &format,
        std::string &data,
        bool &is_file_mode,
        common::FileType &file_type)
    {

        std::string content_type = request.getHeader("Content-Type");

        if (content_type.find("application/json") != std::string::npos)
        {
            return parseJsonBody(request.body, category, model_id, format, data, is_file_mode, file_type);
        }
        else if (content_type.find("multipart/form-data") != std::string::npos || content_type.find("application/x-www-form-urlencoded") != std::string::npos)
        {
            return parseFormBody(request, category, model_id, format, is_file_mode, file_type);
        }

        return parseJsonBody(request.body, category, model_id, format, data, is_file_mode, file_type);
    }

    bool ClassificationController::parseJsonBody(
        const std::string &json_body,
        int &category,
        std::string &model_id,
        int &format,
        std::string &data,
        bool &is_file_mode,
        common::FileType &file_type)
    {

        try
        {
            auto j = nlohmann::json::parse(json_body);

            if (j.contains("category"))
            {
                category = j["category"].get<int>();
            }

            if (j.contains("model_id"))
            {
                model_id = j["model_id"].get<std::string>();
            }

            if (model_id.empty())
            {
                model_id = "svm_news";
            }

            if (j.contains("format"))
            {
                if (j["format"].is_string())
                {
                    std::string fmt = j["format"].get<std::string>();
                    format = (fmt == "csv" || fmt == "1") ? 1 : 0;
                }
                else
                {
                    format = j["format"].get<int>();
                }
            }

            if (j.contains("data"))
            {
                data = j["data"].get<std::string>();
            }

            if (j.contains("file_type"))
            {
                if (j["file_type"].is_number())
                {
                    file_type = static_cast<common::FileType>(j["file_type"].get<int>());
                }
                else if (j["file_type"].is_string())
                {
                    file_type = parseFileTypeString(j["file_type"].get<std::string>());
                }
            }
            else
            {
                file_type = common::FileType::UNKNOWN;
            }

            is_file_mode = false;
            return true;
        }
        catch (const std::exception &e)
        {
            return false;
        }
    }

    bool ClassificationController::parseFormBody(
        HttpRequest &request,
        int &category,
        std::string &model_id,
        int &format,
        bool &is_file_mode,
        common::FileType &file_type)
    {

        std::string content_type = request.getHeader("Content-Type");

        std::string boundary;
        size_t pos = content_type.find("boundary=");
        if (pos != std::string::npos)
        {
            boundary = content_type.substr(pos + 9);
            while (!boundary.empty() && boundary[0] == ' ')
            {
                boundary = boundary.substr(1);
            }
        }

        if (!boundary.empty())
        {
            request.parseMultipart(boundary);
        }

        bool has_category = false;
        if (request.form_data.find("category") != request.form_data.end())
        {
            category = std::stoi(request.form_data["category"]);
            has_category = true;
        }
        if (request.form_data.find("model_id") != request.form_data.end())
        {
            model_id = request.form_data["model_id"];
        }
        if (request.form_data.find("format") != request.form_data.end())
        {
            std::string fmt = request.form_data["format"];
            format = (fmt == "csv" || fmt == "1") ? 1 : 0;
        }

        is_file_mode = request.isFileUpload();

        if (request.form_data.find("file_type") != request.form_data.end())
        {
            file_type = parseFileTypeString(request.form_data["file_type"]);
        }
        else if (is_file_mode)
        {
            file_type = inferFileType(request.getFileName());
        }
        else
        {
            file_type = common::FileType::UNKNOWN;
        }

        if (!has_category)
        {
            category = 0;
        }
        if (model_id.empty())
        {
            model_id = "svm_news";
        }

        return true;
    }

    HttpResponse ClassificationController::buildResponse(
        const common::Result &result,
        int format)
    {
        HttpResponse response;

        if (format == 1)
        {
            response::CsvGenerator csv_gen;
            std::string csv_content = csv_gen.toCsv(result);
            response.setHeader("Content-Type", "text/csv");
            response.setBody(csv_content);
        }
        else
        {
            response::Formatter formatter;
            std::string json_content = formatter.toJson(result);
            response.setJsonBody(json_content);
        }

        return response;
    }

    bool ClassificationController::validateParams(int category, const std::string &model_id) const
    {
        if (category < 0 || category > 2)
        {
            return false;
        }

        if (model_id.empty())
        {
            return false;
        }

        return true;
    }

} // namespace api
