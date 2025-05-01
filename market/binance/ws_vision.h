#ifndef __MARKET_BINANCE_WS_VISION_H__
#define __MARKET_BINANCE_WS_VISION_H__

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <vector>
#include <string>

#include "WebSocket.h"
#include "data_detail.h"

namespace Market {
namespace Binance {

namespace asio = boost::asio;

typedef std::function<boost::asio::awaitable<int>(std::shared_ptr<WsApiAggTrade>)>
    WsApiAggTradeCallback;

class BinanceVisionAPI {
 public:
  BinanceVisionAPI() {}
  asio::awaitable<int> subscribe(const std::string& channel, const std::string& instId);
  asio::awaitable<void> exec();

  void set_agg_trade_callback(WsApiAggTradeCallback callback) { m_agg_trade_callback = callback; }

 private:
  asio::awaitable<int> connect();
  uint64_t generate_id();
  const std::string uri = "wss://data-stream.binance.vision:443/ws";
  std::unique_ptr<Common::WebSocket> m_vision_ws;
  std::vector<std::string> m_channels;

  WsApiAggTradeCallback m_agg_trade_callback;
};

}  // namespace Binance
}  // namespace Market

#endif
