#ifndef __MARKET_OKX_WS_PUBLIC_API_H__
#define __MARKET_OKX_WS_PUBLIC_API_H__

#include <boost/asio/awaitable.hpp>
#include <boost/json.hpp>
#include <memory>
#include <string>

#include "WebSocket.h"
#include "market/public_depth_api.h"
#include "market/public_trade_api.h"
#include "websocket_api_detail.h"

namespace Market {
namespace Okx {

template <typename T>
using awaitable = boost::asio::awaitable<T>;

class WsPublicApi : public PublicTradeApi, public PublicDepthApi {
 public:
  WsPublicApi(int retry_times = 3);

  // PublicApi
  int subscribe_trades(const std::string& channel, const std::string& instId) override;
  int subscribe_depth(const std::string& channel, const std::string& instId) override;

 private:
  // PublicTradeApi
  awaitable<std::string> read_data() override;
  awaitable<std::vector<std::shared_ptr<Market::TradeData>>> parse_data(const std::string& data) override;
  awaitable<int> data_handle(std::vector<std::shared_ptr<Market::TradeData>> data) override { co_return 0; };

  // PublicDepthApi
  virtual awaitable<std::string> read_depth_data() override;
  virtual awaitable<std::shared_ptr<DepthData>> parse_depth_data(const std::string& data) override;
  virtual awaitable<int> depth_data_handle(std::shared_ptr<Market::DepthData> data) override;

  awaitable<int> connect_trades();
  awaitable<int> connect_depth();

  awaitable<int> connect(std::unique_ptr<Common::WebSocket>& ws,
                         const std::vector<std::shared_ptr<Detail::WsRequestArgsParamSubscribe>>& args);

  std::unique_ptr<Common::WebSocket> m_trades_ws;
  std::unique_ptr<Common::WebSocket> m_depth_ws;
  std::string base_uri = "wss://ws.okx.com:8443/ws/v5/public";

  std::vector<std::shared_ptr<Detail::WsRequestArgsParamSubscribe>> m_subscribed_trades;
  std::vector<std::shared_ptr<Detail::WsRequestArgsParamSubscribe>> m_subscribed_depth;
};

}  // namespace Okx
}  // namespace Market

#endif
