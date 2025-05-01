#ifndef __MARKET_OKX_WEBSOCKET_API_H__
#define __MARKET_OKX_WEBSOCKET_API_H__

#include <boost/asio/awaitable.hpp>
#include <boost/json.hpp>
#include <functional>
#include <memory>
#include <string>

#include "WebSocket.h"
#include "api.h"
#include "websocket_api_detail.h"

namespace Market {

namespace Okx {

typedef std::function<boost::asio::awaitable<int>(std::shared_ptr<Detail::WsResponeSubscribeData>)>
    WsResponeSubscribeCallback;

class WebSocketApi : private API {
 public:
  WebSocketApi(const std::string& api_key, const std::string& secret_key, const std::string& passphrase);

  boost::asio::awaitable<int> login();
  boost::asio::awaitable<int> subscribe(const std::string& channel, const std::string& instId);

  boost::asio::awaitable<void> exec();

  void set_public_callback(WsResponeSubscribeCallback callback) { m_public_callback = callback; }

 private:
  boost::asio::awaitable<int> connect_public();
  boost::asio::awaitable<int> keep_alive();

  boost::asio::awaitable<typename std::shared_ptr<Market::Okx::Detail::WsResponeBody>> read_private();
  boost::asio::awaitable<std::shared_ptr<Market::Okx::Detail::WsResponeSubscribe>> read_public();

  std::unique_ptr<Common::WebSocket> m_ws_api_private;

  std::unique_ptr<Common::WebSocket> m_ws_api_public;
  std::vector<std::shared_ptr<Detail::WsRequestArgsParamSubscribe>> m_public_subscribed_args;
  WsResponeSubscribeCallback m_public_callback;
};

}  // namespace Okx

}  // namespace Market

#endif
