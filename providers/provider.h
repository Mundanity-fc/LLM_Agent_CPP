#pragma once
#include <string>
#include "../components/Message.h"
#include <boost/json.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

class provider {
public:
    // 向Provider API发送消息
    virtual boost::json::object createMessage(std::string& message) = 0;
    // 接收Provider API的返回消息并对可能的流式消息进行处理
    virtual Message& receiveMessage(boost::beast::ssl_stream<boost::beast::tcp_stream> stream) = 0;
    // 输出当前Provider和模型的信息
    virtual void printInformation() = 0;
    // 获取当前模型的名称
    virtual std::string getModelName() = 0;
    // 获取当前Provider的API主机信息
    virtual std::string getHost() = 0;
    // 获取当前Provider的API端口信息
    virtual std::string getPort() = 0;
    // 获取当前Provider的API接口信息
    virtual std::string getTarget() = 0;
    // 获取当前Provider的API密钥信息
    virtual std::string getApiKey() = 0;
    // 修改当前模型的流式输出状态
    virtual void setStreamMode(bool mode) = 0;
    // 修改当前模型的思考模式状态
    virtual void setThinkingMode(bool mode) = 0;
    // 修改当前模型的无状态/有状态模式
    virtual void setStateless(bool stateless) = 0;
    // 虚析构
    virtual ~provider() = default;
};