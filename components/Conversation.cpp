#include "Conversation.h"
#include <boost/json/serialize.hpp>

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

void Conversation::append(const ChatMessage& message) {
    messageHistory.push_back(message);
}

const std::vector<ChatMessage> & Conversation::messages() const noexcept {
    return messageHistory;
}

std::vector<ChatMessage> Conversation::snapshot() const {
    return messageHistory;
}