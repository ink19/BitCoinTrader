#include "engine.h"
#include "glog/logging.h"

namespace engine {

Engine::Engine(asio::io_context& ctx) : channel_(ctx, 10) {}

Engine::~Engine() {}

asio::awaitable<void> Engine::push_event(EventPtr event) {
  co_await channel_.async_send(boost::system::error_code(), event, asio::use_awaitable);
}

asio::awaitable<void> Engine::on_event(EventType etype, std::shared_ptr<BaseData> event) {
  co_await channel_.async_send(boost::system::error_code(), std::make_shared<Event>(etype, event), asio::use_awaitable);
}

asio::awaitable<void> Engine::run() {
  for (auto& component : components_) {
    asio::co_spawn(co_await asio::this_coro::executor, [component]() -> asio::awaitable<void> {
      component->init();
      co_await component->run();
    }, asio::detached);
  }

  LOG(INFO) << "Engine start";

  auto executor = co_await asio::this_coro::executor;
  while (true) {
    try {
      auto event = co_await channel_.async_receive(asio::use_awaitable);
      auto& callbacks = callbacks_[event->type];
      for (auto& callback : callbacks) {
        asio::co_spawn(executor, [&]() -> asio::awaitable<void> {
          try {
            co_await callback(event);
          } catch (boost::system::system_error &e) {
            LOG(ERROR) << fmt::format("Type {} callback error: {}", int(event->type), e.what());
          } catch (std::runtime_error &e) {
            LOG(ERROR) << fmt::format("Type {} callback error: {}", int(event->type), e.what());
          } catch (...) {
            LOG(ERROR) << fmt::format("Type {} callback error: unknown error", int(event->type)); 
          }
          co_return;
        }, asio::detached);
      }

      auto& all_callbacks = callbacks_[EventType::kAll];
      for (auto& callback : all_callbacks) {
        asio::co_spawn(executor, [&]() -> asio::awaitable<void> {
          co_await callback(event);
        }, asio::detached);
      }
    } catch (...) {
      continue;
    }
  }
}

void Engine::register_component(std::shared_ptr<Component> component) {
  components_.push_back(component);
}

}
