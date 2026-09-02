#pragma once
#include "Conversation.h"
#include "Chat.h"

class ChatRequestBuilder {
public:
    [[nodiscard]]
    static ChatRequest build(const Conversation& conversation);
private:
    [[nodiscard]]
    static boost::json::object getMessageJson(const ChatMessage& message);
};
