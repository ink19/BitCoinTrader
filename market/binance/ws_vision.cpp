#include "ws_vision.h"

#include <fmt/format.h>
#include <glog/logging.h>

#include <boost/json.hpp>
#include <random>
#include <boost/algorithm/string.hpp>

#include "data_detail.h"
#include "component.hpp"

namespace Market {
namespace Binance {

asio::awaitable<int> BinanceVisionAPI::subscribe_trades(const std::string& channel, const std::string& instId) {
  if (m_vision_ws == nullptr) {
    co_await connect();
  }

  auto new_inst_id = boost::algorithm::to_lower_copy(boost::algorithm::replace_all_copy(instId, "-", ""));
  m_inst_id_map[new_inst_id] = instId;

  auto new_channel = "";
  if (channel == "trades") {
    new_channel = "aggTrade";
  }

  auto chan = fmt::format("{0}@{1}", new_inst_id, new_channel);
  m_channels.push_back(chan);

  WsApiRequest request;
  request.id = generate_id();
  request.method = "SUBSCRIBE";
  request.params = {chan};
  auto request_body = Common::JsonSerialize(request);
  co_await m_vision_ws->write(boost::json::serialize(request_body));

  co_return 0;
}

asio::awaitable<int> BinanceVisionAPI::connect() {
  m_vision_ws = std::make_unique<Common::WebSocket>(uri);
  co_await m_vision_ws->connect();

  if (m_channels.size() > 0) {
    WsApiRequest request;
    request.id = generate_id();
    request.method = "SUBSCRIBE";
    request.params = m_channels;
    auto request_body = Common::JsonSerialize(request);
    co_await m_vision_ws->write(boost::json::serialize(request_body));
  }

  co_return 0;
}

uint64_t BinanceVisionAPI::generate_id() {
  std::random_device rd;
  std::mt19937 gen(rd());
  return gen();
}

asio::awaitable<int> BinanceVisionAPI::exec() {
  for (;;) {
    try {
      auto read_result = co_await m_vision_ws->read();
      auto res = Common::DataReader<WsApiAggTrade>::read_shared_ptr(
        boost::json::parse(read_result));
      
      res->s = m_inst_id_map[boost::to_lower_copy(res->s)];

      if (m_agg_trade_callback) {
        co_await m_agg_trade_callback(mapping(res));
      }
    } catch (const boost::beast::system_error& e) {
      LOG(ERROR) << "Error: " << e.code() << " msg: " << e.what();
      m_vision_ws = nullptr;
    }

    if (m_vision_ws.get() == nullptr) {
      co_await connect();
    }
  }
  co_return 0;
}

}  // namespace Binance
}  // namespace Market
