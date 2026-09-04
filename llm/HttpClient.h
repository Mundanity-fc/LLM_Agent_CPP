#pragma once
#include "../Components/ProviderComponents/ProviderComponents.h"
#include <string>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>

struct HttpResponseHead {
    unsigned status{0};
    boost::beast::http::fields headers;
};

using BodyChunkHandler = std::function<void(std::string_view)>;


class HttpClient {
public:
    explicit HttpClient(const ProviderConfig& config);
    ~HttpClient();

    void sendMessage(const boost::json::object& requestBody);
    boost::beast::http::response<boost::beast::http::string_body> receiveMessage();
    void receiveSSE();

    HttpResponseHead receiveStream(const BodyChunkHandler& onChunk);

private:
    std::string host;
    std::string port;
    std::string target;
    std::string protocol;
    std::string apikey;
    boost::asio::io_context ioc;
    boost::asio::ssl::context sslContext{boost::asio::ssl::context::tls_client};
    typedef boost::beast::tcp_stream httpStream;
    typedef boost::beast::ssl_stream<boost::beast::tcp_stream> httpsStream;
    std::variant<std::monostate, httpStream, httpsStream> stream;
    //
    template<class Stream>
    HttpResponseHead readStream(Stream& stream, const BodyChunkHandler& onChunk);
};
