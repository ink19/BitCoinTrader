#ifndef __MARKET_PUBLIC_API_H__
#define __MARKET_PUBLIC_API_H__

#include "market_data.h"
#include <memory>
#include <boost/asio/awaitable.hpp>

namespace Market {

template<typename T>
using awaitable = boost::asio::awaitable<T>;

typedef std::function<awaitable<int>(std::shared_ptr<Market::TradeData>)>
    TradesCallback;

class PublicApi {
public:
  virtual awaitable<int> subscribe_trades(const std::string& channel, const std::string& inst_id) = 0;
  virtual void set_trades_callback(TradesCallback data) = 0;
  virtual awaitable<int> exec() = 0;
};

}

#endif
