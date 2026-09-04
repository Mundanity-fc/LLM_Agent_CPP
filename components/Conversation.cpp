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
    messageHistory.push_back(modelResponse.assistantMessage);
    return true;
}

const std::vector<ChatMessage> & Conversation::messages() const noexcept {
    return messageHistory;
}

std::vector<ChatMessage> Conversation::snapshot() const {
    return messageHistory;
}