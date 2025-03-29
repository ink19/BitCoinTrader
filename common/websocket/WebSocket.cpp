#include "WebSocket.h"
#include <boost/asio/executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/url.hpp>
#include <boost/url/parse.hpp>
#include <glog/logging.h>
#include "errcode.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
using tcp = boost::asio::ip::tcp;

namespace Common {

WebSocket::WebSocket() {}

WebSocket::WebSocket(const std::string& uri) {
    this->add_uri(uri);
}

int WebSocket::add_uri(const std::string &uri) {
    auto parsedURI = boost::urls::parse_uri(uri);
    if (parsedURI.has_error()) {
        return ErrCode_Invalid_Param;
    }

    if (parsedURI->scheme() == "wss") {
        m_is_ssl = true;
    }
    m_host = parsedURI->host();
    m_port = parsedURI->port();
    m_path = parsedURI->path();
    return ErrCode_OK;
}

asio::awaitable<int> WebSocket::connent(asio::io_context& ctx) {
    if (m_host.empty() || m_port.empty()) {
        co_return ErrCode_Invalid_Param;
    }
    auto execution = co_await asio::this_coro::executor;
    tcp::resolver resolver(ctx);
    auto points = co_await resolver.async_resolve(m_host, m_port, asio::use_awaitable);

    co_return 0;
}

}
