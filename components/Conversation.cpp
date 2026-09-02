#include "Conversation.h"

Conversation::Conversation() {
    messageHistory.reserve(10000);
}

Conversation::~Conversation() {
    clear();
}

void Conversation::clear() {
    messageHistory.clear();
    messageHistory.reserve(100000);
}

bool Conversation::append(const ChatMessage& message) {
    messageHistory.push_back(message);
    return true;
}

bool Conversation::append(const ProviderResponse& modelResponse){
    ChatMessage assistantContent = {
        MessageRole::Assistant,
        boost::json::value(modelResponse.text)
    };
    messageHistory.push_back(assistantContent);
    return true;
}

const std::vector<ChatMessage> & Conversation::messages() const noexcept {
    return messageHistory;
}

std::vector<ChatMessage> Conversation::snapshot() const {
    return messageHistory;
}