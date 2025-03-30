#include <fmt/core.h>
#include <glog/logging.h>

#include <boost/asio.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>

#include "WebSocket.h"
#include "utils.h"
#include "websocket_api.h"
#include "config.h"
#include "options.h"

int main(int argc, char* argv[]) {
  AppOptions(argc, argv);

  google::InitGoogleLogging(argv[0]);
  FLAGS_minloglevel = google::INFO;
  FLAGS_logtostderr = true;

  LOG(INFO) << "CONFIG FILE: " << AppOptions.config_file();
  AppConfig.init(AppOptions.config_file());

  Market::Okx::WebSocketApi ws_api(AppConfig.okx()->api_key(), AppConfig.okx()->secret_key(), AppConfig.okx()->passphrase());
  boost::asio::io_context io_context;

  boost::asio::co_spawn(io_context, [&]() -> boost::asio::awaitable<void> {
    try {
      co_await ws_api.login();
      LOG(INFO) << "Send Login successful";

      boost::asio::co_spawn(io_context, ws_api.exec(), boost::asio::detached);
    } catch (const std::exception& e) {
      LOG(ERROR) << "Error: " << e.what();
    }
  }, boost::asio::detached);


  io_context.run();

  google::ShutdownGoogleLogging();
  return 0;
}
