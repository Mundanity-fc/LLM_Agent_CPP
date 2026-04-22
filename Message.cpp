#include "Message.h"

void Message::addSystemPrompt(const std::string &newPrompt) {
    boost::json::object systemPrompt;
    systemPrompt["role"] = "system";
    systemPrompt["content"] = newPrompt;
    history.push_back(systemPrompt);
    totalHistory++;
}

void Message::addUserPrompt(const std::string &newPrompt) {
    boost::json::object userPrompt;
    userPrompt["role"] = "user";
    userPrompt["content"] = newPrompt;
    history.push_back(userPrompt);
    totalHistory++;
    totalUserHistory++;
}

void Message::addAssistantPrompt(const std::string &newPrompt) {
    boost::json::object assistantPrompt;
    assistantPrompt["role"] = "assistant";
    assistantPrompt["content"] = newPrompt;
    history.push_back(assistantPrompt);
    totalHistory++;
    totalAssistantHistory++;
}

boost::json::array Message::getJsonPrompt() const {
    boost::json::array prompt;
    for (const auto& item : history) {
        prompt.push_back(item);
    }
    return prompt;
}

std::string Message::getStringPrompt() const {
    return boost::json::serialize(getJsonPrompt());
}