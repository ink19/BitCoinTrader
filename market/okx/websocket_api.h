#ifndef __MARKET_OKX_WEBSOCKET_API_H__
#define __MARKET_OKX_WEBSOCKET_API_H__

#include <boost/asio/awaitable.hpp>
#include <boost/json.hpp>
#include <memory>
#include <string>
#include <websocket_api_detail.h>

#include "WebSocket.h"
#include "api.h"

namespace Market {

namespace Okx {

class WebSocketApi : private API {
 public:
  WebSocketApi(const std::string& api_key, const std::string& secret_key, const std::string& passphrase);

  boost::asio::awaitable<int> login();
  boost::asio::awaitable<int> subscribe(const std::string& channel, const std::string& instId);

  boost::asio::awaitable<void> exec();

 private:
  boost::asio::awaitable<int> connect_public();
  boost::asio::awaitable<int> keep_alive();

  boost::asio::awaitable<typename std::shared_ptr<Market::Okx::Detail::WsResponeBody>> read_private();
  boost::asio::awaitable<Market::Okx::Detail::WsResponeSubscribe> read_public();

  std::unique_ptr<Common::WebSocket> m_ws_api_private;

  std::unique_ptr<Common::WebSocket> m_ws_api_public;
  std::vector<std::shared_ptr<Detail::WsRequestArgsParamSubscribe>> m_public_subscribed_args;
};

}  // namespace Okx

}  // namespace Market

#endif
