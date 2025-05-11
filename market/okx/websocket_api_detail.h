#ifndef __MARKET_OKX_WEBSOCKET_API_DETAIL_H__
#define __MARKET_OKX_WEBSOCKET_API_DETAIL_H__

#include <algorithm>
#include <boost/json.hpp>
#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/serialization/serialization.hpp>
#include <memory>

#include "mapping.hpp"
#include "market/market_data.h"

namespace Market {

namespace Okx {

namespace Detail {

namespace mp = boost::multiprecision;
using dec_float = mp::cpp_dec_float_100;

class WsRequestArgsParamSubscribe {
 public:
  std::string channel;
  std::string instId;
};

class WsRequestBody {
 public:
  std::string op;
  std::vector<std::shared_ptr<WsRequestArgsParamSubscribe>> args;
};

class WsResponeArg {
 public:
  std::string channel;
  std::string instId;
};

class WsResponeSubscribeData;

class WsResponeBody {
 public:
  WsResponeArg arg;
  std::vector<std::shared_ptr<WsResponeSubscribeData>> data;
};

class WsResponeSubscribeData {
 public:
  std::string instId;
  std::string tradeId;
  std::string side;
  dec_float px;
  dec_float sz;
  dec_float count;
  int64_t ts;
};

using TradeData = Market::TradeData;
MAPPER_TYPE(WsResponeSubscribeData, TradeData)
MAPPER_TYPE_ITEM(instId, inst_id)
MAPPER_TYPE_ITEM(tradeId, trade_id)
MAPPER_TYPE_ITEM(side, side)
MAPPER_TYPE_ITEM(px, px)
MAPPER_TYPE_ITEM(sz, sz)
MAPPER_TYPE_ITEM(count, count)
MAPPER_TYPE_ITEM(ts, ts)
MAPPER_TYPE_END(WsResponeSubscribeData, TradeData)

class WsResponeDepthData {
 public:
  std::vector<std::vector<dec_float>> asks;
  std::vector<std::vector<dec_float>> bids;
  int64_t ts;
  int64_t checksum;
  int64_t prevSeqId;
  int64_t seqId;
};

using DepthData = Market::DepthData;
using DepthDataItem = Market::DepthDataItem;

inline std::shared_ptr<DepthDataItem> translate_depth_data_item(const std::vector<dec_float>& ask) {
  if (ask.size() != 4) {
    return nullptr;
  }
  auto res = std::make_shared<DepthDataItem>();
  res->price = ask[0];
  res->size = ask[1];
  res->order_count = ask[2].convert_to<int64_t>();
  return res;
}

MAPPER_TYPE(WsResponeDepthData, DepthData)
MAPPER_TYPE_ITEM(ts, ts)
MAPPER_TYPE_ITEM_WITH_FUNC(asks, asks, [](auto& ask) {
  std::vector<std::shared_ptr<DepthDataItem>> result;
  std::transform(ask.begin(), ask.end(), std::back_inserter(result), translate_depth_data_item);
  return result;
})
MAPPER_TYPE_ITEM_WITH_FUNC(bids, bids, [](auto& bid) {
  std::vector<std::shared_ptr<DepthDataItem>> result;
  std::transform(bid.begin(), bid.end(), std::back_inserter(result), translate_depth_data_item);
  return result;
})
MAPPER_TYPE_END(WsResponeDepthData, DepthData)

template <typename T>
class WsRespone {
 public:
  WsResponeArg arg;
  std::string action;
  std::vector<std::shared_ptr<T>> data;
};

typedef WsRespone<WsResponeSubscribeData> WsResponeSubscribe;
typedef WsRespone<WsResponeDepthData> WsResponeDepth;

}  // namespace Detail

}  // namespace Okx

}  // namespace Market

#endif
