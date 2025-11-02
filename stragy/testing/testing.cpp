#include "testing.h"

#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include "utils/utils.h"

using namespace std::chrono_literals;

namespace stragy::testing {

Testing::Testing(engine::EnginePtr engine) : base::Stragy(engine) {}

Testing::~Testing() {}

asio::awaitable<void> Testing::run() {
  auto executor = co_await asio::this_coro::executor;
  ELOG_DEBUG("Testing start");
  for (int loop_i = 0; loop_i < 3; ++loop_i) {
    asio::steady_timer timer(executor, 1s);
    co_await timer.async_wait(asio::use_awaitable);
    ELOG_DEBUG("Testing loop {}", loop_i);
    co_await on_message(std::make_shared<engine::MessageData>("test"));
  }
  co_return;
}
}
