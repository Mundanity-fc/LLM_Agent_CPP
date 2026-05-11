#include "MessageList.h"

MessageList::MessageList(int initialCapacity) {
    list.reserve(initialCapacity);
}

Message & MessageList::getSystemPrompt() {
    if (!list.empty()) {
        return list.front();
    }
    Message msg(System);
    return msg;
}

void MessageList::insertMessage(Message& msg) {
    list.push_back(std::move(msg));
}

Message & MessageList::operator[](int index) {
    return list[index];
}

Message & MessageList::top() {
    return list.back();
}
