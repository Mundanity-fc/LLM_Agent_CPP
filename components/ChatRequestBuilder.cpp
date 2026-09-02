#include "ChatRequestBuilder.h"

ChatRequest ChatRequestBuilder::build(const Conversation &conversation) {
    ChatRequest request;
    boost::json::array allMsg;
    for (const auto& it : conversation.snapshot()) {
        allMsg.push_back(getMessageJson(it));
    }
    request.messages = allMsg;
    return request;
}

boost::json::object ChatRequestBuilder::getMessageJson(const ChatMessage& message) {
    boost::json::object msgObj;
    switch (message.role) {
        case MessageRole::System:
            msgObj["role"] = "system";
            break;
        case MessageRole::User:
            msgObj["role"] = "user";
            break;
        case MessageRole::Assistant:
            msgObj["role"] = "assistant";
            break;
    }
    msgObj["content"] = message.content;
    return msgObj;
}