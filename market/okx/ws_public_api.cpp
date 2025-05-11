#include "ws_public_api.h"

#include <glog/logging.h>

#include <memory>
#include <sstream>
#include <boost/crc.hpp>

#include "component.hpp"
#include "errcode.h"

namespace Market {
namespace Okx {

// 构造函数
WsPublicApi::WsPublicApi(int retry_times) : PublicTradeApi(retry_times) {}

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

boost::asio::awaitable<int> WsPublicApi::connect_depth() { co_return co_await connect(m_depth_ws, m_subscribed_depth); }

awaitable<int> WsPublicApi::connect(std::unique_ptr<Common::WebSocket>& ws,
                                    const std::vector<std::shared_ptr<Detail::WsRequestArgsParamSubscribe>>& args) {
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

  throw boost::system::system_error(static_cast<int>(ErrCode::Connect_Fail), MarketErrorCategory(), "Connect Fail");
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

  throw boost::system::system_error(static_cast<int>(ErrCode::Connect_Fail), MarketErrorCategory(), "Connect Fail");
}

awaitable<std::shared_ptr<DepthData>> WsPublicApi::parse_depth_data(const std::string& data) {
  auto respone_body = Common::DataReader<Detail::WsResponeDepth>::read_shared_ptr(boost::json::parse(data));
  if (respone_body->data.size() == 0) {
    co_return nullptr;
  }
  auto depth_info = respone_body->data[0];
  auto res = mapping(respone_body->data[0]);
  res->inst_id = respone_body->arg.instId;
  res->data_type = respone_body->action.empty() ? DepthDataTypeSnapshot : respone_body->action;

  // 检查序列号
  if (depth_info->prevSeqId == -1) {
    res->data_type = DepthDataTypeSnapshot;
  }

  if (depth_info->prevSeqId == depth_info->seqId) {  // 没有更新
    co_return m_depth->depth;
  }

  if (m_depth == nullptr) {
    m_depth = std::make_shared<OkxDepthData>();
  }

  m_depth->seqId = depth_info->seqId;
  if (res->data_type == DepthDataTypeSnapshot) {
    m_depth->depth = res;
  } else {
    if (m_depth->depth == nullptr) {
      m_depth->depth = std::make_shared<DepthData>();
    }

    m_depth->depth->ts = res->ts;

    for (auto& item : res->asks) {
      m_depth->depth->asks.push_back(item);
    }

    for (auto& item : res->bids) {
      m_depth->depth->bids.push_back(item);
    }
    // 排序
    std::sort(m_depth->depth->asks.begin(), m_depth->depth->asks.end(),
              [](const auto& a, const auto& b) { return a->price < b->price; });

    std::sort(m_depth->depth->bids.begin(), m_depth->depth->bids.end(),
              [](const auto& a, const auto& b) { return a->price > b->price; });

    // 合并相同价格
    for (auto it = m_depth->depth->asks.begin(); it != m_depth->depth->asks.end();) {
      auto it2 = it + 1;
      while (it2 != m_depth->depth->asks.end() && (*it)->price == (*it2)->price) {
        (*it)->size += (*it2)->size;
        it2 = m_depth->depth->asks.erase(it2);
      }
      it = it2;
    }

    for (auto it = m_depth->depth->bids.begin(); it != m_depth->depth->bids.end();) {
      auto it2 = it + 1;
      while (it2 != m_depth->depth->bids.end() && (*it)->price == (*it2)->price) {
        (*it)->size += (*it2)->size;
        it2 = m_depth->depth->bids.erase(it2);
      }
      it = it2;
    }

    // 撮合交易
    while (!m_depth->depth->asks.empty() && !m_depth->depth->bids.empty() &&
           m_depth->depth->asks[0]->price > m_depth->depth->bids[0]->price) {
      auto& ask = m_depth->depth->asks[0];
      auto& bid = m_depth->depth->bids[0];
      if (ask->size > bid->size) {
        ask->size -= bid->size;
        m_depth->depth->bids.erase(m_depth->depth->bids.begin());
      } else {
        bid->size -= ask->size;
        m_depth->depth->asks.erase(m_depth->depth->asks.begin());
      }
    }
  }

  // 校验数据
  if (depth_info->checksum != 0) {
    depth_checksum(depth_info->checksum); //TODO
  }

  co_return m_depth->depth;
}

bool WsPublicApi::depth_checksum(int checksum) {
  auto check_bids_end_iter =
      m_depth->depth->bids.size() > 25 ? m_depth->depth->bids.begin() + 25 : m_depth->depth->bids.end();
  auto check_asks_end_iter =
      m_depth->depth->asks.size() > 25 ? m_depth->depth->asks.begin() + 25 : m_depth->depth->asks.end();
  
  std::stringstream check_strstream;
  auto check_bids_iter = m_depth->depth->bids.begin();
  auto check_asks_iter = m_depth->depth->asks.begin();

  while(check_bids_iter != check_bids_end_iter && check_asks_iter != check_asks_end_iter) {
    if (check_bids_iter != check_bids_end_iter) {
      check_strstream << (*check_bids_iter)->price << ":" << (*check_bids_iter)->size << ":";
      ++check_bids_iter;
    }

    if (check_asks_iter != check_asks_end_iter) {
      check_strstream << (*check_asks_iter)->price << ":" << (*check_asks_iter)->size << ":";
      ++check_asks_iter;
    }
  }

  auto check_str = check_strstream.str();
  
  boost::crc_32_type crc32;
  crc32.process_bytes(check_str.data(), check_str.size() - 1);

  if (static_cast<int32_t>(crc32.checksum()) != checksum) {
    LOG(INFO) << "Depth data check fail: " << check_str << " checksum: " << crc32.checksum() << " expect: " << checksum;
    return false;
  }
  return true;
}

awaitable<int> WsPublicApi::depth_data_handle(std::shared_ptr<Market::DepthData> data) {
  // LOG(INFO) << "Depth data handle: " << boost::json::serialize(Common::DataSerializer<decltype(data)>::write(data));
  co_return 0;
}

}  // namespace Okx
}  // namespace Market
