#include "notice.h"


namespace notice::base {

Notice::Notice(engine::EnginePtr engine) : engine_(engine) {}

void Notice::init() {
  engine_->register_callback<engine::MessageData>(
    engine::EventType::kMessage, std::bind(&Notice::send_message, shared_from_this(), std::placeholders::_1));
}

asio::awaitable<void> Notice::on_log(engine::LogDataPtr log) {
  return engine_->on_event(engine::EventType::kLog, log);
}

}  // namespace notice::base
