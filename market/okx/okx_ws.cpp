#include "okx_ws.h"
#include "data.hpp"
#include <glog/logging.h>

namespace market::okx {

// 初始化WebSocket客户端，连接到OKX的WebSocket服务器
OkxWs::OkxWs() {
  ws_ = std::make_shared<cpphttp::WebSocket>(url_);
}

OkxWs::~OkxWs() {}

// 连接到WebSocket服务器
asio::awaitable<void> OkxWs::connect() {
  co_await ws_->connect();
  co_return;
}

// 从WebSocket读取消息并解析为WsMessage结构
asio::awaitable<market::okx::WsMessage> OkxWs::read() {
  // 读取原始JSON数据
  auto rsp = co_await ws_->read();
  // 解析JSON为WsMessage结构体
  auto msg = *jsoncpp::from_json<market::okx::WsMessage>(rsp);
  co_return msg;
}

}  // namespace market::okx
