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
  LOG(INFO) << fmt::format("run");
  co_await on_request_account();
  co_await on_request_position();
  co_await on_subscribe_book("BTC-USDT");
  co_await on_subscribe_tick("BTC-USDT");
  co_return;
}

asio::awaitable<void> Testing::recv_account(engine::AccountDataPtr account) {
  LOG(INFO) << fmt::format("recv_account: {}", account->balance.str());
  co_return;
}

asio::awaitable<void> Testing::recv_position(engine::PositionDataPtr position) {
  LOG(INFO) << fmt::format("recv_position: {}", position->items.size());
  for (auto& item : position->items) {
    LOG(INFO) << fmt::format("position: {}, {} {} {}", item->symbol, item->volume.str(), item->price.str(), int(item->direction));
  }
  co_return;
}

asio::awaitable<void> Testing::recv_book(engine::BookPtr order) {
  LOG(INFO) << fmt::format("recv_book {}: ask {} bid {}", order->symbol, order->asks.size(), order->bids.size());
  for (auto& item : order->asks) {
    LOG(INFO) << fmt::format("book ask {} {}", item.price.str(), item.volume.str());
  }

  for (auto& item : order->bids) {
    LOG(INFO) << fmt::format("book bid {} {}", item.price.str(), item.volume.str());
  }
  co_return;
}

asio::awaitable<void> Testing::recv_tick(engine::TickDataPtr ticker) {
  LOG(INFO) << fmt::format("recv_tick: {}", ticker->symbol);
  co_return;
}

}
