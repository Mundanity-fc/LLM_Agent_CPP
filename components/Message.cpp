#include "Message.h"

void Message::updateContent(std::string& content) {
    this->content = std::move(content);
}

void Message::updateContent(const char *str) {
    this->content = str;
}

void Message::updateTokenCount(int tokenCount) {
    this->tokenCount = tokenCount;
}
