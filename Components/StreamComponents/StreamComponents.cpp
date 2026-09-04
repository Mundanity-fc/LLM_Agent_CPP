#include "StreamComponents.h"
#include <boost/json/parse.hpp>


std::optional<SSEBoundary> findSSEBoundary(const std::string &buffer) {
    for (int i = 0; i < static_cast<int>(buffer.length()); i++) {
        // \n\n 型分割
        if (buffer[i] == '\n' && i + 1 < static_cast<int>(buffer.length()) && buffer[i + 1] == '\n') {
            return SSEBoundary{
                .position = static_cast<std::size_t>(i),
                .length = 2
            };
        }
        // \r\n\r\n 型分割
        if (buffer[i] == '\r' &&
            i + 3 < static_cast<int>(buffer.length()) &&
            buffer[i + 1] == '\n' &&
            buffer[i + 2] == '\r' &&
            buffer[i + 3] == '\n') {
            return SSEBoundary{
                .position = static_cast<std::size_t>(i),
                .length = 4
            };
        }
    }
    return std::nullopt;
}

std::string takeSSEEvent(std::string &buffer, const SSEBoundary &boundary) {
    // 字符串截取到边界标记为
    std::string event = buffer.substr(0, boundary.position);
    // 截取的字符串和标记全部清除
    buffer.erase(0,boundary.position + boundary.length);
    return event;
}

StreamEvent parseStreamJson(boost::json::object &eventObject) {

    // finish_reason 非空，输出流结束
    if (eventObject.at("choices").as_array()[0].at("finish_reason") != nullptr) {
        return {
            .type = StreamEvent::Type::Completed,
            .text = std::string(eventObject.at("choices").as_array()[0].at("finish_reason").as_string()),
            .data = eventObject
        };
    }
    // 处理 Delta 部分
    boost::json::object deltaObject = eventObject.at("choices").as_array()[0].at("delta").as_object();
    // 无Content，为推理过程
    if (deltaObject.at("content") == nullptr) {
        // 推理开始
        if (deltaObject.at("reasoning_content") == "") {
            return {
                .type = StreamEvent::Type::ReasoningStarted,
                .text = "",
                .data = eventObject
            };
        }
        // 推理增量
        return {
            .type = StreamEvent::Type::ReasoningDelta,
            .text = std::string(deltaObject.at("reasoning_content").as_string()),
            .data = eventObject
        };
    }
    return {
        .type = StreamEvent::Type::TextDelta,
        .text = std::string(deltaObject.at("content").as_string()),
        .data = eventObject
    };
}