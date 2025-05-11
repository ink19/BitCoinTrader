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
  } else {
    m_depth_data->ts = res_depth->ts;

    for (auto& item : res_depth->asks) {
      m_depth_data->asks.push_back(item);
    }

    for (auto& item : res_depth->bids) {
      m_depth_data->bids.push_back(item);
    }
    // 排序
    std::sort(m_depth_data->asks.begin(), m_depth_data->asks.end(),
              [](const auto& a, const auto& b) { return a->price < b->price; });

    std::sort(m_depth_data->bids.begin(), m_depth_data->bids.end(),
              [](const auto& a, const auto& b) { return a->price > b->price; });

    // 合并相同价格
    for (auto it = m_depth_data->asks.begin(); it != m_depth_data->asks.end();) {
      auto it2 = it + 1;
      while (it2 != m_depth_data->asks.end() && (*it)->price == (*it2)->price) {
        (*it)->size += (*it2)->size;
        it2 = m_depth_data->asks.erase(it2);
      }
      it = it2;
    }

    for (auto it = m_depth_data->bids.begin(); it != m_depth_data->bids.end();) {
      auto it2 = it + 1;
      while (it2 != m_depth_data->bids.end() && (*it)->price == (*it2)->price) {
        (*it)->size += (*it2)->size;
        it2 = m_depth_data->bids.erase(it2);
      }
      it = it2;
    }

    // 撮合交易
    while (!m_depth_data->asks.empty() && !m_depth_data->bids.empty() &&
           m_depth_data->asks[0]->price > m_depth_data->bids[0]->price) {
      auto& ask = m_depth_data->asks[0];
      auto& bid = m_depth_data->bids[0];
      if (ask->size > bid->size) {
        ask->size -= bid->size;
        m_depth_data->bids.erase(m_depth_data->bids.begin());
      } else {
        bid->size -= ask->size;
        m_depth_data->asks.erase(m_depth_data->asks.begin());
      }
    }
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
