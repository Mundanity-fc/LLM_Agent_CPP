#include <iostream>
#include <string>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>
#include <openssl/err.h>


namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;

int main() {
#ifdef __WIN32__
    SetConsoleOutputCP(65001);
#endif
    try {
        // 1. 设置必要的上下文
        const std::string host = "api.deepseek.com";
        const std::string port = "443";
        const std::string target = "/chat/completions";

        net::io_context ioc;

        // 1. SSL 上下文初始化 (TLS 客户端)
        ssl::context ctx{ssl::context::tlsv12_client};
        // 如果不需要验证证书（仅用于测试），可以注释掉或配置证书路径
        ctx.set_default_verify_paths();
        ctx.set_verify_mode(ssl::verify_none);
        tcp::resolver resolver{ioc};
        // 2. 创建 SSL 流
        beast::ssl_stream<beast::tcp_stream> stream{ioc, ctx};
        // 设置 SNI（服务器名称指示），对于现代 Web 服务是必须的
        if(! SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
            throw beast::system_error(
                beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()),
                "Failed to set SNI Hostname");
        }
        // 3. 连接与握手
        auto const results = resolver.resolve(host, port);
        beast::get_lowest_layer(stream).connect(results);
        stream.handshake(ssl::stream_base::client);

        // 3. 构建 HTTP POST 请求
        http::request<http::string_body> req{http::verb::post, target, 11};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");
        req.set(http::field::authorization, "Bearer {$APIKEY}");

        // 设置请求体数据内容
        req.body() = R"({
            "model": "deepseek-chat",
            "messages": [
                {"role": "system", "content": "你是一个AI助理"},
                {"role": "user", "content": "输出一些你的基本信息"}
            ],
            "stream": false
        })";
        req.prepare_payload(); // 自动计算 Content-Length

        // 4. 发送请求
        http::write(stream, req);

        // 5. 接收响应
        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(stream, buffer, res);

        std::string body_str = boost::beast::buffers_to_string(res.body().data());
        // 输出响应结果
        std::cout << body_str << std::endl;

        // 6. 优雅地关闭连接
        boost::beast::error_code ec;
        auto shutdown_ec = stream.shutdown(ec);
        if (shutdown_ec == net::error::eof || shutdown_ec == ssl::error::stream_truncated) {
            shutdown_ec = {};
        }
        if (shutdown_ec) {
            throw beast::system_error{shutdown_ec, "shutdown"};
        }
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}