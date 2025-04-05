#include <fmt/core.h>
#include <glog/logging.h>

#include <boost/asio.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>

#include "websocket_api.h"
#include "rest_api.h"
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
  Market::Okx::RestApi rest_api(AppConfig.okx()->api_key(), AppConfig.okx()->secret_key(), AppConfig.okx()->passphrase());


  boost::asio::io_context io_context;

  boost::asio::co_spawn(io_context, [&]() -> boost::asio::awaitable<void> {
    try {
      // test for http
      auto account_balance = co_await rest_api.get_account_balance();
      LOG(INFO) << "Account Balance: " << account_balance;

      // test for ws
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
