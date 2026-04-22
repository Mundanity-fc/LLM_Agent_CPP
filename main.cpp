#include <iostream>
#include <fstream>
#include <string>
#include <boost/json.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>
#include <openssl/err.h>


int main() {
    std::fstream env_config("./env.json", std::ios::in);
    std::string content;
    if(env_config.good()) {
        content = std::string ((std::istreambuf_iterator<char>(env_config)), std::istreambuf_iterator<char>());
    }
    env_config.close();
    boost::json::value config = boost::json::parse(content);
    boost::json::array providers = config.at("providers").as_array();
    boost::json::object deepkseek = providers[0].as_object();
#ifdef __WIN32__
    SetConsoleOutputCP(65001);
#endif
    try {
        // 1. 设置必要的上下文
        const std::string host = deepkseek.at("host").as_string().c_str();
        const std::string port = deepkseek.at("port").as_string().c_str();
        const std::string target = deepkseek.at("target").as_string().c_str();
        const std::string apikey = deepkseek.at("apikey").as_string().c_str();
        boost::asio::io_context ioc;

        // 1. SSL 上下文初始化 (TLS 客户端)
        boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12_client};
        // 如果不需要验证证书（仅用于测试），可以注释掉或配置证书路径
        ctx.set_default_verify_paths();
        ctx.set_verify_mode(boost::asio::ssl::verify_none);
        boost::asio::ip::tcp::resolver resolver{ioc};
        // 2. 创建 SSL 流
        boost::beast::ssl_stream<boost::beast::tcp_stream> stream{ioc, ctx};
        // 设置 SNI（服务器名称指示），对于现代 Web 服务是必须的
        if(! SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
            throw boost::beast::system_error(
                boost::beast::error_code(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()),
                "Failed to set SNI Hostname");
        }
        // 3. 连接与握手
        auto const results = resolver.resolve(host, port);
        boost::beast::get_lowest_layer(stream).connect(results);
        stream.handshake(boost::asio::ssl::stream_base::client);

        // 3. 构建 HTTP POST 请求
        boost::beast::http::request<boost::beast::http::string_body> req{boost::beast::http::verb::post, target, 11};
        req.set(boost::beast::http::field::host, host);
        req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(boost::beast::http::field::content_type, "application/json");
        req.set(boost::beast::http::field::authorization, "Bearer "+apikey);

        // 设置请求体数据内容
        req.body() = R"({
            "model": "deepseek-chat",
            "messages": [
                {"role": "system", "content": "你是一个AI助理"},
                {"role": "user", "content": "输出一些你的基本信息，由于你的输出内容显示在命令行，因此请不要使用Markdown标记"}
            ],
            "stream": true
        })";
        req.prepare_payload(); // 自动计算 Content-Length

        // 4. 发送请求
        boost::beast::http::write(stream, req);

        // 5. 接收响应
        boost::beast::flat_buffer buffer;
        // 使用 buffer_body 允许我们提供自己的缓冲区进行增量读取
        boost::beast::http::response_parser<boost::beast::http::buffer_body> parser;
        // 流式响应可能非常大，取消默认的 body 大小限制
        parser.body_limit(boost::none);
        // 首先只读取 HTTP 头部
        boost::beast::http::read_header(stream, buffer, parser);

        std::cout << "--- Stream Started (HTTP " << parser.get().result_int() << ") ---" << std::endl;
        // 循环读取数据块
        char body_buf[4096];
        boost::beast::error_code ec;
        std::string accumulator;
        std::string total_response = "";
        while(!parser.is_done()) {

            // 绑定当前读取使用的缓冲区
            parser.get().body().data = body_buf;
            parser.get().body().size = sizeof(body_buf);
            // 读取一块数据
            boost::beast::http::read(stream, buffer, parser, ec);
            // need_buffer 是正常的：表示提供的缓冲区 body_buf 已经写满，需要我们处理后再次读取
            if(ec == boost::beast::http::error::need_buffer) {
                ec = {};
            } else if(ec) {
                throw boost::beast::system_error{ec};
            }
            // 计算本次实际读取到了多少字节
            size_t bytes_transferred = sizeof(body_buf) - parser.get().body().size;
            if (bytes_transferred > 0) {
                // 1. 将新到达的碎片数据追加到累加器中
                accumulator.append(body_buf, bytes_transferred);
                // 2. 循环处理累加器中的完整消息（处理 SSE 协议的 \n\n 边界）
                size_t pos;
                // SSE 规范中，一个完整的事件以双换行符结束
                while ((pos = accumulator.find("\n\n")) != std::string::npos) {
                    std::string sse_event = accumulator.substr(0, pos).erase(0, 6);// 删除data前缀
                    if (sse_event == "[DONE]") {
                        break;
                    }
                    accumulator.erase(0, pos + 2);
                    std::string response_content;
                    {
                        boost::json::value json_body = boost::json::parse(sse_event);
                        boost::json::array choices = json_body.at("choices").as_array();
                        boost::json::object first_choice = choices.at(0).as_object().at("delta").as_object();
                        response_content = first_choice.at("content").as_string().c_str();
                    }
                    std::cout << response_content << std::flush;
                    total_response += response_content;
                }
            }
        }
        std::cout << "\n--- Stream Finished ---" << std::endl;
        std::cout << total_response << std::endl;

        // 6. 优雅地关闭连接
        auto shutdown_ec = stream.shutdown(ec);
        if (shutdown_ec == boost::asio::error::eof || shutdown_ec == boost::asio::ssl::error::stream_truncated) {
            shutdown_ec = {};
        }
        if (shutdown_ec) {
            throw boost::beast::system_error{shutdown_ec, "shutdown"};
        }
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}