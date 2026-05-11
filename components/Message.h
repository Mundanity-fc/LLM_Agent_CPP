#pragma once
#include <string>
#include <utility>

enum MessageRole {
    System,
    User,
    Assistant
};

class Message {
public:
    explicit Message(const MessageRole roleNumber, std::string content = ""): role(roleNumber), content(std::move(content)), tokenCount(0) {}
    void updateContent(std::string& content);
    void updateContent(const char * str);
    void updateTokenCount(int tokenCount);
    ~Message() = default;


private:
    MessageRole role;
    int tokenCount;
    std::string content;
};