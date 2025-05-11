#include "websocket_api.h"
#include "utils.h"
#include <glog/logging.h>
#include <boost/asio/awaitable.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/core/span.hpp>
#include <memory>
#include "errcode.h"
#include "websocket_api_detail.h"
#include "component.hpp"
#include <boost/asio/steady_timer.hpp>

// 构造函数
Market::Okx::WebSocketApi::WebSocketApi(const std::string& api_key, const std::string& secret_key,
                                        const std::string& passphrase)
    : API(api_key, secret_key, passphrase) {}

boost::asio::awaitable<int> Market::Okx::WebSocketApi::login() {
  // auto ctx = co_await boost::asio::this_coro::executor;

  // m_ws_api_private = std::make_unique<Common::WebSocket>("wss://ws.okx.com:8443/ws/v5/private");
  // co_await m_ws_api_private->connect();
  // LOG(INFO) << "Connected to WebSocket server";

  // int64_t ts = Common::get_current_time_s();
  // std::string ts_str = std::to_string(ts);

  // LOG(INFO) << "m_api_key: " << m_api_key << " m_passphrase: " << m_passphrase
  //           << " ts_str: " << ts_str;
  // auto login_param = std::make_shared<Detail::WsRequestArgsParamLogin>(m_api_key, m_passphrase,
  //                                       genSingature(ts_str, "GET", "/users/self/verify"), ts_str);

  // Detail::WsRequestBody req_body(Detail::OpLOING, login_param);
  // auto json_body = req_body.Json();

  // LOG(INFO) << "Login request: " << boost::json::serialize(json_body);
  // co_await m_ws_api_private->write(boost::json::serialize(json_body));
  
  co_return 0;
}
