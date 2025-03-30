#ifndef __MARKET_OKX_WEBSOCKET_API_H__
#define __MARKET_OKX_WEBSOCKET_API_H__

#include <boost/asio/awaitable.hpp>
#include <boost/json.hpp>
#include <memory>
#include <string>
#include <vector>

#include "WebSocket.h"

namespace Market {

namespace Okx {

class WebSocketApi {
 public:
  WebSocketApi(const std::string& api_key, const std::string& secret_key, const std::string& passphrase);

  boost::asio::awaitable<int> login();
  boost::asio::awaitable<std::string> read();
  boost::asio::awaitable<void> exec();

 private:
  std::string m_api_key;
  std::string m_secret_key;
  std::string m_passphrase;
  std::string genSingature(const std::string& timestamp, const std::string& method, const std::string& request_path);
  std::unique_ptr<Common::WebSocket> m_ws_api_detail;
};

}  // namespace Okx

}  // namespace Market

#endif