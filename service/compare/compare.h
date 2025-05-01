#ifndef __SERVICE_COMPARE_H__
#define __SERVICE_COMPARE_H__

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <memory>

#include "binance/ws_vision.h"
#include "okx/websocket_api.h"
#include "wework/wework.h"

namespace Service {
namespace Compare {

using dec_float = boost::multiprecision::cpp_dec_float_100;
namespace asio = boost::asio;

class MarketVal {
 public:
  dec_float price;
  bool changed = false;
};

class CompareSerivce {
 public:
  CompareSerivce(std::shared_ptr<Market::Binance::BinanceVisionAPI> binance_api,
                 std::shared_ptr<Market::Okx::WebSocketApi> okx_api,
                 std::shared_ptr<Notice::WeWork::WeWorkAPI> wework_api);

  asio::awaitable<int> run();
  asio::awaitable<int> compare();

 private:
  std::shared_ptr<Market::Binance::BinanceVisionAPI> binance_api;
  std::shared_ptr<Market::Okx::WebSocketApi> okx_api;
  std::shared_ptr<Notice::WeWork::WeWorkAPI> wework_api;

  asio::awaitable<int> okx_callback(std::shared_ptr<Market::Okx::Detail::WsResponeSubscribeData> data);
  asio::awaitable<int> binance_callback(std::shared_ptr<Market::Binance::WsApiAggTrade> data);

  MarketVal binance;
  MarketVal okx;
};

}  // namespace Compare
}  // namespace Service

#endif
