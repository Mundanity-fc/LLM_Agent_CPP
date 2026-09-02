#pragma once
#include <optional>
#include <string>
#include <boost/json/object.hpp>

// 消息角色类型
enum class MessageRole {
    System,
    User,
    Assistant
};

// 对话消息内容结构
struct ChatMessage {
    MessageRole role;
    boost::json::value content;

    std::optional<std::string> name;
    std::optional<std::string> toolCallId;
    boost::json::array toolCalls;
};

// 对话消息请求结构
struct ChatRequest {
    std::string model;
    boost::json::array messages;

    bool stream{false};
    boost::json::array tools;
    boost::json::value toolChoice;

    std::optional<double> temperature;
    std::optional<std::size_t> maxOutputTokens;

    boost::json::object extraBody;
};