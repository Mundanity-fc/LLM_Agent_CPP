#pragma once

#include <iostream>
#include <string>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>

class LLM_Client {
public:
    LLM_Client(std::string host, std::string port, std::string target, std::string apikey);

    void connect();

    void shutdown();

    void switchProvider(std::string host, std::string port, std::string target, std::string apikey);

    boost::beast::ssl_stream<boost::beast::tcp_stream> &stream();

    [[nodiscard]] const std::string &host() const;

    [[nodiscard]] const std::string &target() const;

    [[nodiscard]] const std::string &apikey() const;

    void sentMessage(const boost::json::array& message);

    std::string getResponse();

private:
    std::string host_;
    std::string port_;
    std::string target_;
    std::string apikey_;

    boost::asio::io_context ioc_;
    boost::asio::ssl::context ssl_ctx_;
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
    boost::beast::http::request<boost::beast::http::string_body> request_;
    bool connected_{false};

    void create_request_head();
};