#include "public_trade_api.h"

#include <glog/logging.h>

#include <boost/asio.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/beast.hpp>

#include "errcode.h"

namespace Market {

awaitable<int> PublicTradeApi::exec() {
  auto ctx = co_await boost::asio::this_coro::executor;
  for (;;) {
    int retry_times = 0;
    for (int i = 0; i < m_retry_times; ++i) {
      try {
        auto read_result = co_await read_data();
        retry_times++;
        boost::asio::co_spawn(ctx, this->exec_data(read_result), boost::asio::detached);
        break;
      } catch (const boost::beast::system_error& e) {
        LOG(ERROR) << "Error: " << e.code() << " msg: " << e.what();
      }
    }

    if (retry_times >= m_retry_times) {
      throw boost::system::system_error(static_cast<int>(ErrCode::Connect_Fail), MarketErrorCategory(), "Connect Fail");
    }
  }
  co_return 0;
}

awaitable<int> PublicTradeApi::exec_data(const std::string& data) {
  auto parse_result = co_await parse_data(data);
  if (m_public_callback) {
    for (auto& data : parse_result) {
      co_await m_public_callback(data);
    }
  }

  co_return co_await data_handle(parse_result);
}

PublicTradeApi::PublicTradeApi(int retry_times) : m_retry_times(retry_times) {}

}  // namespace Market
