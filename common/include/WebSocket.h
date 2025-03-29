#ifndef __COMMON_WEBSOCKET_H
#define __COMMON_WEBSOCKET_H

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/execution_context.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <string>

namespace Common {

namespace asio = boost::asio;
namespace beast = boost::beast;

class WebSocket {
 public:
  WebSocket();
  WebSocket(const std::string &uri);
  int add_uri(const std::string &uri);

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

  virtual asio::awaitable<void> connect() { co_return; }

  asio::awaitable<std::string> read();
  asio::awaitable<void> write(const std::string &msg);
  asio::awaitable<void> close();

 protected:
  const std::string m_host;
  const int m_port;
  const std::string m_path;
  std::unique_ptr<WsSocketType> m_ws;
};

class WebSocketDetailWS : public WebSocketDetail<beast::websocket::stream<asio::ip::tcp::socket>> {
 public:
  WebSocketDetailWS(const std::string &host, int port, const std::string &path)
      : WebSocketDetail<beast::websocket::stream<asio::ip::tcp::socket>>(host, port, path){};
  asio::awaitable<void> connect() override;
};

class WebSocketDetailWSS : public WebSocketDetail<beast::websocket::stream<asio::ssl::stream<asio::ip::tcp::socket>>> {
 public:
  WebSocketDetailWSS(const std::string &host, int port, const std::string &path)
      : WebSocketDetail<beast::websocket::stream<asio::ssl::stream<asio::ip::tcp::socket>>>(host, port, path){};
  asio::awaitable<void> connect() override;
};

}  // namespace Common

#endif
