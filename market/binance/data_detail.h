#ifndef __MARKET_BINANCE_DATA_DETAIL_H
#define __MARKET_BINANCE_DATA_DETAIL_H

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <string>
#include <vector>

#include "market/market_data.h"
#include "mapping.hpp"

namespace Market {
namespace Binance {

using dec_float = boost::multiprecision::cpp_dec_float_100;

class WsApiRequest {
 public:
  std::string method;
  std::vector<std::string> params;
  uint64_t id;
};

class WsApiAggTrade {
 public:
  std::string e;  // 事件类型
  uint64_t E;     // 事件时间
  std::string s;  // 交易对
  uint64_t a;     // 归集交易ID
  dec_float p;    // 成交价格
  dec_float q;    // 成交数量
  uint64_t f;     // 被归集的首个交易ID
  uint64_t l;     // 被归集的末次交易ID
  uint64_t T;     // 成交时间
  bool m;  // 买方是否是做市方。如true，则此次成交是一个主动卖出单，否则是一个主动买入单。
  bool M;  // 请忽略该字段
};

using TradeData = Market::TradeData;
MAPPER_TYPE(WsApiAggTrade, TradeData)
MAPPER_TYPE_ITEM(s, inst_id)
MAPPER_TYPE_ITEM_WITH_FUNC(a, trade_id, std::to_string)
MAPPER_TYPE_ITEM_WITH_FUNC(m, side, std::to_string)
MAPPER_TYPE_ITEM(p, px)
MAPPER_TYPE_ITEM(q, sz)
MAPPER_TYPE_ITEM(q, count)
MAPPER_TYPE_ITEM(T, ts)
MAPPER_TYPE_END(WsResponeSubscribeData, TradeData)


}  // namespace Binance
}  // namespace Market

#endif
