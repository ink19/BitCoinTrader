#ifndef __MARKET_BASE_ENGINE_H__
#define __MARKET_BASE_ENGINE_H__

#include <boost/asio/experimental/concurrent_channel.hpp>
#include <memory>
#include <string>
#include "utils/utils.h"
#include "object.h"
#include <map>
#include <set>
#include <glog/logging.h>

namespace engine {

class Component {
public:
  virtual asio::awaitable<void> run() = 0;
  virtual void init() = 0;
};

// 事件回调
typedef std::function<asio::awaitable<void>(EventPtr)> EventCallback;

class Engine {
public:
  Engine(asio::io_context& ctx);
  ~Engine();

  asio::awaitable<void> run();

  asio::awaitable<void> push_event(EventPtr event);

  asio::awaitable<void> on_event(EventType etype, std::shared_ptr<BaseData> event);

  template<typename EventDataType>
  void register_callback(EventType type, std::function<asio::awaitable<void>(std::shared_ptr<EventDataType>)> callback) {
    callbacks_[type].push_back([callback](EventPtr event) -> asio::awaitable<void> {
      auto data = std::dynamic_pointer_cast<EventDataType>(event->data);
      co_await callback(data);
      co_return;
    });
  }

  void register_component(std::shared_ptr<Component> component);
  
private:
  boost::asio::experimental::concurrent_channel<void(boost::system::error_code, EventPtr)> channel_;
  std::map<EventType, std::vector<EventCallback>> callbacks_;
  std::vector<std::shared_ptr<Component>> components_;
};

typedef std::shared_ptr<Engine> EnginePtr;

}  // namespace engine

#endif  // __MARKET_BASE_ENGINE_H__
