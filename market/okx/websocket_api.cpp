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
  auto ctx = co_await boost::asio::this_coro::executor;

  m_ws_api_private = std::make_unique<Common::WebSocket>("wss://ws.okx.com:8443/ws/v5/private");
  co_await m_ws_api_private->connect();
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
  co_await m_ws_api_private->write(boost::json::serialize(json_body));
  
  co_return ErrCode_OK;
}

boost::asio::awaitable<int> Market::Okx::WebSocketApi::subscribe(const std::string& channel, const std::string& instId) {
  if (m_ws_api_public.get() == nullptr) {
    co_await connect_public();
  }

  auto subscribe_param = std::make_shared<Detail::WsRequestArgsParamSubscribe>(channel, "", "", instId);
  m_public_subscribed_args.push_back(subscribe_param);


  Detail::WsRequestBody req_body(Detail::OpSUBSCRIBE, subscribe_param);
  auto json_body = req_body.Json();

  LOG(INFO) << "Subscribe request: " << boost::json::serialize(json_body);
  co_await m_ws_api_public->write(boost::json::serialize(json_body));

  co_return ErrCode_OK;
}

boost::asio::awaitable<int> Market::Okx::WebSocketApi::connect_public() {
  auto ctx = co_await boost::asio::this_coro::executor;
  m_ws_api_public = std::make_unique<Common::WebSocket>("ws://ws.okx.com:8443/ws/v5/public");
  co_await m_ws_api_public->connect();
  LOG(INFO) << "Connected to WebSocket server";

  if (m_public_subscribed_args.size() > 0) {
    for (auto& arg : m_public_subscribed_args) {
      Detail::WsRequestBody req_body(Detail::OpSUBSCRIBE, arg);
      auto json_body = req_body.Json();

      LOG(INFO) << "Subscribe request: " << boost::json::serialize(json_body);
      co_await m_ws_api_public->write(boost::json::serialize(json_body));
    }
  }

  co_return ErrCode_OK;
}

boost::asio::awaitable<typename std::shared_ptr<Market::Okx::Detail::WsResponeBody>> Market::Okx::WebSocketApi::read_private() {
  auto read_data= co_await m_ws_api_private->read();
  auto respone_body = std::make_shared<Detail::WsResponeBody>(read_data);
  co_return respone_body;
}

boost::asio::awaitable<std::shared_ptr<Market::Okx::Detail::WsResponeSubscribe>> Market::Okx::WebSocketApi::read_public() {
  auto read_data= co_await m_ws_api_public->read();
  auto respone_body = Common::DataReader<
    std::shared_ptr<Detail::WsResponeSubscribe>
  >::read(boost::json::parse(read_data));
  co_return respone_body;
}

boost::asio::awaitable<int> Market::Okx::WebSocketApi::keep_alive() {
  co_return ErrCode_OK;
}

boost::asio::awaitable<void> Market::Okx::WebSocketApi::exec() {
  for (;;) {
    try {
      auto read_result = co_await read_public();
      LOG(INFO) << "Received: " << Common::DataPrinter(*read_result);
      if (m_public_callback) {
        for (auto data : read_result->data) {
          co_await m_public_callback(data);
        }
      }
    } catch (const boost::beast::system_error& e) {
      LOG(ERROR) << "Error: " << e.code() << " msg: " << e.what();
      m_ws_api_public = nullptr;
    }

    if (m_ws_api_public.get() == nullptr) {
      co_await connect_public();
    }
  }
  co_return;
}
