#include "compare.h"

#include <fmt/format.h>
#include <glog/logging.h>
#include "config.h"

namespace Service {

namespace Compare {

namespace bm = boost::multiprecision;

CompareSerivce::CompareSerivce(std::shared_ptr<Market::PublicTradeApi> binance_api,
                               std::shared_ptr<Market::PublicTradeApi> okx_api,
                               std::shared_ptr<Notice::WeWork::WeWorkAPI> wework_api)
    : binance_api(binance_api), okx_api(okx_api), wework_api(wework_api) {
  binance_api->set_trades_callback(std::bind(&CompareSerivce::binance_callback, this, std::placeholders::_1));
  okx_api->set_trades_callback(std::bind(&CompareSerivce::okx_callback, this, std::placeholders::_1));
  report.last_report_time = Common::get_current_time_s();
}

asio::awaitable<int> CompareSerivce::okx_callback(std::shared_ptr<Market::TradeData> data) {
  auto okx_price = okx.data[data->inst_id];
  if (okx_price == 0) {
    okx.data[data->inst_id] = std::make_shared<MarketInstVal>();
    okx_price = okx.data[data->inst_id];
  }

  okx_price->price = data->px;
  okx_price->changed = true;
  co_return 0;
}

asio::awaitable<int> CompareSerivce::binance_callback(std::shared_ptr<Market::TradeData> data) {
  auto binance_price = binance.data[data->inst_id];
  if (binance_price == 0) {
    binance.data[data->inst_id] = std::make_shared<MarketInstVal>();
    binance_price = binance.data[data->inst_id];
  }

  binance_price->price = data->px;
  binance_price->changed = true;
  co_return 0;
}

asio::awaitable<int> CompareSerivce::run() {
  auto ctx = co_await asio::this_coro::executor;

  boost::asio::steady_timer timer(ctx);

  while (true) {
    timer.expires_after(std::chrono::milliseconds(100));
    co_await timer.async_wait(asio::use_awaitable);
    boost::asio::co_spawn(ctx, this->compare(), asio::detached);
  }

  co_return 0;
}

asio::awaitable<int> CompareSerivce::compare() {
  uint64_t now_time = Common::get_current_time_s();
  for (auto okxDataItem : okx.data) {
    auto instId = okxDataItem.first;
    auto okxItem = okxDataItem.second;
    if (instId.empty()) {
      continue;
    }

    if (binance.data.find(instId) == binance.data.end() || okxItem == nullptr) {
      continue;
    }

    auto binanceItem = binance.data[instId];

    if (binanceItem->price == 0 || okxItem->price == 0) {
      co_return 0;
    }
  
    if (!binanceItem->changed && !okxItem->changed) {
      co_return 0;
    }
  
    binanceItem->changed = false;
    okxItem->changed = false;
  
    dec_float diff = binanceItem->price - okxItem->price;
    diff = bm::abs(diff);
  
    dec_float rate = diff / okxItem->price * 100;
  
    LOG(ERROR) << instId <<" diff: " << diff << " rate: " << rate << "%";
  
    if (rate > AppConfig.compare()->min_diff() && wework_api != nullptr) {
      co_await wework_api->send(fmt::format("{} binance price: {}, okx price: {}, diff: {}, rate: {}%", instId, binanceItem->price.str(),
                                            okxItem->price.str(), diff.str(), rate.str(5)));
    }

    auto report_item = report.data[instId];
    if (report_item == nullptr) {
      report_item = std::make_shared<ReportInstVal>();
      report_item->inst_id = instId;

      report.data[instId] = report_item;
    }

    if (rate > report_item->max_price_diff_rate) {
      report_item->max_price_diff_rate = rate;
    }

    if (diff > report_item->max_price_diff) {
      report_item->max_price_diff = diff;
    }

    if (binanceItem->price > report_item->max_price) {
      report_item->max_price = binanceItem->price;
    }

    if (okxItem->price > report_item->max_price) {
      report_item->max_price = okxItem->price;
    }

    if (report_item->min_price == 0 || binanceItem->price < report_item->min_price) {
      report_item->min_price = binanceItem->price;
    }

    if (report_item->min_price == 0 || okxItem->price < report_item->min_price) {
      report_item->min_price = okxItem->price;
    }
  }

  
  if (now_time > report.last_report_time + AppConfig.compare()->report_time_s()) {
    report.last_report_time = now_time;
    
    std::string report_str = "report: \n";
    for (auto report_item : report.data) {
      report_str += fmt::format("{} max_price_diff_rate: {}%, max_price_diff: {}, max_price: {}, min_price: {}\n", report_item.first, report_item.second->max_price_diff_rate.str(5), report_item.second->max_price_diff.str(5), report_item.second->max_price.str(5), report_item.second->min_price.str(5));
    }

    report.data.clear();

    LOG(ERROR) << report_str;
    if (wework_api != nullptr) {
      co_await wework_api->send(report_str);
    }
  }
  co_return 0;
}

}  // namespace Compare

}  // namespace Service
