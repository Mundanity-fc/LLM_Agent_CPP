#include <iostream>
#include <fstream>
#include <string>
#include <boost/json.hpp>
#include <boost/beast.hpp>

#include "LLM_Client.h"
#include "Message.h"

int main() {
#ifdef __WIN32__
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF8");
#endif
    // 配置读取
    boost::json::array providers;
    boost::json::object model;
    {
        std::fstream env_config("./env.json", std::ios::in);
        std::string content;
        if(env_config.good()) {
            content = std::string ((std::istreambuf_iterator<char>(env_config)), std::istreambuf_iterator<char>());
        }
        env_config.close();
        boost::json::value config = boost::json::parse(content);
        providers = config.at("providers").as_array();
        model = providers[0].as_object();
    }
    try {
        const std::string name = model.at("model").as_string().c_str();
        const std::string host = model.at("host").as_string().c_str();
        const std::string port = model.at("port").as_string().c_str();
        const std::string target = model.at("target").as_string().c_str();
        const std::string apikey = model.at("apikey").as_string().c_str();

        LLM_Client connection(name, host, port, target, apikey);
        connection.connect();

        Message msg;
        msg.addSystemPrompt("你是一个AI助理");

        std::cout <<"系统提示词：你是一个AI助理" << std::endl;
        std::cout <<"用户输入：";
        std::string prompt;
        std::cin >> prompt;
        while (prompt != "quit") {
            msg.addUserPrompt(prompt);
            connection.sendMessage(msg.getJsonPrompt());
            std::cout << "模型回答：";
            std::string assistantResponse = connection.getResponse();
            msg.addAssistantPrompt(assistantResponse);
            std::cout<<std::endl;
            std::cout <<"用户输入：";
            std::cin >> prompt;
        }
        // 6. 优雅地关闭连接
        connection.shutdown();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}