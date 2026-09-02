#include <utility>
#include "OpenAICompatibleProvider.h"

#include <iostream>

OpenAICompatibleProvider::OpenAICompatibleProvider(ProviderConfig config) {
    providerConfig = std::move(config);
}

ProviderConfig OpenAICompatibleProvider::getConfig() {
    return providerConfig;
}

boost::json::object OpenAICompatibleProvider::buildRequest(const ChatRequest &request) {
    boost::json::object RequestBody;
    RequestBody["model"] = request.model.empty() ? providerConfig.model : request.model;
    RequestBody["messages"] = request.messages;
    return RequestBody;
}

ProviderCapabilities OpenAICompatibleProvider::capabilities() {
    return ProviderCapabilities{};
}

ProviderResponse OpenAICompatibleProvider::parseResponse(std::string_view body) {
    boost::json::object ResponseBody = boost::json::parse(body).as_object();
    if (boost::json::value *err = ResponseBody.if_contains("error")) {
        std::cerr << "Error: " << err->at("message") << std::endl;
        return ProviderResponse{
            "",
            std::string(err->at("message").as_string()),
            {},
            "",
            {},
            boost::json::parse(body)
        };
    }
    return ProviderResponse{
        std::string(ResponseBody.at("id").as_string()),
        std::string(ResponseBody.at("choices").as_array()[0].at("message").as_object().at("content").as_string()),
        {},
        std::string(ResponseBody.at("choices").as_array()[0].at("message").as_object().at("reasoning_content").as_string()),
        {},
        boost::json::parse(body)
    };
}

std::vector<StreamEvent> OpenAICompatibleProvider::parseStreamChunk(std::string_view chunk) {
    return std::vector<StreamEvent>{};
}

OpenAICompatibleProvider::~OpenAICompatibleProvider() = default;