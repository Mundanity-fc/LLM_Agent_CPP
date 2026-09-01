#pragma once

#include <string>

// 具体连接类型结构
struct ProviderConfig {
    // 配置名称
    std::string name;
    // 模型名称
    std::string model;
    // 提供商主机
    std::string host;
    // 提供商端口
    std::string port;
    // 通信协议
    std::string protocol;
    // 目标路径
    std::string target;
    // 提供商 APIKey
    std::string apikey;
};