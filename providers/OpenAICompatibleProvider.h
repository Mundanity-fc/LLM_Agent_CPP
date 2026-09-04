#pragma once
#include "Provider.h"
#include <boost/json.hpp>


class OpenAICompatibleProvider : public Provider{
public:
    /**
     * @brief 构造函数
     * @param config 模型提供商的配置信息
     * @return 构造完成的提供商对象
     */
    explicit OpenAICompatibleProvider(ProviderConfig config);

    /**
     * @brief 列出模型的配置信息
     * @return 模型提供商的配置信息
     */
    ProviderConfig getConfig();

    /**
     * @brief 列出模型的能力信息
     * @return 模型提供商的能力信息
     */
    ProviderCapabilities capabilities() override;

    /**
     * @brief 构造对应的 JSON 请求体
     * @param request 从对话历史中生成的请求信息
     * @return 构造完成的 JSON 请求体
     */
    boost::json::object buildRequest(const ChatRequest& request) override;

    /**
     * @brief 处理 HTTP 的响应信息
     * @param body 获取的 HTTP 的响应体内容
     * @return 处理完成的标准响应信息
     */
    ProviderResponse parseResponse(std::string_view body) override;
    // 处理流式块
    std::vector<StreamEvent> parseStreamChunk(std::string_view chunk) override;

    /**
     * @brief 启用模型的某种能力
     * @param capability 目标能力的名称
     * @return 存在该能力且能启用，返回真，否则为否
     */
    bool enableCapability(std::string capability) override;

    /**
     * @brief 禁用模型的某种能力
     * @param capability 目标能力的名称
     * @return 存在该能力且能禁用，返回真，否则为否
     */
    bool disableCapability(std::string capability) override;




    /**
     * @brief 检测模型是否启用流式输出
     * @return 启用了流式输出，返回真，否则为否
     */
    [[nodiscard]]
    bool isStreaming() const;


    /**
     * @brief 检测模型是否启用思考模式
     * @return 启用了思考模式，返回真，否则为否
     */
    [[nodiscard]]
    bool isReasoning() const;

    /**
     * @brief 提供商对象的析构函数
     * @return 析构本对象
     */
    ~OpenAICompatibleProvider() override;

private:
    // 提供商配置
    ProviderConfig providerConfig;
    // 提供商能力
    ProviderCapabilities providerCapabilities;
    // 流式缓存
    std::string streamBuffer;
};