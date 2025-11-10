#include <fmt/core.h>
#include <glog/logging.h>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/system.hpp>
#include <boost/system/system_error.hpp>

#include "config/config.h"
#include "config/options.h"
#include "wework/wework.h"
#include "testing/testing.h"
#include "okx/okx.h"

int main(int argc, char* argv[]) {
  AppOptions(argc, argv);

  google::InitGoogleLogging(argv[0]);
  FLAGS_minloglevel = google::INFO;
  FLAGS_logtostderr = true;

  LOG(INFO) << "CONFIG FILE: " << AppOptions.config_file();
  AppConfig.init(AppOptions.config_file());

  boost::asio::io_context io_context;
  auto engine = std::make_shared<engine::Engine>(io_context);

  auto wework = std::make_shared<notice::wework::WeworkNotice>(engine, AppConfig.wework()->key());
  auto testing = std::make_shared<stragy::testing::Testing>(engine);
  auto okx = std::make_shared<market::okx::Okx>(engine,
    AppConfig.okx()->api_key(), AppConfig.okx()->secret_key(), AppConfig.okx()->passphrase());

  engine->register_component(wework);
  engine->register_component(testing);
  engine->register_component(okx);

  asio::co_spawn(io_context, engine->run(), asio::detached);

  io_context.run();

  google::ShutdownGoogleLogging();
  return 0;
}
