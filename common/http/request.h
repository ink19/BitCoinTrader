#ifndef __COMM_REQUEST_H__
#define __COMM_REQUEST_H__

#include <boost/beast/http/message_fwd.hpp>
#include <memory>
#include <string>
#include <map>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/beast.hpp>

#include "connect.h"

namespace Common {

#define UA "BitCoainTrader/0.1 beta"

namespace asio = boost::asio;
namespace http = boost::beast::http;
namespace beast = boost::beast;

class HttpRequest {
  public:
    HttpRequest() = default;
    HttpRequest(const std::string &url, const std::string &method = "GET", const std::string &body = "");
    int set_url(const std::string &url);
    int set_method(const std::string &method);
    int set_body(const std::string &content_type, const std::string &body);
    int set_header(const std::string &header_name, const std::string &header_value);
    int set_header(const std::map<std::string, std::string> &headers);

    asio::awaitable<std::string> request();

  private:
    std::string m_url;
    std::string m_method;
    std::string m_body;
    std::string m_content_type;
    std::map<std::string, std::string> m_headers;

    template<typename SocketType>
    asio::awaitable<std::string> do_request(std::unique_ptr<SocketType> conn, const http::request<http::string_body> &req) {
      co_await http::async_write(*conn, req, asio::use_awaitable);
      beast::flat_buffer buffer;
      http::response<http::string_body> res;
      co_await http::async_read(*conn, buffer, res, asio::use_awaitable);
      if (res.result() != http::status::ok) {
        throw std::runtime_error("HTTP request failed: " + std::to_string(res.result_int()));
      }
      std::string response_body = res.body();
      co_return response_body;
    }
};

}

#endif