#include "HttpClient.h"
#include <iostream>
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


HttpResponseHead HttpClient::receiveStream(const BodyChunkHandler& onChunk) {
    if (protocol == "https") {
        return readStream(std::get<httpsStream>(stream), onChunk);
    }
    return readStream(std::get<httpStream>(stream), onChunk);
}

template<class Stream>
HttpResponseHead HttpClient::readStream(Stream& stream, const BodyChunkHandler& onChunk) {
    boost::beast::flat_buffer readBuffer;

    boost::beast::http::response_parser<boost::beast::http::buffer_body> parser;
    parser.body_limit(boost::none);
    boost::beast::http::read_header(stream,readBuffer,parser);

    HttpResponseHead head{
        .status = parser.get().result_int(),
        .headers = parser.get().base()
    };

    if (head.status < 200 || head.status >= 300) {
        // 错误响应通常需要完整读取后交给 Provider 解析
    }

    std::array<char, 4096> bodyBuffer{};

    while (!parser.is_done()) {
        parser.get().body().data =
            bodyBuffer.data();

        parser.get().body().size =
            bodyBuffer.size();

        boost::beast::error_code ec;

        boost::beast::http::read_some(stream,readBuffer,parser,ec);

        const std::size_t received = bodyBuffer.size() - parser.get().body().size;

        // 存在读取数据，构造 Chunk 由对应回调处理
        if (received > 0) {
            onChunk(std::string_view{bodyBuffer.data(),received});
        }

        if (ec ==boost::beast::http::error::need_buffer) {
            ec = {};
        }

        if (ec) {
            throw boost::beast::system_error{ec};
        }
    }

    return head;
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