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
#include "market/public_api.h"

namespace Market {

namespace Okx {

class WebSocketApi : private API, public PublicApi {
 public:
  WebSocketApi(const std::string& api_key, const std::string& secret_key, const std::string& passphrase);

  boost::asio::awaitable<int> login();

  // PublicApi
  boost::asio::awaitable<int> subscribe_trades(const std::string& channel, const std::string& instId) override;
  boost::asio::awaitable<int> exec() override;
  void set_trades_callback(Market::TradesCallback callback) override { m_public_callback = callback; }

 private:
  boost::asio::awaitable<int> connect_public();
  boost::asio::awaitable<int> keep_alive();

  boost::asio::awaitable<typename std::shared_ptr<Market::Okx::Detail::WsResponeBody>> read_private();
  boost::asio::awaitable<std::shared_ptr<Market::Okx::Detail::WsResponeSubscribe>> read_public();

  std::unique_ptr<Common::WebSocket> m_ws_api_private;

  std::unique_ptr<Common::WebSocket> m_ws_api_public;
  std::vector<std::shared_ptr<Detail::WsRequestArgsParamSubscribe>> m_public_subscribed_args;
  Market::TradesCallback m_public_callback;
};

}  // namespace Okx

}  // namespace Market

#endif
