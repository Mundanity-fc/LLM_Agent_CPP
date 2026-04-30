#include "deepseek.h"
#include <iostream>
#include <utility>

deepseek::deepseek() {
    this->hasStreamMode = true;
    this->streamMode = false;
    this->hasThinkingMode = true;
    this->thinkingMode = false;
    this->isStateless = false;
}

void deepseek::setApiConfig(std::string model, std::string host, std::string port, std::string target, std::string apikey) {
    this->model = std::move(model);
    this->host = std::move(host);
    this->port = std::move(port);
    this->target = std::move(target);
    this->apikey = std::move(apikey);
}

boost::json::object deepseek::createMessage(std::string& message) {
    boost::json::object body;
    // 消息内容
    body["model"] = this->model;
    body["messages"] = message;
    body["stream"] = this->streamMode;
    // 思考模式参数
    boost::json::object thinking;
    thinking["type"] = "disabled";
    body["thinking"] = thinking;
    return body;
}

Message& deepseek::receiveMessage(boost::beast::ssl_stream<boost::beast::tcp_stream> stream) {
    Message msg;
    if (this->streamMode) {
        // 流式输出处理

    } else {
        // 非流式输出处理

    }
    return msg;
}

void deepseek::printInformation() {
    std::cout << "当前的模型为" << this->getModelName() << std::endl;
    std::cout << "连接的地址为：" << this->getHost() << ":" << this->getPort() << this->target << std::endl;
}

std::string deepseek::getModelName() {
    return this->model;
}

std::string deepseek::getHost() {
    return this->host;
}

std::string deepseek::getPort() {
    return this->port;
}

std::string deepseek::getApiKey() {
    return this->apikey;
}

void deepseek::setStreamMode(const bool mode) {
    if (!this->hasStreamMode) {
        std::cerr << "当前模型不支持流式输出" << std::endl;
        this->streamMode = false;
    }else {
        this->streamMode = mode;
    }
}

void deepseek::setThinkingMode(const bool mode) {
    if (!this->hasThinkingMode) {
        std::cerr << "当前模型不支持思考模式" << std::endl;
        this->thinkingMode = false;
    }else {
        this->thinkingMode = mode;
    }
}

void deepseek::setStateless(const bool stateless) {
    this->isStateless = stateless;
}

deepseek::~deepseek() {
}
