
#include "request.h"

#include <boost/beast/http/string_body_fwd.hpp>
#include <boost/url/parse.hpp>
#include <boost/system.hpp>
#include <glog/logging.h>
#include <boost/beast.hpp>

namespace Common {

namespace http = boost::beast::http;

HttpRequest::HttpRequest(const std::string &url, const std::string &method, const std::string &body)
    : m_method(method), m_body(body), m_url(url) {
}

int HttpRequest::set_url(const std::string &url) {
  m_url = url;
  return 0;
}

int HttpRequest::set_method(const std::string &method) {
  m_method = method;
  return 0;
}

int HttpRequest::set_body(const std::string &content_type, const std::string &body) {
  m_body = body;
  m_content_type = content_type;
  return 0;
}

int HttpRequest::set_header(const std::string &header_name, const std::string &header_value) {
  m_headers[header_name] = header_value;
  return 0;
}

int HttpRequest::set_header(const std::map<std::string, std::string> &headers) {
  for (const auto &header : headers) {
    m_headers[header.first] = header.second;
  }
  return 0;
}

asio::awaitable<std::string> HttpRequest::request() {
  auto executor = co_await asio::this_coro::executor;
  auto parsedURI = boost::urls::parse_uri(m_url);

  if (parsedURI.has_error()) {
    LOG(ERROR) << "Invalid URL: " << m_url;
    throw boost::system::system_error(parsedURI.error());
  }

  std::string host = parsedURI->host();
  bool is_ssl = (parsedURI->scheme() == "https");
  
  int port = parsedURI->port_number();
  if (port == 0) {
    port = is_ssl ? 443 : 80;
  }
  std::string path = parsedURI->path();

  http::request<http::string_body> req{http::verb::get, path, 11};

  req.set(http::field::host, host); // Set the host header
  req.set(http::field::user_agent, UA); // Set the user agent
  for (const auto& iter : m_headers) {
    req.set(iter.first, iter.second); // Set custom headers
  }
  if (m_method == "POST") {
    req.method(http::verb::post); // Set the request method to POST
    req.set(http::field::content_type, m_content_type); // Set the content type

    req.body() = m_body; // Set the request body
    req.prepare_payload();
  } else {
    req.method(http::verb::get); // Default to GET
  }

  if (is_ssl) {
    auto socket = co_await ConnectSSL(host, port)();
    co_return co_await do_request(std::move(socket), req);
  } else {
    auto socket = co_await Connect(host, port)();
    co_return co_await do_request(std::move(socket), req);
  }
}

}  // namespace Common