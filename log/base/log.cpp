#include "log.h"

namespace elog::base {
Baselog::Baselog(engine::EnginePtr engine) : engine_(engine) {}

void Baselog::init() {
  engine_->register_callback<engine::LogData>(engine::EventType::kLog,
                                              std::bind(&Baselog::write_log, this, std::placeholders::_1));
}

Baselog::~Baselog() {}

asio::awaitable<void> Baselog::write_log(engine::LogDataPtr log_data) {
  switch (log_data->level) {
    case engine::LogLevel::kError:
      LOG(ERROR) << log_data->log;
    case engine::LogLevel::kWarning:
      LOG(WARNING) << log_data->log;
    case engine::LogLevel::kInfo:
      LOG(INFO) << log_data->log;
    case engine::LogLevel::kDebug:
      LOG(INFO) << log_data->log;
  }
  co_return;
}

asio::awaitable<void> Baselog::run() { co_return; }

}  // namespace elog::base
