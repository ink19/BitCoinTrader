#include <fmt/core.h>
#include <glog/logging.h>

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system.hpp>
#include <boost/system/system_error.hpp>

#include "binance/ws_vision.h"
#include "compare/compare.h"
#include "config.h"
#include "okx/websocket_api.h"
#include "options.h"
#include "wework/wework.h"

int main(int argc, char* argv[]) {
  AppOptions(argc, argv);

  google::InitGoogleLogging(argv[0]);
  FLAGS_minloglevel = google::INFO;
  FLAGS_logtostderr = true;

  LOG(INFO) << "CONFIG FILE: " << AppOptions.config_file();
  AppConfig.init(AppOptions.config_file());

  auto ws_api = std::make_shared<Market::Okx::WebSocketApi>(AppConfig.okx()->api_key(), AppConfig.okx()->secret_key(),
                                                            AppConfig.okx()->passphrase());
  auto binance_api = std::make_shared<Market::Binance::BinanceVisionAPI>();
  auto notice_api = std::make_shared<Notice::WeWork::WeWorkAPI>(AppConfig.wework()->key());
  boost::asio::io_context io_context;

  Service::Compare::CompareSerivce s(binance_api, ws_api, notice_api);

  boost::asio::co_spawn(
      io_context,
      [&]() -> boost::asio::awaitable<void> {
        try {
          co_await binance_api->subscribe("aggTrade", fmt::format("{}usdt", boost::to_lower_copy(AppOptions.coin())));
          co_await ws_api->subscribe("trades", fmt::format("{}-USDT", boost::to_upper_copy(AppOptions.coin())));

          boost::asio::co_spawn(io_context, binance_api->exec(), boost::asio::detached);
          boost::asio::co_spawn(io_context, ws_api->exec(), boost::asio::detached);
          boost::asio::co_spawn(io_context, s.run(), boost::asio::detached);
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
