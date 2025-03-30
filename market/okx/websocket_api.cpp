#include "websocket_api.h"
#include "utils.h"
#include <glog/logging.h>
#include <boost/asio/awaitable.hpp>
#include <boost/core/span.hpp>
#include <memory>
#include "errcode.h"
#include "websocket_api_detail.hpp"

// 构造函数
Market::Okx::WebSocketApi::WebSocketApi(const std::string& api_key, const std::string& secret_key,
                                        const std::string& passphrase)
    : m_api_key(api_key), m_secret_key(secret_key), m_passphrase(passphrase) {}

// 生成签名
std::string Market::Okx::WebSocketApi::genSingature(const std::string& timestamp, const std::string& method,
                                                    const std::string& request_path) {
  // 拼接字符串
  std::string str_to_sign = timestamp + method + request_path;

  // 计算SHA-256哈希值
  std::string hash = Common::sha256_hash_base64(str_to_sign, m_secret_key);

  // 返回Base64编码的哈希值
  return hash;
}

boost::asio::awaitable<int> Market::Okx::WebSocketApi::login() {
  auto ctx = co_await boost::asio::this_coro::executor;

  m_ws_api_detail = std::make_unique<Common::WebSocket>("wss://ws.okx.com:8443/ws/v5/private");
  co_await m_ws_api_detail->connect();
  LOG(INFO) << "Connected to WebSocket server";

  int64_t ts = Common::get_current_time_s();
  std::string ts_str = std::to_string(ts);
  auto login_param = std::make_shared<Detail::RequestArgsParamLogin>(m_api_key, m_passphrase,
                                        genSingature(ts_str, "GET", "/users/self/verify"), ts_str);

  Detail::RequestBody req_body(Detail::OpLOING, login_param);
  auto json_body = req_body.Json();

  LOG(INFO) << "Login request: " << boost::json::serialize(json_body);
  co_await m_ws_api_detail->write(boost::json::serialize(json_body));

  co_return ErrCode_OK;
}

boost::asio::awaitable<std::string> Market::Okx::WebSocketApi::read() {
  co_return co_await m_ws_api_detail->read();
}

boost::asio::awaitable<void> Market::Okx::WebSocketApi::exec() {
  for (;;) {
    auto read_result = co_await read();
    if (read_result.empty()) {
      LOG(ERROR) << "Read error";
      co_return;
    }
    LOG(INFO) << "Received: " << read_result;
  }
  co_return;
} 
