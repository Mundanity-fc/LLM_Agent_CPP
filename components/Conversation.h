#pragma once
#include "./Chat.h"

class Conversation {
public:
    Conversation();

    void append(const ChatMessage& message);

    [[nodiscard]]
    const std::vector<ChatMessage>& messages() const noexcept;

    [[nodiscard]]
    std::vector<ChatMessage> snapshot() const;

    void clear();

    ~Conversation();

private:
    std::vector<ChatMessage> messageHistory;
};