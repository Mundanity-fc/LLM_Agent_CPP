#include "StreamResponseAccumulator.h"

#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>

#include "../ReasoningComponent.h"

void StreamResponseAccumulator::apply(const StreamEvent& event){
    switch (event.type) {
        // 推理内容增量，直接增加推理文本
        case StreamEvent::Type::ReasoningDelta:
            reasoningText += event.text;
            break;
        // 回答内容增量，直接增加回答文本
        case StreamEvent::Type::TextDelta:
            answerText += event.text;
            break;
        // 工具调用增量，调用对应工具
        case StreamEvent::Type::ToolCallDelta:
            //accumulateToolCall(event.data);
            break;
        // 用量统计，统计用量
        case StreamEvent::Type::Usage:
            //usage = parseUsage(event.data);
            break;
        // 事件完成
        case StreamEvent::Type::Completed:
            completed = true;
            break;
        // 事件出错
        case StreamEvent::Type::Error:
            break;

        default:
            break;
    }
}


ProviderResponse StreamResponseAccumulator::finish() const {
    ProviderResponse response;

    response.assistantMessage = ChatMessage{
        .role = MessageRole::Assistant,
        .content = answerText
    };

    // response.assistantMessage.toolCalls =
    //     encodeToolCalls(toolCalls_);

    response.reasoningOutput = ReasoningOutput{
        .kind = reasoningText.empty() ? ReasoningOutputKind::None : ReasoningOutputKind::Trace,
        .text = reasoningText
    };

    response.finishReason = finishReason;
    // response.usage = usage;

    return response;
}