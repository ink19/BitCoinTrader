#include "public_depth_api.h"

#include <glog/logging.h>

#include <algorithm>
#include <boost/asio.hpp>
#include <mutex>

#include "errcode.h"

namespace Market {

PublicDepthApi::PublicDepthApi(int retry_times) : m_retry_times(retry_times) {}

awaitable<int> PublicDepthApi::exec_depth_data(const std::string& data_str) {
  auto res_depth = co_await parse_depth_data(data_str);
  if (res_depth == nullptr) {
    co_return 0;
  }

  std::lock_guard<std::shared_mutex> guard(m_lock);
  if (res_depth->data_type == DepthDataTypeSnapshot) {
    m_depth_data = res_depth;
  } else { // 不应该出现其他类型
    throw std::runtime_error("Unknown DepthDataType");
  }

  co_await depth_data_handle(res_depth);

  co_return 0;
}

awaitable<int> PublicDepthApi::exec() {
  auto ctx = co_await boost::asio::this_coro::executor;
  for (;;) {
    int retry_times = 0;
    for (int i = 0; i < m_retry_times; ++i) {
      try {
        auto read_result = co_await read_depth_data();
        retry_times++;
        boost::asio::co_spawn(ctx, this->exec_depth_data(read_result), boost::asio::detached);
        break;
      } catch (const boost::system::system_error& e) {
        LOG(ERROR) << "Error: " << e.code() << " msg: " << e.what();
      }
    }

    if (retry_times >= m_retry_times) {
      throw boost::system::system_error(static_cast<int>(ErrCode::Connect_Fail), MarketErrorCategory(), "Connect Fail");
    }
  }
  co_return 0;
}

}  // namespace Market
