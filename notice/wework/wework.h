#ifndef __NOTICE_WEWORK_WEWORK_API_H__
#define __NOTICE_WEWORK_WEWORK_API_H__

#include <string>
#include <boost/asio/awaitable.hpp>

#include "base/notice.h"

namespace notice {
namespace wework {

namespace asio = boost::asio;

class WeworkText {
public:
  std::string content;
};

class WeworkData {
public:
  std::string msgtype = "text";
  WeworkText text;
};

class WeworkNotice : public notice::base::Notice {
 public:
  WeworkNotice(engine::EnginePtr engine, std::string key);
  asio::awaitable<void> send_message(engine::MessageDataPtr msg) override;
  asio::awaitable<void> run() override;
 private:
  std::string m_base_uri = "https://qyapi.weixin.qq.com/cgi-bin/webhook/send?key=";
  std::string m_key = "";
  std::string m_uri = "";
};

}  // namespace WeWork
}  // namespace Notice

#endif
