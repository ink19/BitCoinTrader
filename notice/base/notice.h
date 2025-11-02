#ifndef BITCOINTRADER_NOTICE_BASE_NOTICE_H_
#define BITCOINTRADER_NOTICE_BASE_NOTICE_H_

#include "engine.h"
#include "utils/utils.h"

namespace notice::base {

class Notice : public std::enable_shared_from_this<Notice>, public engine::Component{
public:
  Notice(engine::EnginePtr engine);
  void init() override;

  asio::awaitable<void> on_log(engine::LogDataPtr log);

  virtual asio::awaitable<void> send_message(engine::MessageDataPtr msg) = 0;
private:
  engine::EnginePtr engine_;
};

}

#endif  // BITCOINTRADER_NOTICE_BASE_NOTICE_H_
