#pragma once
#include <boost/json.hpp>
#include "../components/ProviderComponents.h"
#include "../components/StreamComponents.h"
#include "../components/Chat.h"


// 提供商基类
class Provider {
public:
    virtual ~Provider() = default;

    // 提供商可用能力
    [[nodiscard("禁止忽略")]]
    virtual ProviderCapabilities capabilities() = 0;

    // 构造请求体
    [[nodiscard("禁止忽略")]]
    virtual boost::json::object buildRequest(const ChatRequest& request) = 0;

    // 处理回复体
    [[nodiscard("禁止忽略")]]
    virtual ProviderResponse parseResponse(std::string_view body) = 0;

    // 处理流缓存块
    virtual std::vector<StreamEvent> parseStreamChunk(std::string_view chunk) = 0;

    // 启用能力
    virtual bool enableCapability(std::string capability) = 0;

    // 禁用能力
    virtual bool disableCapability(std::string capability) = 0;
private:
    ProviderCapabilities providerCapabilities;
    ProviderConfig providerConfig;
};
