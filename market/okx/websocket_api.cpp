#include "websocket_api.h"
#include "utils.h"
#include <glog/logging.h>
#include <boost/asio/awaitable.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/core/span.hpp>
#include <memory>
#include "errcode.h"
#include "websocket_api_detail.h"

// 构造函数
Market::Okx::WebSocketApi::WebSocketApi(const std::string& api_key, const std::string& secret_key,
                                        const std::string& passphrase)
    : API(api_key, secret_key, passphrase) {}

boost::asio::awaitable<int> Market::Okx::WebSocketApi::login() {
  auto ctx = co_await boost::asio::this_coro::executor;

  m_ws_api_detail = std::make_unique<Common::WebSocket>("wss://ws.okx.com:8443/ws/v5/private");
  co_await m_ws_api_detail->connect();
  LOG(INFO) << "Connected to WebSocket server";

  int64_t ts = Common::get_current_time_s();
  std::string ts_str = std::to_string(ts);

  LOG(INFO) << "m_api_key: " << m_api_key << " m_passphrase: " << m_passphrase
            << " ts_str: " << ts_str;
  auto login_param = std::make_shared<Detail::WsRequestArgsParamLogin>(m_api_key, m_passphrase,
                                        genSingature(ts_str, "GET", "/users/self/verify"), ts_str);

  Detail::WsRequestBody req_body(Detail::OpLOING, login_param);
  auto json_body = req_body.Json();

  LOG(INFO) << "Login request: " << boost::json::serialize(json_body);
  co_await m_ws_api_detail->write(boost::json::serialize(json_body));
  
  co_return ErrCode_OK;
}

boost::asio::awaitable<typename std::shared_ptr<Market::Okx::Detail::WsResponeBody>> Market::Okx::WebSocketApi::read() {
  auto read_data= co_await m_ws_api_detail->read();
  auto respone_body = std::make_shared<Detail::WsResponeBody>(read_data);
  co_return respone_body;
}

boost::asio::awaitable<void> Market::Okx::WebSocketApi::exec() {
  for (;;) {
    try {
      auto read_result = co_await read();
      if (read_result.get() == nullptr) {
        LOG(ERROR) << "Read error";
        co_return;
      }
      LOG(INFO) << "Received: " << read_result;
    } catch (const boost::beast::system_error& e) {
      LOG(ERROR) << "Error: " << e.code() << " msg: " << e.what();
      co_return;
    }
  }
  co_return;
} 
