#include "Message.h"

void Message::editSystemPrompt(const std::string &newPrompt) {
    this->systemPrompt = newPrompt;
}

void Message::editUserPrompt(const std::string &newPrompt) {
    this->userPrompt = newPrompt;
}

boost::json::array Message::getJsonPrompt() const {
    boost::json::object systemPrompt;
    systemPrompt["role"] = "system";
    systemPrompt["content"] = this->systemPrompt;
    boost::json::object userPrompt;
    userPrompt["role"] = "user";
    userPrompt["content"] = this->userPrompt;
    boost::json::array prompt;
    prompt.push_back(systemPrompt);
    prompt.push_back(userPrompt);
    return prompt;
}

std::string Message::getStringPrompt() const {
    return boost::json::serialize(getJsonPrompt());
}