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
#include "connect.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
using tcp = boost::asio::ip::tcp;

namespace Common {

WebSocket::WebSocket() {}

WebSocket::WebSocket(const std::string &uri) { this->add_uri(uri); }

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

asio::awaitable<void> WebSocket::connect() {
  if (m_is_ssl) {
    m_ws_detail = std::make_unique<WebSocketDetailWSS>(m_host, m_port, m_path);
  } else {
    m_ws_detail = std::make_unique<WebSocketDetailWS>(m_host, m_port, m_path);
  }
  co_await m_ws_detail->connect();
  co_return;
}

asio::awaitable<std::string> WebSocket::read() {
  co_return co_await m_ws_detail->read();
}

asio::awaitable<void> WebSocket::write(const std::string &msg) {
  co_await m_ws_detail->write(msg);
  co_return;
}

asio::awaitable<void> WebSocket::close() {
  co_await m_ws_detail->close();
  co_return;
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

asio::awaitable<void> WebSocketDetailWS::connect() {
  auto base_socket = co_await Connect(this->m_host, this->m_port)();

  this->m_ws = std::make_unique<beast::websocket::stream<asio::ip::tcp::socket>>(std::move(*base_socket));
  co_await this->m_ws->async_handshake(this->m_host, this->m_path, boost::asio::use_awaitable);

  co_return;
}

asio::awaitable<void> WebSocketDetailWSS::connect() {
  auto base_socket = co_await ConnectSSL(this->m_host, this->m_port)();

  this->m_ws = std::make_unique<beast::websocket::stream<asio::ssl::stream<asio::ip::tcp::socket>>>(std::move(*base_socket));

  co_await this->m_ws->async_handshake(this->m_host, this->m_path, boost::asio::use_awaitable);

  co_return;
}

}  // namespace Common
