#include <iostream>
#include <fstream>
#include <string>
#include "providers/OpenAICompatibleProvider.h"
#include "llm/HttpClient.h"
#include "components/Conversation.h"
#include "components/ChatRequestBuilder.h"

int main() {
#ifdef __WIN32__
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF8");
#endif
    // 配置读取
    boost::json::array providerList;
    boost::json::object selectedModel;
    {
        std::fstream env_config("./env.json", std::ios::in);
        std::string content;
        if(env_config.good()) {
            content = std::string ((std::istreambuf_iterator<char>(env_config)), std::istreambuf_iterator<char>());
        }
        env_config.close();
        boost::json::value config = boost::json::parse(content);
        providerList = config.at("providers").as_array();
        selectedModel = providerList[0].as_object();
    }

    OpenAICompatibleProvider deepseek({
        std::string(selectedModel.at("name").as_string()),
        std::string(selectedModel.at("model").as_string()),
        std::string(selectedModel.at("host").as_string()),
        std::string(selectedModel.at("port").as_string()),
        std::string(selectedModel.at("protocol").as_string()),
        std::string(selectedModel.at("target").as_string()),
        std::string(selectedModel.at("apikey").as_string())
    });
    HttpClient http(deepseek.getConfig());
    Conversation conversation;
    ChatMessage message;
    ProviderResponse response;
    message = {
        MessageRole::System,
        "你是一个运行在终端的智能助手，需要回答用户输入的问题。"
    };
    conversation.append(message);
    message.role = MessageRole::User;

    std::cout <<"用户输入：";
    std::string prompt;
    std::cin >> prompt;
    while (prompt != "quit") {
        message.content = prompt;
        conversation.append(message);
        http.sendMessage(deepseek.buildRequest(ChatRequestBuilder::build(conversation)));
        response = deepseek.parseResponse(http.receiveMessage().body());
        conversation.append(response);
        std::cout << "模型回复：" << response.text << std::endl;
        std::cout<<std::endl;
        std::cout <<"用户输入：";
        std::cin >> prompt;
    }
    std::cout << "对话结束，历史对话如下：\n" << ChatRequestBuilder::build(conversation).messages << std::endl;
    return 0;
}