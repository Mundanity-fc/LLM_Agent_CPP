#include <iostream>
#include <fstream>
#include <string>
#include "providers/OpenAICompatibleProvider.h"
#include "llm/HttpClient.h"

int main() {
    OpenAICompatibleProvider local({
        "local",
        "/home/mundanity/LLMs/Qwen3.5-2B",
        "127.0.0.1",
        "8000",
        "http",
        "/v1/chat/completions",
        "key"
    });
    HttpClient client(local.getConfig());
    client.sendMessage();
    std::cout << client.receiveMessage().body() << std::endl;


    OpenAICompatibleProvider ds({
        "ds",
        "deepseek-v4-pro",
        "api.deepseek.com",
        "443",
        "https",
        "/chat/completions",
        "KEY"
    });
    HttpClient client2(ds.getConfig());
    client2.sendMessage();
    std::cout << client2.receiveMessage().body() << std::endl;

    return 0;
}