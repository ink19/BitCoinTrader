#include "ws_public_api.h"

#include <glog/logging.h>
#include "component.hpp"
#include <memory>
#include "errcode.h"

namespace Market {
namespace Okx {

// 构造函数
WsPublicApi::WsPublicApi(int retry_times) : PublicTradeApi(retry_times){}

int WsPublicApi::subscribe_trades(const std::string& channel, const std::string& instId) {
  auto subscribe_param = std::make_shared<Detail::WsRequestArgsParamSubscribe>();
  subscribe_param->channel = channel;
  subscribe_param->instId = instId;
  m_subscribed_trades.push_back(subscribe_param);

  return 0;
}

int WsPublicApi::subscribe_depth(const std::string& channel, const std::string& instId) {
  auto subscribe_param = std::make_shared<Detail::WsRequestArgsParamSubscribe>();
  subscribe_param->channel = channel;
  subscribe_param->instId = instId;
  m_subscribed_depth.push_back(subscribe_param);

  return 0;
}

boost::asio::awaitable<int> WsPublicApi::connect_trades() {
  co_return co_await connect(m_trades_ws, m_subscribed_trades);
}

boost::asio::awaitable<int> WsPublicApi::connect_depth() {
  co_return co_await connect(m_depth_ws, m_subscribed_depth);
}

awaitable<int> WsPublicApi::connect(std::unique_ptr<Common::WebSocket>& ws, const std::vector<std::shared_ptr<Detail::WsRequestArgsParamSubscribe>>& args) {
  auto ctx = co_await boost::asio::this_coro::executor;
  ws = std::make_unique<Common::WebSocket>(base_uri);
  co_await ws->connect();

  if (args.size() > 0) {
    Detail::WsRequestBody req_body;
    req_body.op = "subscribe";
    req_body.args = args;
    auto json_body = Common::DataSerializer<Detail::WsRequestBody>::write(req_body);
    auto json_body_str = boost::json::serialize(json_body);
    LOG(INFO) << "Subscribe request: " << json_body_str;
    co_await ws->write(json_body_str);
  }

  co_return 0;
}

boost::asio::awaitable<std::string> WsPublicApi::read_data() {
  if (m_trades_ws == nullptr) {
    co_await connect_trades();
  }

  try {
    co_return co_await m_trades_ws->read();
  } catch (const boost::beast::system_error& e) {
    LOG(ERROR) << "Error: " << e.code() << " msg: " << e.what();
    m_trades_ws = nullptr;
  }

  throw boost::system::system_error(
    static_cast<int>(ErrCode::Connect_Fail), MarketErrorCategory(), "Connect Fail");
}

awaitable<std::vector<std::shared_ptr<Market::TradeData>>> WsPublicApi::parse_data(const std::string& data) {
  auto respone_body = Common::DataReader<Detail::WsResponeSubscribe>::read_shared_ptr(boost::json::parse(data));
  auto res = std::vector<std::shared_ptr<Market::TradeData>>();
  for (auto item : respone_body->data) {
    res.push_back(mapping(item));
  }
  co_return res;
}

awaitable<std::string> WsPublicApi::read_depth_data() {
  if (m_depth_ws == nullptr) {
    co_await connect_depth();
  }

  try {
    co_return co_await m_depth_ws->read();
  } catch (const boost::beast::system_error& e) {
    LOG(ERROR) << "Error: " << e.code() << " msg: " << e.what();
    m_depth_ws = nullptr;
  }

  throw boost::system::system_error(
    static_cast<int>(ErrCode::Connect_Fail), MarketErrorCategory(), "Connect Fail");
}

awaitable<std::shared_ptr<DepthData>> WsPublicApi::parse_depth_data(const std::string& data) {
  auto respone_body = Common::DataReader<Detail::WsResponeDepth>::read_shared_ptr(boost::json::parse(data));
  if (respone_body->data.size() == 0) {
    co_return nullptr;
  }
  auto res = mapping(respone_body->data[0]);
  res->inst_id = respone_body->arg.instId;
  res->data_type = respone_body->action.empty()? "snapshot": respone_body->action;

  co_return res;
}

awaitable<int> WsPublicApi::depth_data_handle(std::shared_ptr<Market::DepthData> data) {
  LOG(INFO) << "Depth data handle: " << boost::json::serialize(Common::DataSerializer<decltype(data)>::write(data));
  co_return 0;
}

}  // namespace Okx
}  // namespace Market
