#include <utility>
#include "OpenAICompatibleProvider.h"

OpenAICompatibleProvider::OpenAICompatibleProvider(ProviderConfig config) {
    providerConfig = std::move(config);
}

ProviderConfig OpenAICompatibleProvider::getConfig() {
    return providerConfig;
}

boost::json::object OpenAICompatibleProvider::buildRequest(const ChatRequest &request) {
    return boost::json::object{};
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