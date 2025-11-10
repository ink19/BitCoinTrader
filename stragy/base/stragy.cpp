#include "stragy.h"

namespace stragy::base {

Stragy::Stragy(engine::EnginePtr engine) : _engine(engine) {
}

Stragy::~Stragy() {}

void Stragy::init() {
  _engine->register_callback<engine::AccountData>(engine::EventType::kAccount,
    std::bind(&Stragy::recv_account, shared_from_this(), std::placeholders::_1));

  _engine->register_callback<engine::PositionData>(engine::EventType::kPosition,
    std::bind(&Stragy::recv_position, shared_from_this(), std::placeholders::_1));
}

asio::awaitable<void> Stragy::on_message(engine::MessageDataPtr msg) {
  return _engine->on_event(engine::EventType::kMessage, msg);
}

asio::awaitable<void> Stragy::on_request_account() {
  return _engine->on_event(engine::EventType::kQueryAccount, std::make_shared<engine::QueryAccountData>());
}

asio::awaitable<void> Stragy::on_request_position() {
  return _engine->on_event(engine::EventType::kQueryPosition, std::make_shared<engine::QueryPositionData>());
}

}
