#ifndef __MARKET_OKX_WEBSOCKET_API_H__
#define __MARKET_OKX_WEBSOCKET_API_H__

#include <boost/asio/awaitable.hpp>
#include <boost/json.hpp>
#include <memory>
#include <string>

#include "WebSocket.h"
#include "api.h"
#include "websocket_api_detail.h"

namespace Market {

namespace Okx {

class WebSocketApi : private API {
 public:
  WebSocketApi(const std::string& api_key, const std::string& secret_key, const std::string& passphrase);

  boost::asio::awaitable<int> login();

 private:
  boost::asio::awaitable<int> connect_public();
  boost::asio::awaitable<int> keep_alive();

  boost::asio::awaitable<std::shared_ptr<Market::Okx::Detail::WsResponeSubscribe>> read_public();

  std::unique_ptr<Common::WebSocket> m_ws_api_private;

};

}  // namespace Okx

}  // namespace Market

#endif
