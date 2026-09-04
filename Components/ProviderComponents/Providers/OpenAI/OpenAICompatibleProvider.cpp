#include <utility>
#include <iostream>
#include <boost/json.hpp>
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
    if (providerCapabilities.reasoning) {
        boost::json::object reasoning;
        reasoning["type"] = "enabled";
        RequestBody["thinking"] = reasoning;
    } else {
        boost::json::object reasoning;
        reasoning["type"] = "disabled";
        RequestBody["thinking"] = reasoning;
    }
    if (providerCapabilities.streaming) {
        RequestBody["stream"] = true;
    }
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
            .id = "",
            .model = "",
            .finishReason ="Error",
            .errorContent = std::string(err->at("message").as_string()),
            .raw = boost::json::parse(body)
        };
    }

    if (providerCapabilities.reasoning) {
        return ProviderResponse{
            .id = std::string(ResponseBody.at("id").as_string()),
            .assistantMessage = {
                .role = MessageRole::Assistant,
                .content = std::string(ResponseBody.at("choices").as_array()[0].at("message").as_object().at("content").as_string())
            },
            .reasoningOutput = {
                .kind = ReasoningOutputKind::Trace,
                .text = std::string(ResponseBody.at("choices").as_array()[0].at("message").as_object().at("reasoning_content").as_string()),
            },
            .finishReason = std::string(ResponseBody.at("choices").as_array()[0].at("finish_reason").as_string()),
            .raw = boost::json::parse(body)
        };
    } else
        return ProviderResponse{
        .id = std::string(ResponseBody.at("id").as_string()),
        .assistantMessage = {
            .role = MessageRole::Assistant,
            .content = std::string(ResponseBody.at("choices").as_array()[0].at("message").as_object().at("content").as_string())
        },
        .reasoningOutput = {},
        .finishReason = std::string(ResponseBody.at("choices").as_array()[0].at("finish_reason").as_string()),
        .raw = boost::json::parse(body)
    };
}

std::vector<StreamEvent> OpenAICompatibleProvider::parseStreamChunk(std::string_view chunk) {
    streamBuffer.append(chunk);
    std::vector<StreamEvent> events;

    while (true) {
        auto boundary = findSSEBoundary(streamBuffer);

        // 无边界直接？
        if (!boundary) {
            break;
        }

        std::string rawEvent = takeSSEEvent(streamBuffer, boundary.value());

        rawEvent.erase(0, 6);

        if (rawEvent == "[DONE]") {
            events.push_back({
                .type = StreamEvent::Type::Completed
            });
            continue;
        }

        boost::json::value jsonEvent;
        try {
            boost::system::error_code ec;
            jsonEvent = boost::json::parse(rawEvent, ec);
        } catch (std::bad_alloc const& e) {
            continue;
        }

        auto parsed = parseStreamJson(jsonEvent.as_object());
        events.push_back(parsed);
    }

    return events;
}

bool OpenAICompatibleProvider::enableCapability(const std::string capability) {
    if (capability == "streaming") {
        providerCapabilities.streaming = true;
        return true;
    }
    if (capability == "reasoning") {
        providerCapabilities.reasoning = true;
        return true;
    }
    return false;
}

bool OpenAICompatibleProvider::disableCapability(std::string capability) {
    if (capability == "streaming") {
        providerCapabilities.streaming = false;
        return true;
    }
    if (capability == "reasoning") {
        providerCapabilities.reasoning = false;
        return true;
    }
    return false;
}

bool OpenAICompatibleProvider::isStreaming() const {
    return providerCapabilities.streaming;
}

bool OpenAICompatibleProvider::isReasoning() const {
    return providerCapabilities.reasoning;
}

OpenAICompatibleProvider::~OpenAICompatibleProvider() = default;
