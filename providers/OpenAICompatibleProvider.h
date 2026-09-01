#pragma once
#include "Provider.h"
#include "../components/ProviderConfig.h"
#include <boost/json.hpp>


class OpenAICompatibleProvider : public Provider{
public:
    explicit OpenAICompatibleProvider(ProviderConfig config);

    ProviderConfig getConfig();

    ProviderCapabilities capabilities() override;

    boost::json::object buildRequest(const ChatRequest& request) override;

    ProviderResponse parseResponse(std::string_view body) override;

    std::vector<StreamEvent> parseStreamChunk(std::string_view chunk) override;

    ~OpenAICompatibleProvider() override;

private:
    ProviderConfig providerConfig;
    std::string streamBuffer;
};