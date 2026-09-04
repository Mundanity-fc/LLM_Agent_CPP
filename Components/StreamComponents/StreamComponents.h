#pragma once
#include <boost/json/value.hpp>

// 消息流事件
struct StreamEvent {
    enum class Type {
        ReasoningStarted,
        ReasoningDelta,
        ReasoningCompleted,

        TextDelta,
        ToolCallDelta,
        Usage,

        Completed,
        Error
    };

    Type type;
    std::string text;
    boost::json::value data;
};

//  SSE 消息的边界定位
struct SSEBoundary {
    std::size_t position;
    std::size_t length;
};

std::optional<SSEBoundary> findSSEBoundary(const std::string &buffer);
std::string takeSSEEvent(std::string &buffer, const SSEBoundary &boundary);
StreamEvent parseStreamJson(boost::json::object &eventObject);