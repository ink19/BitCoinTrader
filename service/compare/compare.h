#ifndef __SERVICE_COMPARE_H__
#define __SERVICE_COMPARE_H__

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <memory>
#include <map>

#include "market/public_api.h"
#include "wework/wework.h"

namespace Service {
namespace Compare {

using dec_float = boost::multiprecision::cpp_dec_float_100;
namespace asio = boost::asio;

class MarketInstVal {
 public:
  dec_float price;
  bool changed = false;
};

class MarketVal {
 public:
  std::map<std::string, std::shared_ptr<MarketInstVal>> data;
};

class ReportInstVal {
public:
  std::string inst_id;
  dec_float max_price;
  dec_float min_price;
  dec_float max_price_diff;
  dec_float max_price_diff_rate;
};

class ReportVal {
 public:
  std::map<std::string, std::shared_ptr<ReportInstVal>> data;
  int64_t last_report_time = 0;
};

class CompareSerivce {
 public:
  CompareSerivce(std::shared_ptr<Market::PublicApi> binance_api, std::shared_ptr<Market::PublicApi> okx_api,
                 std::shared_ptr<Notice::WeWork::WeWorkAPI> wework_api);

  asio::awaitable<int> run();
  asio::awaitable<int> compare();

 private:
  std::shared_ptr<Market::PublicApi> binance_api;
  std::shared_ptr<Market::PublicApi> okx_api;
  std::shared_ptr<Notice::WeWork::WeWorkAPI> wework_api;

  asio::awaitable<int> okx_callback(std::shared_ptr<Market::TradeData> data);
  asio::awaitable<int> binance_callback(std::shared_ptr<Market::TradeData> data);

  MarketVal binance;
  MarketVal okx;

  ReportVal report;
};

}  // namespace Compare
}  // namespace Service

#endif
