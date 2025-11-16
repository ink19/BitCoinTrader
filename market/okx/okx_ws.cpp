#include "okx_ws.h"
#include "data.hpp"
#include <glog/logging.h>

namespace market::okx {

OkxWs::OkxWs() {
  ws_ = std::make_shared<cpphttp::WebSocket>(url_);
}

OkxWs::~OkxWs() {}

asio::awaitable<void> OkxWs::connect() {
  co_await ws_->connect();
  co_return;
}

asio::awaitable<market::okx::WsMessage> OkxWs::read() {
  auto rsp = co_await ws_->read();
  auto msg = *jsoncpp::from_json<market::okx::WsMessage>(rsp);
  co_return msg;
}

}  // namespace market::okx
