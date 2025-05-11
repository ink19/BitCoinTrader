#ifndef __MARKET_BINANCE_WS_VISION_H__
#define __MARKET_BINANCE_WS_VISION_H__

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <vector>
#include <string>

#include "WebSocket.h"
#include "market/public_trade_api.h"
#include <map>

namespace Market {
namespace Binance {

namespace asio = boost::asio;

class BinanceVisionAPI : public PublicTradeApi {
 public:
  BinanceVisionAPI(int retry_times = 3);
  int subscribe_trades(const std::string& channel, const std::string& instId) override;

 private:
  asio::awaitable<int> connect();
  awaitable<std::string> read_data() override;
  awaitable<std::vector<std::shared_ptr<Market::TradeData>>> parse_data(const std::string& data) override;
  awaitable<int> data_handle(std::vector<std::shared_ptr<Market::TradeData>> data) override {
    co_return 0;
  }


  uint64_t generate_id();
  const std::string uri = "wss://data-stream.binance.vision:443/ws";
  std::unique_ptr<Common::WebSocket> m_vision_ws;
  std::vector<std::string> m_channels;

  std::map<std::string, std::string> m_inst_id_map;
};

}  // namespace Binance
}  // namespace Market

#endif
