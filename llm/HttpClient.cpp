#include "HttpClient.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

HttpClient::HttpClient(const ProviderConfig& config){
    host = config.host;
    port = config.port;
    target = config.target;
    protocol = config.protocol;
    apikey = config.apikey;

    boost::asio::ip::tcp::resolver resolver(ioc);
    const auto endpoints = resolver.resolve(host, port);

    stream.expires_after(std::chrono::seconds(30));
    stream.connect(endpoints);
}

void HttpClient::sendMessage() {
    boost::beast::http::request<boost::beast::http::string_body> request{
        boost::beast::http::verb::post,
        this->target,
        11
    };
    request.set(boost::beast::http::field::host, host);
    request.set(boost::beast::http::field::content_type, "application/json");
    request.set(boost::beast::http::field::authorization, "Bearer "+this->apikey);
    request.set(boost::beast::http::field::accept, "application/json");
    request.body() = R"({"model": "/home/mundanity/LLMs/Qwen3.5-2B","messages": [{"role": "user", "content": "介绍一下你自己"}]})";
    // Set Payload Size
    request.prepare_payload();
    boost::beast::http::write(stream, request);
}

boost::beast::http::response<boost::beast::http::string_body> HttpClient::receiveMessage() {
    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::string_body> response;
    boost::beast::http::read(stream, buffer, response);
    return response;
}

HttpClient::~HttpClient() {
    boost::beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
}