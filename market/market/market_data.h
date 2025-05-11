#ifndef __MARKET_MARKET_H__
#define __MARKET_MARKET_H__

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <string>
#include <memory>

namespace Market {

using dec_float = boost::multiprecision::cpp_dec_float_100;

class TradeData {
 public:
  std::string inst_id;
  std::string trade_id;
  std::string side;
  dec_float px, sz, count;
  int64_t ts;
};

class DepthDataItem {
 public:
  dec_float price;
  dec_float size;
  int64_t order_count;
};

const std::string DepthDataTypeSnapshot = "snapshot";
const std::string DepthDataTypeIncremental = "incremental";

class DepthData {
 public:
  std::string inst_id;
  std::string data_type; // 快照 或 增量
  std::vector<std::shared_ptr<DepthDataItem>> asks, bids; // ask 卖， bid 买。ask 价格从低到高， bid 价格从高到低
  int64_t ts;  // 上次更新时间
};

}  // namespace Market

#endif
