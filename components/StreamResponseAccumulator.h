#pragma once
#include <string>
#include <vector>
#include "StreamComponents.h"
#include "ProviderComponents.h"
#include "TokenComponents.h"

class StreamResponseAccumulator {
public:
    /**
     * @brief 事件应用函数
     * @param event 流式输出的当前事件
     */
    void apply(const StreamEvent& event);

    [[nodiscard]]
    /**
     * @brief 流式累加处理结束
     * @return 返回标准的回复对象
     */
    ProviderResponse finish() const;

private:
    std::string reasoningText;
    std::string answerText;

    // std::vector<ToolCall> toolCalls;

    TokenUsage usage;
    std::string finishReason;

    bool completed{false};
};