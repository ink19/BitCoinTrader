#ifndef __MARKET_PUBLIC_DEPTH_API_H__
#define __MARKET_PUBLIC_DEPTH_API_H__

#include <string>
#include <memory>
#include "market_data.h"
#include <boost/asio/awaitable.hpp>
#include <shared_mutex>

namespace Market {

template<typename T>
using awaitable = boost::asio::awaitable<T>;

class PublicDepthApi {
 public:
  PublicDepthApi(int retry_times = 3);
  virtual int subscribe_depth(const std::string& channel, const std::string& inst_id) = 0;
  awaitable<int> exec();

 private:
  virtual awaitable<std::string> read_depth_data() = 0;
  virtual awaitable<std::shared_ptr<DepthData>> parse_depth_data(const std::string& data) = 0;
  virtual awaitable<int> depth_data_handle(std::shared_ptr<Market::DepthData> data) = 0;
  
  awaitable<int> exec_depth_data(const std::string& data_str);
  
  std::shared_ptr<DepthData> m_depth_data;
  std::shared_mutex m_lock;
  const int m_retry_times;
};

}  // namespace Market

#endif
