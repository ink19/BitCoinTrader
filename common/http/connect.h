#ifndef __COMMON_HTTP_CONNECT_H__
#define __COMMON_HTTP_CONNECT_H__

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>
#include <string>

namespace asio = boost::asio;

namespace Common {

class Connect {
 public:
  Connect() = default;
  Connect(const std::string &domain, const int port);
  asio::awaitable<std::unique_ptr<asio::ip::tcp::socket>> connect();
  asio::awaitable<std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket>>> connect_ssl();

  asio::awaitable<std::unique_ptr<asio::ip::tcp::socket>> operator()() {
    co_return co_await connect();
  }
 private:
  asio::awaitable<void> connect_base(asio::ip::tcp::socket &socket);

  std::string m_domain;
  int m_port;
};

class ConnectSSL : public Connect {
 public:
  ConnectSSL() = default;
  ConnectSSL(const std::string &domain, const int port) : Connect(domain, port){};
  asio::awaitable<std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket>>> operator()() {
    co_return co_await connect_ssl();
  }
};

}  // namespace Common
#endif