#include "compare.h"

#include <glog/logging.h>
#include <fmt/format.h>

namespace Service {

namespace Compare {

namespace bm = boost::multiprecision;

CompareSerivce::CompareSerivce(std::shared_ptr<Market::Binance::BinanceVisionAPI> binance_api,
                               std::shared_ptr<Market::Okx::WebSocketApi> okx_api,
                               std::shared_ptr<Notice::WeWork::WeWorkAPI> wework_api)
: binance_api(binance_api), okx_api(okx_api), wework_api(wework_api)
{
  binance_api->set_agg_trade_callback(std::bind(&CompareSerivce::binance_callback, this, std::placeholders::_1));
  okx_api->set_public_callback(std::bind(&CompareSerivce::okx_callback, this, std::placeholders::_1));
}

asio::awaitable<int> CompareSerivce::okx_callback(std::shared_ptr<Market::Okx::Detail::WsResponeSubscribeData> data) {
  okx.price = data->px;
  okx.changed = true;
  co_return 0;
}

asio::awaitable<int> CompareSerivce::binance_callback(std::shared_ptr<Market::Binance::WsApiAggTrade> data) {
  binance.price = data->p;
  binance.changed = true;

  co_return 0;
}

asio::awaitable<int> CompareSerivce::run() {
  auto ctx = co_await asio::this_coro::executor;

  boost::asio::steady_timer timer(ctx);

  while (true) {
    timer.expires_after(std::chrono::milliseconds(100));
    co_await timer.async_wait(asio::use_awaitable);
    co_await compare();
  }

  co_return 0;
}

asio::awaitable<int> CompareSerivce::compare() {
  // LOG(INFO) << "binance price: " << binance.price << " okx price: " << okx.price;

  if (binance.price == 0 || okx.price == 0) {
    co_return 0;
  }

  if (!binance.changed && !okx.changed) {
    co_return 0;
  }

  binance.changed = false;
  okx.changed = false;

  dec_float diff = binance.price - okx.price;
  diff = bm::abs(diff);
  
  dec_float rate = diff / okx.price * 100;
  
  LOG(ERROR) << "diff: " << diff << " rate: " << rate << "%";

  if (rate > 0.1 && wework_api != nullptr) {
    co_await wework_api->send(fmt::format("binance price: {}, okx price: {}, diff: {}, rate: {}%", 
      binance.price.str(), okx.price.str(), diff.str(), rate.str(5)));
  }

  co_return 0;
}


}  // namespace Compare

}  // namespace Service
