#include <fmt/core.h>
#include <glog/logging.h>

#include <boost/asio.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>

#include "websocket_api.h"
#include "request.h"

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
      // test for http
      Common::HttpRequest http_request("http://echo.free.beeceptor.com/", "POST", "Hello world");
      http_request.set_header("Test", "Test Header");
      auto respone = co_await http_request.request();
      LOG(INFO) << "HTTP Response: " << respone;

      co_await ws_api.login();
      LOG(INFO) << "Send Login successful";

      boost::asio::co_spawn(io_context, ws_api.exec(), boost::asio::detached);
    } catch (const std::exception& e) {
      LOG(ERROR) << "Error: " << e.what();
    }
  }, boost::asio::detached);

  try {
    io_context.run();
  } catch (const std::exception& e) {
    LOG(ERROR) << "Error: " << e.what();
  }

  google::ShutdownGoogleLogging();
  return 0;
}
