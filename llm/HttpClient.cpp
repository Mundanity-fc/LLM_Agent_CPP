#include "HttpClient.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>

HttpClient::HttpClient(const ProviderConfig& config){
    host = config.host;
    port = config.port;
    target = config.target;
    protocol = config.protocol;
    apikey = config.apikey;
    boost::asio::ip::tcp::resolver resolver(ioc);

    if (protocol == "https") {
        sslContext.set_default_verify_paths();
        sslContext.set_verify_mode(boost::asio::ssl::verify_peer);
        auto& currentStream = stream.emplace<httpsStream>(ioc, sslContext);
        if (!SSL_set_tlsext_host_name(currentStream.native_handle(),host.c_str())) {
            const auto errorCode = static_cast<int>(::ERR_get_error());
            throw boost::beast::system_error{
                errorCode,
                boost::asio::error::get_ssl_category(),
                "Failed to set SNI hostname"
            };
        }
        currentStream.set_verify_callback(boost::asio::ssl::host_name_verification{host});
        const auto endpoints = resolver.resolve(host, port);
        boost::beast::get_lowest_layer(currentStream).expires_after(std::chrono::seconds(30));
        boost::beast::get_lowest_layer(currentStream).connect(endpoints);
        currentStream.handshake(boost::asio::ssl::stream_base::client);
    } else {
        auto& currentStream = stream.emplace<httpStream>(ioc);
        currentStream.expires_after(std::chrono::seconds(30));
        const auto endpoints = resolver.resolve(host, port);
        currentStream.connect(endpoints);
    }
}

void HttpClient::sendMessage(const boost::json::object& requestBody) {
    boost::beast::http::request<boost::beast::http::string_body> request{
        boost::beast::http::verb::post,
        this->target,
        11
    };
    request.set(boost::beast::http::field::host, host);
    request.set(boost::beast::http::field::content_type, "application/json");
    request.set(boost::beast::http::field::authorization, "Bearer "+this->apikey);
    request.set(boost::beast::http::field::accept, "application/json");
    request.body() = boost::json::serialize(requestBody);
    request.prepare_payload();
    if (protocol == "https") {
        auto& currentStream = std::get<httpsStream>(stream);
        boost::beast::http::write(currentStream, request);
    }else {
        auto& currentStream = std::get<httpStream>(stream);
        boost::beast::http::write(currentStream, request);
    }
}

boost::beast::http::response<boost::beast::http::string_body> HttpClient::receiveMessage() {
    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::string_body> response;
    if (protocol == "https") {
        auto& currentStream = std::get<httpsStream>(stream);
        boost::beast::http::read(currentStream, buffer, response);
    }else {
        auto& currentStream = std::get<httpStream>(stream);
        boost::beast::http::read(currentStream, buffer, response);
    }
    return response;
}

HttpClient::~HttpClient() {
    boost::beast::error_code ec;
    if (protocol == "https") {
        auto& currentSteam = std::get<httpsStream>(stream);
        currentSteam.shutdown(ec);
    } else {
        auto& currentSteam = std::get<httpStream>(stream);
        currentSteam.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    }
}