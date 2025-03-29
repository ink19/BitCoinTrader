#ifndef __COMMON_WEBSOCKET_H
#define __COMMON_WEBSOCKET_H

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/execution_context.hpp>
#include <boost/beast.hpp>
#include <string>
#include <boost/beast/ssl.hpp>

namespace Common {

namespace asio = boost::asio;
namespace beast = boost::beast;

class WebSocket {
public:
  WebSocket();
  WebSocket(const std::string& uri);
  int add_uri(const std::string& uri);

private:
  bool m_is_ssl;
  std::string m_host;
  int m_port;
  std::string m_path;
};

template <typename WsSocketType>
class WebSocketDetail {
 public:
  WebSocketDetail(const std::string &host, int port, const std::string &path)
      : m_host(host), m_port(port), m_path(path){};
  ~WebSocketDetail() {}

  virtual asio::awaitable<int> connect() { co_return 0; }

  asio::awaitable<std::string> read();
  asio::awaitable<void> write(const std::string &msg);
  asio::awaitable<void> close();

 protected:
  const std::string m_host;
  const int m_port;
  const std::string m_path;
  std::unique_ptr<WsSocketType> m_ws;
};

template <bool ssl>
class WebSocketDetailImpl;

template <>
class WebSocketDetailImpl<false> : public WebSocketDetail<beast::websocket::stream<asio::ip::tcp::socket>> {
 public:
  WebSocketDetailImpl(const std::string &host, int port, const std::string &path)
      : WebSocketDetail<beast::websocket::stream<asio::ip::tcp::socket>>(host, port, path){};
  asio::awaitable<int> connect() override;
};

template <>
class WebSocketDetailImpl<true> : public WebSocketDetail<beast::websocket::stream<asio::ssl::stream<asio::ip::tcp::socket>>> {
public:
  WebSocketDetailImpl(const std::string &host, int port, const std::string &path)
      : WebSocketDetail<beast::websocket::stream<asio::ssl::stream<asio::ip::tcp::socket>>>(host, port, path) {};
  asio::awaitable<int> connect() override;
};

}  // namespace Common

#endif
