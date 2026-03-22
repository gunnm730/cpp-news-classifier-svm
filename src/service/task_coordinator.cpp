/**
 * @file task_coordinator.cpp
 * @brief 任务协调器实现 / Task coordinator implementation
 */

#include "task_coordinator.hpp"
#include <stdexcept>
#include <iostream>

namespace service
{

    TaskCoordinator::TaskCoordinator()
    {
        initialize();
    }

    void TaskCoordinator::initialize()
    {
        parser_factory_.initializeDefaultParsers();
    }

    common::Result TaskCoordinator::process(const common::ClassificationTask &task)
    {
        common::Document document;
        if (task.is_file_mode)
        {
            auto parser = parser_factory_.createParser(task.file_type);
            if (!parser)
            {
                throw std::runtime_error("无法创建解析器");
            }
            document = parser->parse(task.raw_data);
        }
        else
        {
            document.text = task.raw_text;
        }

        common::Result result;
        if (model_proxy_)
        {
            result.label = model_proxy_->predict(document.text, task.model_id);
        }

        return result;
    }

    void TaskCoordinator::setModelProxy(std::shared_ptr<proxy::IModelProxy> proxy)
    {
        model_proxy_ = proxy;
    }

} // namespace service
