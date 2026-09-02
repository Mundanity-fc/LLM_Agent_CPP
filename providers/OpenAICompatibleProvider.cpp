#include <utility>
#include "OpenAICompatibleProvider.h"

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
    return ProviderResponse{};
}

std::vector<StreamEvent> OpenAICompatibleProvider::parseStreamChunk(std::string_view chunk) {
    return std::vector<StreamEvent>{};
}

OpenAICompatibleProvider::~OpenAICompatibleProvider() = default;