#pragma once
#include "provider.h"


class deepseek: public provider {
public:
    deepseek();
    void setApiConfig(std::string model, std::string host, std::string port, std::string target, std::string apikey);
    boost::json::object createMessage(std::string& message) override;
    Message& receiveMessage(boost::beast::ssl_stream<boost::beast::tcp_stream> stream) override;
    void printInformation() override;
    std::string getModelName() override;
    std::string getHost() override;
    std::string getPort() override;
    std::string getApiKey() override;
    void setStreamMode(bool mode) override;
    void setThinkingMode(bool mode) override;
    void setStateless(bool stateless) override;
    ~deepseek() override;
private:
    /*
     * 模型API参数
     * model为Provider中可调用的模型名称
     * host为Provider的API地址
     * port为Provider的API地址端口（80/443）
     * target为Provider的API具体接口
     * apikey为Provider提供的密钥
     * hasStreamMode为当前模型是否支持流式输出标记
     * steamMode为当前模型的流式输出状态标记
     * hasThinkingMode为当前模型是否支持思考模式标记
     * thinkingMode为当前模型的思考模式状态标记
     * isStateless为当前模型启用无状态（关闭历史对话）的标记
     */
    std::string model;
    std::string host;
    std::string port;
    std::string target;
    std::string apikey;
    bool hasStreamMode;
    bool streamMode;
    bool hasThinkingMode;
    bool thinkingMode;
    bool isStateless;
};