#include "ws_vision.h"

#include <fmt/format.h>
#include <glog/logging.h>

#include <boost/algorithm/string.hpp>
#include <boost/json.hpp>
#include <random>

#include "component.hpp"
#include "data_detail.h"

namespace Market {
namespace Binance {

BinanceVisionAPI::BinanceVisionAPI(int retry_times) : PublicTradeApi(retry_times), PublicDepthApi(retry_times) {}

int BinanceVisionAPI::subscribe_trades(const std::string& channel, const std::string& instId) {
  auto new_inst_id = boost::algorithm::to_lower_copy(boost::algorithm::replace_all_copy(instId, "-", ""));
  m_inst_id_map[new_inst_id] = instId;

  auto new_channel = "";
  if (channel == "trades") {
    new_channel = "aggTrade";
  }

  auto chan = fmt::format("{0}@{1}", new_inst_id, new_channel);
  m_channels.push_back(chan);
  return 0;
}

int BinanceVisionAPI::subscribe_depth(const std::string& channel, const std::string& instId) {
  auto new_inst_id = boost::algorithm::to_lower_copy(boost::algorithm::replace_all_copy(instId, "-", ""));
  m_inst_id_map[new_inst_id] = instId;

  auto new_channel = "";
  if (channel == "books5") {
    new_channel = "depth5@100ms";
  }

  auto chan = fmt::format("{0}@{1}", new_inst_id, new_channel);
  m_depth_channels.push_back(chan);
  return 0;
}

awaitable<int> BinanceVisionAPI::connect(std::unique_ptr<Common::WebSocket>& ws , std::vector<std::string> channels) {
  ws = std::make_unique<Common::WebSocket>(uri);
  co_await ws->connect();

  if (channels.size() > 0) {
    WsApiRequest request;
    request.id = generate_id();
    request.method = "SUBSCRIBE";
    request.params = channels;
    auto request_body = Common::JsonSerialize(request);
    co_await ws->write(boost::json::serialize(request_body));
  }

  co_return 0;
}

awaitable<int> BinanceVisionAPI::connect_trade() {
  co_await connect(m_vision_ws, m_channels);
  co_return 0;
}

awaitable<std::string> BinanceVisionAPI::read_data() {
  if (m_vision_ws == nullptr) {
    co_await connect_trade();
  }

  try {
    auto read_result = co_await m_vision_ws->read();
    co_return read_result;
  } catch (const boost::beast::system_error& e) {
    LOG(ERROR) << "Error: " << e.code() << " msg: " << e.what();
    m_vision_ws = nullptr;
  }

  co_return std::string("");
}

awaitable<std::vector<std::shared_ptr<Market::TradeData>>> BinanceVisionAPI::parse_data(const std::string& data) {
  auto res = Common::DataReader<WsApiAggTrade>::read_shared_ptr(boost::json::parse(data));

  res->s = m_inst_id_map[boost::to_lower_copy(res->s)];
  co_return std::vector<std::shared_ptr<Market::TradeData>>{mapping(res)};
}

uint64_t BinanceVisionAPI::generate_id() {
  std::random_device rd;
  std::mt19937 gen(rd());
  return gen();
}

awaitable<int> BinanceVisionAPI::connect_depth() {
  co_await connect(m_depth_ws, m_depth_channels);
  co_return 0;
}

awaitable<std::string> BinanceVisionAPI::read_depth_data() {
  if (m_depth_ws == nullptr) {
    co_await connect_depth();
  }

  try {
    auto read_result = co_await m_depth_ws->read();
    co_return read_result;
  } catch (const boost::beast::system_error& e) {
    LOG(ERROR) << "Error: " << e.code() << " msg: " << e.what();
    m_depth_ws = nullptr;
  }

  co_return std::string("");
}

awaitable<std::shared_ptr<DepthData>> BinanceVisionAPI::parse_depth_data(const std::string& data) {
  // LOG(INFO) << "parse_depth_data: " << data;
  auto res = Common::DataReader<WsApiDepth>::read_shared_ptr(boost::json::parse(data));
  if (res->lastUpdateId == 0) {
    co_return nullptr;
  }
  auto depth_info = mapping(res);
  depth_info->data_type = DepthDataTypeSnapshot;

  if (m_depth == nullptr) {
    m_depth = std::make_shared<BinanceDepthData>();
  }

  m_depth->depth = depth_info;
  m_depth->seqId = res->lastUpdateId;

  co_return m_depth->depth;
}

awaitable<int> BinanceVisionAPI::depth_data_handle(std::shared_ptr<Market::DepthData> data) {
  // LOG(INFO) << "depth_data_handle: " << Common::JsonSerialize(data);
  co_return 0;
}


}  // namespace Binance
}  // namespace Market
