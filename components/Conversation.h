#pragma once
#include "./Chat.h"
#include "../providers/Provider.h"

class Conversation {
public:
    Conversation();

    bool append(const ChatMessage& message);

    bool append(const ProviderResponse& modelResponse);

    [[nodiscard]]
    const std::vector<ChatMessage>& messages() const noexcept;

    [[nodiscard]]
    std::vector<ChatMessage> snapshot() const;

    void clear();

    ~Conversation();

private:
    std::vector<ChatMessage> messageHistory;
};