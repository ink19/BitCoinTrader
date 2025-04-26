#include <fmt/core.h>
#include <glog/logging.h>

#include <boost/asio.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system.hpp>
#include <boost/system/system_error.hpp>

#include "component.hpp"
#include "config.h"
#include "options.h"
#include "request.h"
#include "rest_api.h"
#include "websocket_api.h"
#include "record.h"

int main(int argc, char* argv[]) {
  AppOptions(argc, argv);

  google::InitGoogleLogging(argv[0]);
  FLAGS_minloglevel = google::INFO;
  FLAGS_logtostderr = true;

  LOG(INFO) << "CONFIG FILE: " << AppOptions.config_file();
  AppConfig.init(AppOptions.config_file());

  Market::Okx::WebSocketApi ws_api(AppConfig.okx()->api_key(), AppConfig.okx()->secret_key(),
                                   AppConfig.okx()->passphrase());
  boost::asio::io_context io_context;

  Record::Record record("record_file");
  auto ws_api_callback = [&record](std::shared_ptr<Market::Okx::Detail::WsResponeSubscribeData> data) -> boost::asio::awaitable<int> { 
    co_return co_await record.write(data);
  };
  ws_api.set_public_callback(ws_api_callback);


  boost::asio::co_spawn(
      io_context,
      [&]() -> boost::asio::awaitable<void> {
        try {
          co_await ws_api.subscribe("trades", "ETH-USDT");

          boost::asio::co_spawn(io_context, ws_api.exec(), boost::asio::detached);
        } catch (const boost::system::system_error& e) {
          LOG(ERROR) << "System Error: " << e.what();
        } catch (const std::exception& e) {
          LOG(ERROR) << "Error: " << e.what();
        }
      },
      boost::asio::detached);

  io_context.run();

  LOG(INFO) << "END";

  google::ShutdownGoogleLogging();
  return 0;
}
