#pragma once
#include "Message.h"
#include <vector>

class MessageList {
public:
    MessageList(int initialCapacity);
    Message& getSystemPrompt();
    void insertMessage(Message& msg);
    Message& operator[](int index);
    Message& top();
private:
    std::vector<Message> list;
};
