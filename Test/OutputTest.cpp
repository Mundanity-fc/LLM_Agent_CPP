#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "llm/HttpClient.h"
#include "Components/ProviderComponents/Providers/OpenAI/OpenAICompatibleProvider.h"
#include "Components/MessageComponents/Conversation.h"
#include "Components/MessageComponents/ChatRequestBuilder.h"
#include "Components/StreamComponents/StreamResponseAccumulator.h"

TEST(OutputTestWithoutStream, withoutReasoning) {
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
    OpenAICompatibleProvider deepseek({
        .name = std::string(selectedModel.at("name").as_string()),
        .model = std::string(selectedModel.at("model").as_string()),
        .host = std::string(selectedModel.at("host").as_string()),
        .port = std::string(selectedModel.at("port").as_string()),
        .protocol = std::string(selectedModel.at("protocol").as_string()),
        .target = std::string(selectedModel.at("target").as_string()),
        .apikey = std::string(selectedModel.at("apikey").as_string())
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
    message.content = "介绍一下你自己";
    conversation.append(message);
    http.sendMessage(deepseek.buildRequest(ChatRequestBuilder::build(conversation)));
    response = deepseek.parseResponse(http.receiveMessage().body());
    conversation.append(response);
    if (response.reasoningOutput.kind != ReasoningOutputKind::None) {
        std::cout << "模型思考：" << response.reasoningOutput.text << std::endl << std::endl;
    }
    std::cout << "模型回复：" << response.assistantMessage.content << std::endl;

}

TEST(OutputTestWithoutStream, withReasoning) {
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
    OpenAICompatibleProvider deepseek({
        .name = std::string(selectedModel.at("name").as_string()),
        .model = std::string(selectedModel.at("model").as_string()),
        .host = std::string(selectedModel.at("host").as_string()),
        .port = std::string(selectedModel.at("port").as_string()),
        .protocol = std::string(selectedModel.at("protocol").as_string()),
        .target = std::string(selectedModel.at("target").as_string()),
        .apikey = std::string(selectedModel.at("apikey").as_string())
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
    message.content = "介绍一下你自己";
    conversation.append(message);
    deepseek.enableCapability("reasoning");
    http.sendMessage(deepseek.buildRequest(ChatRequestBuilder::build(conversation)));
    response = deepseek.parseResponse(http.receiveMessage().body());
    conversation.append(response);
    if (response.reasoningOutput.kind != ReasoningOutputKind::None) {
        std::cout << "模型思考：" << response.reasoningOutput.text << std::endl << std::endl;
    }
    std::cout << "模型回复：" << response.assistantMessage.content << std::endl;
}

TEST(OutputTestWithStream, withoutReasoning) {
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
    OpenAICompatibleProvider deepseek({
        .name = std::string(selectedModel.at("name").as_string()),
        .model = std::string(selectedModel.at("model").as_string()),
        .host = std::string(selectedModel.at("host").as_string()),
        .port = std::string(selectedModel.at("port").as_string()),
        .protocol = std::string(selectedModel.at("protocol").as_string()),
        .target = std::string(selectedModel.at("target").as_string()),
        .apikey = std::string(selectedModel.at("apikey").as_string())
    });
    deepseek.enableCapability("streaming");
    HttpClient http(deepseek.getConfig());
    Conversation conversation;
    ChatMessage message;
    ProviderResponse response;
    message = {
        MessageRole::System,
        "你是一个运行在终端的智能助手，需要回答用户输入的问题，回复尽量简单，不超过20字。"
    };
    conversation.append(message);
    message.role = MessageRole::User;
    message.content = "介绍一下你自己";
    conversation.append(message);
    http.sendMessage(deepseek.buildRequest(ChatRequestBuilder::build(conversation)));
    {
        StreamResponseAccumulator accumulator;
        http.receiveStream([&](std::string_view bytes) {
            auto events = deepseek.parseStreamChunk(bytes);
            for (const auto& event : events) {
                accumulator.apply(event);
            }
        });
    }
}