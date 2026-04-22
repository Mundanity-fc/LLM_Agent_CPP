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
    std::string apikey;
    boost::asio::io_context ioc;
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream;

public:
    LLM_Connection(const std::string &host, const std::string &port, const std::string &target,
                   const std::string &apikey) : host(host),
                                                port(port), target(target), apikey(apikey) {
    }

    bool establish_connection();

    ~LLM_Connection();
};
