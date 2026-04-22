#include "LLM_Connection.h"

LLM_Connection::LLM_Connection(const std::string &host, const std::string &port, const std::string &target,
                               const std::string &apikey) : host(host), port(port), target(target), apikey(apikey) {
    // 设置SSL Context
    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12_client};
    // 设置默认证书验证路径
    ctx.set_default_verify_paths();
    // 设置SSL证书验证模式
    ctx.set_verify_mode(boost::asio::ssl::verify_client_once);

    // 定义TCP解析器
    boost::asio::ip::tcp::resolver resolver{ioc};

    // 创建SSL流
    stream = boost::beast::ssl_stream<boost::beast::tcp_stream>{ioc, ctx};
    // 设置SNI
    if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
        throw boost::beast::system_error(
            boost::beast::error_code(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()),
            "Failed to set SNI Hostname");
    }
    // 连接与握手
    auto const results = resolver.resolve(host, port);
    boost::beast::get_lowest_layer(stream).connect(results);
    stream.handshake(boost::asio::ssl::stream_base::client);
}
