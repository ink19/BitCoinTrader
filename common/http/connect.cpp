#include "connect.h"

#include <boost/system.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>
#include <glog/logging.h>

namespace Common {

Connect::Connect(const std::string &domain, const int port) : m_domain(domain), m_port(port) {}

asio::awaitable<std::unique_ptr<asio::ip::tcp::socket>> Connect::connect() {
  auto executor = co_await asio::this_coro::executor;
  auto socket = std::make_unique<asio::ip::tcp::socket>(executor);
  
  co_await connect_base(*socket);
  co_return socket;
}

asio::awaitable<std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket>>> Connect::connect_ssl() {
  auto executor = co_await asio::this_coro::executor;
  asio::ssl::context ssl_ctx(asio::ssl::context::tlsv13_client);
  ssl_ctx.set_options(asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2 | asio::ssl::context::single_dh_use | asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3);

  auto socket = std::make_unique<asio::ssl::stream<asio::ip::tcp::socket>>(executor, ssl_ctx);
  co_await connect_base(socket->next_layer());
  if (!SSL_set_tlsext_host_name(socket->native_handle(), m_domain.c_str())) {
    throw boost::system::system_error(
        boost::system::error_code(static_cast<int>(::ERR_get_error()), asio::error::get_ssl_category()),
        "Unable to set SNI hostname");
  }
  
  LOG(INFO) << "Performing SSL handshake";
  co_await socket->async_handshake(asio::ssl::stream_base::client, asio::use_awaitable);
  co_return socket;
}

asio::awaitable<void> Connect::connect_base(asio::ip::tcp::socket &socket) {
  auto executor = co_await asio::this_coro::executor;
  asio::ip::tcp::resolver resolver(executor);

  LOG(INFO) << "Resolving " << m_domain << ":" << m_port;

  auto points = co_await resolver.async_resolve(m_domain, std::to_string(m_port), asio::use_awaitable);

  LOG(INFO) << "Resolved";

  if (points.empty()) {
    throw boost::system::system_error(
        boost::system::error_code(static_cast<int>(::ERR_get_error()), asio::error::get_ssl_category()),
        "Unable to get address");
  }

  LOG(INFO) << "Connecting to " << points.begin()->endpoint();

  co_await asio::async_connect(socket, points, asio::use_awaitable);
}

}