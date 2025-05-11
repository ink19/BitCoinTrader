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

class PublicTradeApi {
public:
  PublicTradeApi(int retry_times);
  virtual int subscribe_trades(const std::string& channel, const std::string& inst_id) = 0;
  void set_trades_callback(TradesCallback callback) {
    m_public_callback = callback;
  }

  awaitable<int> exec();
private:
  virtual awaitable<std::string> read_data() = 0;
  virtual awaitable<std::vector<std::shared_ptr<Market::TradeData>>> parse_data(const std::string& data) = 0;
  virtual awaitable<int> data_handle(std::vector<std::shared_ptr<Market::TradeData>> data) = 0;

  awaitable<int> exec_data(const std::string& data_str);

  Market::TradesCallback m_public_callback;
  const int m_retry_times;
};

}

#endif
