#pragma once
#include <string>
#include "../MessageComponents/Chat.h"
#include "../ReasoningComponent.h"

// 具体连接类型结构
struct ProviderConfig {
    // 配置名称
    std::string name;
    // 模型名称
    std::string model;
    // 提供商主机
    std::string host;
    // 提供商端口
    std::string port;
    // 通信协议
    std::string protocol;
    // 目标路径
    std::string target;
    // 提供商 APIKey
    std::string apikey;
};

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
    std::string model;

    ChatMessage assistantMessage;
    ReasoningOutput reasoningOutput;
    std::string finishReason;
    std::string errorContent;

    boost::json::array toolCalls;

    boost::json::object usage;
    boost::json::value raw;
};