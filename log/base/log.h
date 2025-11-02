#ifndef __LOG_BASE_BASE_H__
#define __LOG_BASE_BASE_H__

#include <string>
#include "engine.h"
#include "glog/logging.h"

namespace elog::base {

class Baselog : public std::enable_shared_from_this<Baselog>, public engine::Component {
public:
  Baselog(engine::EnginePtr engine);
  void init() override;
  virtual ~Baselog();

  virtual asio::awaitable<void> write_log(engine::LogDataPtr log_data);

  virtual asio::awaitable<void> run() override;

protected:
  engine::EnginePtr engine_;
};

} // namespace log

#endif // __LOG_BASE_BASE_H__
