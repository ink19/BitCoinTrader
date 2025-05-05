#ifndef __MARKET_BINANCE_WS_VISION_H__
#define __MARKET_BINANCE_WS_VISION_H__

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <vector>
#include <string>

#include "WebSocket.h"
#include "market/public_api.h"
#include <map>

namespace Market {
namespace Binance {

namespace asio = boost::asio;

class BinanceVisionAPI : public PublicApi {
 public:
  BinanceVisionAPI() {}
  asio::awaitable<int> subscribe_trades(const std::string& channel, const std::string& instId) override;
  asio::awaitable<int> exec() override;

  void set_trades_callback(TradesCallback callback) override { m_agg_trade_callback = callback; }

 private:
  asio::awaitable<int> connect();
  uint64_t generate_id();
  const std::string uri = "wss://data-stream.binance.vision:443/ws";
  std::unique_ptr<Common::WebSocket> m_vision_ws;
  std::vector<std::string> m_channels;

  TradesCallback m_agg_trade_callback;
  std::map<std::string, std::string> m_inst_id_map;
};

}  // namespace Binance
}  // namespace Market

#endif
