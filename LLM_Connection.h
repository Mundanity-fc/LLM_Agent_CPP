#include <string>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>
#include <openssl/err.h>


class LLM_Connection {
private:
    std::string host;
    std::string port;
    std::string target;
    boost::asio::io_context ioc;
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream;
    boost::beast::http::request<boost::beast::http::string_body> request;
    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> response;
public:
    bool establish_connection();
    ~LLM_Connection();
};
