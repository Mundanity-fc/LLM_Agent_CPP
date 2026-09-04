#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include "llm/HttpClient.h"
#include "Components/ProviderComponents/Providers/OpenAI/OpenAICompatibleProvider.h"
#include "Components/MessageComponents/Conversation.h"
#include "Components/MessageComponents/ChatRequestBuilder.h"

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
        std::fstream env_config(std::filesystem::path{LLM_AGENT_CONFIG_DIR}/"env.json", std::ios::in);
        std::string content;
        if(env_config.good()) {
            content = std::string ((std::istreambuf_iterator<char>(env_config)), std::istreambuf_iterator<char>());
        }
        env_config.close();
        boost::json::value config = boost::json::parse(content);
        providerList = config.at("providers").as_array();
        selectedModel = providerList[0].as_object();
    }

    // 构建提供商
    OpenAICompatibleProvider deepseek({
        .name = std::string(selectedModel.at("name").as_string()),
        .model = std::string(selectedModel.at("model").as_string()),
        .host = std::string(selectedModel.at("host").as_string()),
        .port = std::string(selectedModel.at("port").as_string()),
        .protocol = std::string(selectedModel.at("protocol").as_string()),
        .target = std::string(selectedModel.at("target").as_string()),
        .apikey = std::string(selectedModel.at("apikey").as_string())
    });
    // 构建通信器
    HttpClient http(deepseek.getConfig());
    // 构建上下文管理器
    Conversation conversation;
    // 构建对话消息管理器
    ChatMessage message;
    // 构建回复管理
    ProviderResponse response;
    //
    deepseek.enableCapability("reasoning");
    // 系统提示词
    message = {
        MessageRole::System,
        "你是一个运行在终端的智能助手，需要回答用户输入的问题。"
    };
    conversation.append(message);
    // 用户消息
    message.role = MessageRole::User;
    std::cout <<"用户输入：";
    std::string prompt;
    std::cin >> prompt;
    while (prompt != "quit") {
        // 消息生成与发送
        message.content = prompt;
        conversation.append(message);
        http.sendMessage(deepseek.buildRequest(ChatRequestBuilder::build(conversation)));
        // 获取回复
        if (deepseek.isStreaming()) {

        }else{
            response = deepseek.parseResponse(http.receiveMessage().body());
            conversation.append(response);
            if (response.reasoningOutput.kind != ReasoningOutputKind::None) {
                std::cout << "模型思考：" << response.reasoningOutput.text << std::endl << std::endl;
            }
            std::cout << "模型回复：" << response.assistantMessage.content << std::endl;
            std::cout<<std::endl;
        }
        // 新一轮对话
        std::cout <<"用户输入：";
        std::cin >> prompt;
    }
    std::cout << "对话结束，历史对话如下：\n" << ChatRequestBuilder::build(conversation).messages << std::endl;
    return 0;
}