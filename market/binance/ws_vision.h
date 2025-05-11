#ifndef __MARKET_BINANCE_WS_VISION_H__
#define __MARKET_BINANCE_WS_VISION_H__

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <map>
#include <string>
#include <vector>

#include "WebSocket.h"
#include "market/public_depth_api.h"
#include "market/public_trade_api.h"

namespace Market {
namespace Binance {

template <typename T>
using awaitable = boost::asio::awaitable<T>;

class BinanceDepthData {
 public:
  std::shared_ptr<Market::DepthData> depth;
  int64_t seqId;
};

class BinanceVisionAPI : public PublicTradeApi, public PublicDepthApi {
 public:
  BinanceVisionAPI(int retry_times = 3);
  int subscribe_trades(const std::string& channel, const std::string& instId) override;
  int subscribe_depth(const std::string& channel, const std::string& inst_id) override;

 private:
  awaitable<int> connect(std::unique_ptr<Common::WebSocket>& ws, std::vector<std::string> channels);

  // PublicTradeApi
  awaitable<int> connect_trade();
  awaitable<std::string> read_data() override;
  awaitable<std::vector<std::shared_ptr<Market::TradeData>>> parse_data(const std::string& data) override;
  awaitable<int> data_handle(std::vector<std::shared_ptr<Market::TradeData>> data) override { co_return 0; }

  std::unique_ptr<Common::WebSocket> m_vision_ws;
  std::vector<std::string> m_channels;

  // PublicDepthApi
  awaitable<int> connect_depth();
  awaitable<std::string> read_depth_data() override;
  awaitable<std::shared_ptr<DepthData>> parse_depth_data(const std::string& data) override;
  awaitable<int> depth_data_handle(std::shared_ptr<Market::DepthData> data) override;
  std::shared_ptr<BinanceDepthData> m_depth;

  std::unique_ptr<Common::WebSocket> m_depth_ws;
  std::vector<std::string> m_depth_channels;

  const std::string uri = "wss://data-stream.binance.vision:443/ws";
  uint64_t generate_id();
  std::map<std::string, std::string> m_inst_id_map;
};

}  // namespace Binance
}  // namespace Market

#endif
