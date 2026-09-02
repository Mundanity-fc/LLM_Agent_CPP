#include <iostream>
#include <fstream>
#include <string>
#include "providers/OpenAICompatibleProvider.h"
#include "llm/HttpClient.h"
#include "components/Conversation.h"
#include "components/ChatRequestBuilder.h"

int main() {
    // OpenAICompatibleProvider local({
    //     "local",
    //     "/home/mundanity/LLMs/Qwen3.5-2B",
    //     "127.0.0.1",
    //     "8000",
    //     "http",
    //     "/v1/chat/completions",
    //     "key"
    // });
    // HttpClient client(local.getConfig());
    // client.sendMessage();
    // std::cout << client.receiveMessage().body() << std::endl;
    //
    //
    OpenAICompatibleProvider ds({
        "ds",
        "deepseek-v4-flash",
        "api.deepseek.com",
        "443",
        "https",
        "/chat/completions",
        "KEY"
    });
    HttpClient client2(ds.getConfig());
    // client2.sendMessage();
    // std::cout << client2.receiveMessage().body() << std::endl;
    Conversation conversation;
    ChatMessage message{
        MessageRole::System,
        "你是一个文本生成器，需要根据用户的输入来继续生成对应的文本。"
    };
    conversation.append(message);
    message.role = MessageRole::User;
    message.content = "C++是一个面向对象的语言，它以";
    conversation.append(message);
    client2.sendMessage(ds.buildRequest(ChatRequestBuilder::build(conversation)));
    std::cout << client2.receiveMessage().body() << std::endl;

    return 0;
}