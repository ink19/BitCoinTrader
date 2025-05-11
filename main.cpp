#include <fmt/core.h>
#include <glog/logging.h>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system.hpp>
#include <boost/system/system_error.hpp>

#include "binance/ws_vision.h"
#include "compare/compare.h"
#include "config.h"
#include "options.h"
#include "wework/wework.h"
#include "okx/ws_public_api.h"

int main(int argc, char* argv[]) {
  AppOptions(argc, argv);

  google::InitGoogleLogging(argv[0]);
  FLAGS_minloglevel = google::INFO;
  FLAGS_logtostderr = true;

  LOG(INFO) << "CONFIG FILE: " << AppOptions.config_file();
  AppConfig.init(AppOptions.config_file());

  std::shared_ptr<Market::PublicTradeApi> ws_api = std::make_shared<Market::Okx::WsPublicApi>();
  std::shared_ptr<Market::PublicTradeApi> binance_api = std::make_shared<Market::Binance::BinanceVisionAPI>();
  std::shared_ptr<Market::PublicDepthApi> okx_depth_api = std::make_shared<Market::Okx::WsPublicApi>();
  std::shared_ptr<Market::PublicDepthApi> binance_depth_api = std::make_shared<Market::Binance::BinanceVisionAPI>();

  auto notice_api = std::make_shared<Notice::WeWork::WeWorkAPI>(AppConfig.wework()->key());
  boost::asio::io_context io_context;

  Service::Compare::CompareSerivce s(binance_api, ws_api, notice_api);

  std::vector<std::string> inst_ids = {};
  boost::algorithm::split(inst_ids, AppOptions.coin(), boost::is_any_of(","));
  for (auto& inst_id : inst_ids) {
    inst_id = fmt::format("{}-USDT", boost::to_upper_copy(inst_id));
  }

  for (auto& inst_id : inst_ids) {
    binance_api->subscribe_trades("trades", inst_id);
    ws_api->subscribe_trades("trades", inst_id);
    okx_depth_api->subscribe_depth("books5", inst_id);
    binance_depth_api->subscribe_depth("books5", inst_id);
  }

  boost::asio::co_spawn(
      io_context,
      [&]() -> boost::asio::awaitable<void> {
        try {
          // boost::asio::co_spawn(io_context, binance_api->exec(), boost::asio::detached);
          // boost::asio::co_spawn(io_context, ws_api->exec(), boost::asio::detached);
          // boost::asio::co_spawn(io_context, s.run(), boost::asio::detached);
          boost::asio::co_spawn(io_context, okx_depth_api->exec(), boost::asio::detached);
          boost::asio::co_spawn(io_context, binance_depth_api->exec(), boost::asio::detached);
        } catch (const boost::system::system_error& e) {
          LOG(ERROR) << "System Error: " << e.what();
        } catch (const std::exception& e) {
          LOG(ERROR) << "Error: " << e.what();
        }
        co_return;
      },
      boost::asio::detached);

  io_context.run();

  LOG(INFO) << "END";

  google::ShutdownGoogleLogging();
  return 0;
}
