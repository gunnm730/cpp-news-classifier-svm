/**
 * @file proxy_manager.hpp
 * @brief 代理管理器 / Proxy manager
 */

#pragma once

#include "i_model_proxy.hpp"
#include <string>
#include <unordered_map>
#include <memory>

namespace proxy {

class ProxyManager : public IModelProxy {
public:
    ProxyManager();
    std::string predict(const std::string& text, const std::string& model_id) override;

private:
    std::unordered_map<std::string, std::shared_ptr<IModelProxy>> proxies_;
};

} // namespace proxy
