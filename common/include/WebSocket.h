#ifndef __COMMON_WEBSOCKET_H
#define __COMMON_WEBSOCKET_H

#include <boost/asio/execution_context.hpp>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>

namespace Common {

namespace asio = boost::asio;

class WebSocket {
public:
    WebSocket();
    WebSocket(const std::string& uri);

    int add_uri(const std::string& uri);
    asio::awaitable<int> connent(asio::io_context& ctx);
private:
    bool m_is_ssl;
    std::string m_host;
    std::string m_port;
    std::string m_path;
};

}

#endif
