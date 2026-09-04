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