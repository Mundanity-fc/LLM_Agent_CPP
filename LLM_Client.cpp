#include "LLM_Client.h"

#include <utility>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <openssl/err.h>
#include <openssl/ssl.h>

LLM_Client::LLM_Client(std::string model, std::string host, std::string port, std::string target, std::string apikey)
    : model_(std::move(model)),
      host_(std::move(host)),
      port_(std::move(port)),
      target_(std::move(target)),
      apikey_(std::move(apikey)),
      ssl_ctx_(boost::asio::ssl::context::tls_client),
      stream_(ioc_, ssl_ctx_) {
    ssl_ctx_.set_default_verify_paths();
    // Keep the original behavior for now; can be switched to verify_peer later.
    ssl_ctx_.set_verify_mode(boost::asio::ssl::verify_none);
}

void LLM_Client::connect() {
    if (connected_) {
        return;
    }

    if (!SSL_set_tlsext_host_name(stream_.native_handle(), host_.c_str())) {
        throw boost::beast::system_error(
            boost::beast::error_code(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()),
            "Failed to set SNI Hostname");
    }

    boost::asio::ip::tcp::resolver resolver{ioc_};
    auto const results = resolver.resolve(host_, port_);
    boost::beast::get_lowest_layer(stream_).connect(results);
    stream_.handshake(boost::asio::ssl::stream_base::client);
    connected_ = true;
}

void LLM_Client::shutdown() {
    if (!connected_) {
        return;
    }

    boost::beast::error_code ec;
    stream_.shutdown(ec);

    // Common for HTTP servers that close TCP without TLS close_notify.
    if (ec == boost::asio::error::eof || ec == boost::asio::ssl::error::stream_truncated) {
        ec = {};
    }

    connected_ = false;
    if (ec) {
        throw boost::beast::system_error{ec, "shutdown"};
    }
}

void LLM_Client::switchProvider(std::string model, std::string host, std::string port, std::string target, std::string apikey) {
    this->model_ = std::move(model);
    this->host_ = std::move(host);
    this->port_ = std::move(port);
    this->target_ = std::move(target);
    this->apikey_ = std::move(apikey);
}

boost::beast::ssl_stream<boost::beast::tcp_stream> &LLM_Client::stream() {
    return stream_;
}

const std::string &LLM_Client::host() const {
    return host_;
}

const std::string &LLM_Client::target() const {
    return target_;
}

const std::string &LLM_Client::apikey() const {
    return apikey_;
}

void LLM_Client::sendMessage(const boost::json::array& message) {
    this->create_request_head();
    boost::json::object body;
    body["model"] = this->model_;
    body["messages"] = message;
    body["stream"] = true;
    this->request_.body() = boost::json::serialize(body);
    this->request_.prepare_payload();
    boost::beast::http::write(this->stream_, this->request_);
}

std::string LLM_Client::getResponse() {
    // 完整的回复内容
    std::string total_response;

    // 接收缓存
    boost::beast::flat_buffer buffer;
    // 使用 buffer_body 允许我们提供自己的缓冲区进行增量读取
    boost::beast::http::response_parser<boost::beast::http::buffer_body> parser;
    // 流式响应可能非常大，取消默认的body大小限制
    parser.body_limit(boost::none);
    // 首先只读取HTTP头部
    boost::beast::http::read_header(this->stream_, buffer, parser);
    // 循环读取数据块
    char body_buf[4096];
    boost::beast::error_code ec;
    std::string accumulator;
    bool done = false;
    while(!parser.is_done() && !done) {
        // 绑定当前读取使用的缓冲区
        parser.get().body().data = body_buf;
        parser.get().body().size = sizeof(body_buf);
        // 读取一块数据
        boost::beast::http::read(this->stream_, buffer, parser, ec);
        // need_buffer是正常的，表示提供的缓冲区body_buf已经写满，需要我们处理后再次读取
        if(ec == boost::beast::http::error::need_buffer) {
            ec = {};
        } else if(ec) {
            throw boost::beast::system_error{ec};
        }
        // 计算本次实际读取到了多少字节
        size_t bytes_transferred = sizeof(body_buf) - parser.get().body().size;
        if (bytes_transferred > 0) {
            // 将新到达的碎片数据追加到累加器中
            accumulator.append(body_buf, bytes_transferred);
            // 循环处理累加器中的完整消息
            size_t pos;
            // SSE规范中，一个完整的事件以双换行符结束
            while (!done && (pos = accumulator.find("\n\n")) != std::string::npos) {
                std::string sse_event_block = accumulator.substr(0, pos);
                accumulator.erase(0, pos + 2);

                // 逐行解析SSE事件块，兼容注释行、event/id行等非标准内容
                size_t line_start = 0;
                while (line_start < sse_event_block.size()) {
                    size_t line_end = sse_event_block.find('\n', line_start);
                    if (line_end == std::string::npos) {
                        line_end = sse_event_block.size();
                    }
                    std::string line = sse_event_block.substr(line_start, line_end - line_start);
                    // 去除行尾可能存在的\r（兼容\r\n换行符）
                    if (!line.empty() && line.back() == '\r') {
                        line.pop_back();
                    }
                    line_start = line_end + 1;

                    // 跳过SSE注释行（如 ": OPENROUTER PROCESSING"）及空行
                    if (line.empty() || line.front() == ':') {
                        continue;
                    }
                    // 跳过event/id等非data字段行
                    if (!line.starts_with("data:")) {
                        continue;
                    }

                    // 提取data字段值，兼容"data:"和"data: "两种格式
                    constexpr size_t DATA_PREFIX_LEN = 5; // length of "data:"
                    std::string data = line.substr(DATA_PREFIX_LEN);
                    if (!data.empty() && data.front() == ' ') {
                        data.erase(0, 1);
                    }

                    if (data == "[DONE]") {
                        done = true;
                        break;
                    }

                    // 解析JSON并提取增量文本内容
                    boost::json::error_code json_ec;
                    boost::json::value json_body = boost::json::parse(data, json_ec);
                    if (json_ec) {
                        continue;
                    }
                    const auto* choices = json_body.as_object().if_contains("choices");
                    if (!choices || !choices->is_array() || choices->as_array().empty()) {
                        continue;
                    }
                    const auto* delta = choices->as_array().at(0).as_object().if_contains("delta");
                    if (!delta || !delta->is_object()) {
                        continue;
                    }
                    const auto* content = delta->as_object().if_contains("content");
                    if (!content || !content->is_string()) {
                        continue;
                    }
                    std::string response_content = content->as_string().c_str();
                    // 文本流式输出
                    std::cout << response_content << std::flush;
                    // 返回文本拼接
                    total_response += response_content;
                }
            }
        }
    }
    return total_response;
}

void LLM_Client::create_request_head() {
    this->request_ = boost::beast::http::request<boost::beast::http::string_body> {boost::beast::http::verb::post, this->target_, 11};
    this->request_.set(boost::beast::http::field::host, this->host_);
    this->request_.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    this->request_.set(boost::beast::http::field::content_type, "application/json");
    this->request_.set(boost::beast::http::field::authorization, "Bearer " + this->apikey_);
}
