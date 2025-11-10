#ifndef __STRAGY_BASE_STRAGY_H__
#define __STRAGY_BASE_STRAGY_H__

#include "utils/utils.h"
#include "engine.h"

namespace stragy {
namespace base {

class Stragy : public std::enable_shared_from_this<Stragy>, public engine::Component {
public:
  Stragy(engine::EnginePtr engine);
  ~Stragy();

  void init() override;

  asio::awaitable<void> on_message(engine::MessageDataPtr msg);
  asio::awaitable<void> on_request_account();
  asio::awaitable<void> on_request_position();

  virtual asio::awaitable<void> recv_account(engine::AccountDataPtr account) = 0;
  virtual asio::awaitable<void> recv_position(engine::PositionDataPtr position) = 0;

private:
  engine::EnginePtr _engine;
};

}  // namespace base
}  // namespace stragy

#endif  // __STRAGY_BASE_STRAGY_H__
