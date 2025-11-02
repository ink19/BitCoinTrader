#include "stragy.h"

namespace stragy::base {

Stragy::Stragy(engine::EnginePtr engine) : _engine(engine) {
}

Stragy::~Stragy() {}

void Stragy::init() {}

asio::awaitable<void> Stragy::on_log(engine::LogDataPtr log) {
  return _engine->on_event(engine::EventType::kLog, log);
}

asio::awaitable<void> Stragy::on_message(engine::MessageDataPtr msg) {
  return _engine->on_event(engine::EventType::kMessage, msg);
}

}
