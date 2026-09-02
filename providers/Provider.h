#pragma once
#include <boost/json.hpp>
#include "../components/ProviderConfig.h"
#include "../components/Chat.h"


// 提供商可选功能
struct ProviderCapabilities {
    bool streaming{false};
    bool tools{false};
    bool parallelToolCalls{false};
    bool structuredOutput{false};
    bool reasoning{false};
    bool vision{false};
};

// 提供商消息回复
struct ProviderResponse {
    std::string id;
    std::string text;
    boost::json::array toolCalls;
    std::string finishReason;
    boost::json::object usage;
    boost::json::value raw;
};

// 消息流事件
struct StreamEvent {
    enum class Type {
        TextDelta,
        ToolCallDelta,
        Completed,
        Error
    };

    Type type;
    std::string text;
    boost::json::value data;
};

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
};
