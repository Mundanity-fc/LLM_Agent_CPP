#pragma once
#include "../components/ProviderConfig.h"
#include <string>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

class HttpClient {
public:
    explicit HttpClient(const ProviderConfig& config);
    ~HttpClient();

    void sendMessage();
    boost::beast::http::response<boost::beast::http::string_body> receiveMessage();

private:
    std::string host;
    std::string port;
    std::string target;
    std::string protocol;
    std::string apikey;
    boost::asio::io_context ioc;
    boost::beast::tcp_stream stream{ioc};
};
