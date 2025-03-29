#include "WebSocket.h"

#include <glog/logging.h>

#include <boost/asio.hpp>
#include <boost/asio/executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/url.hpp>
#include <boost/url/parse.hpp>

#include "errcode.h"

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
    m_port = parsedURI->port_number();
    m_path = parsedURI->path();
    return ErrCode_OK;
}

template <typename WsSocketType>
asio::awaitable<std::string> WebSocketDetail<WsSocketType>::read() {
  auto executor = co_await asio::this_coro::executor;
  beast::flat_buffer buffer;
  co_await m_ws->async_read(buffer, asio::use_awaitable);
  co_return std::string((char *)buffer.data().data(), buffer.data().size());
}

template <typename WsSocketType>
asio::awaitable<void> WebSocketDetail<WsSocketType>::write(const std::string &msg) {
  auto executor = co_await asio::this_coro::executor;
  co_await m_ws->async_write(asio::buffer(msg), asio::use_awaitable);
  co_return;
}

template <typename WsSocketType>
asio::awaitable<void> WebSocketDetail<WsSocketType>::close() {
  auto executor = co_await asio::this_coro::executor;
  co_await m_ws->async_close(beast::websocket::close_code::normal, boost::asio::use_awaitable);
  co_return;
}

asio::awaitable<int> WebSocketDetailImpl<false>::connect() {
  auto executor = co_await boost::asio::this_coro::executor;
  boost::asio::ip::tcp::resolver resolver(executor);
  auto points = co_await resolver.async_resolve(this->m_host, std::to_string(this->m_port), boost::asio::use_awaitable);

  typeof(points.begin()) point_iter;
  if (!points.empty()) {
    co_return ErrCode_Resolve_Fail;
  }
  point_iter = points.begin();

  tcp::socket socket(executor);
  co_await socket.async_connect(*point_iter, boost::asio::use_awaitable);
  this->m_ws = std::make_unique<beast::websocket::stream<asio::ip::tcp::socket>>(std::move(socket));
  co_await this->m_ws->async_handshake(this->m_host, this->m_path, boost::asio::use_awaitable);

  co_return ErrCode_OK;
}

asio::awaitable<int> WebSocketDetailImpl<true>::connect() {
  auto executor = co_await boost::asio::this_coro::executor;
  boost::asio::ip::tcp::resolver resolver(executor);
  auto points = co_await resolver.async_resolve(this->m_host, std::to_string(this->m_port), boost::asio::use_awaitable);

  asio::ssl::context ssl_ctx(asio::ssl::context::tlsv13);
  asio::ssl::stream<asio::ip::tcp::socket> socket(executor, ssl_ctx);

  typeof(points.begin()) point_iter;
  if (!points.empty()) {
    co_return ErrCode_Resolve_Fail;
  }
  point_iter = points.begin();

  co_await socket.lowest_layer().async_connect(*point_iter, boost::asio::use_awaitable);

  if (!SSL_set_tlsext_host_name(socket.native_handle(), m_host.c_str())) {
    throw boost::beast::system_error(
        boost::beast::error_code(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()),
        "Unable to set SNI hostname");
  }
  co_await socket.async_handshake(asio::ssl::stream_base::client, boost::asio::use_awaitable);

  this->m_ws = std::make_unique<beast::websocket::stream<asio::ssl::stream<asio::ip::tcp::socket>>>(std::move(socket));

  co_await this->m_ws->async_handshake(this->m_host, this->m_path, boost::asio::use_awaitable);
  
  co_return ErrCode_OK;
}

}  // namespace Common
