#include "wework.h"

#include <cpphttp/request.h>
#include "utils/component.hpp"
#include <boost/json.hpp>
#include "utils/utils.h"

namespace notice::wework {

WeworkNotice::WeworkNotice(engine::EnginePtr engine, std::string key) : 
  notice::base::Notice(engine), m_key(key), m_uri(m_base_uri + key) {}

asio::awaitable<void> WeworkNotice::send_message(engine::MessageDataPtr msg) {
  auto send_obj = WeworkData();
  send_obj.text.content = msg->message;

  auto snd_msg = boost::json::serialize(jsoncpp::to_json(send_obj));

  ELOG_DEBUG("send msg req: {}", snd_msg);

  auto req = cpphttp::HttpRequest(
    m_uri, "POST", snd_msg
  );

  auto resp = co_await req.request();
  ELOG_DEBUG("send msg rsp: {}", resp);
  
  co_return;
}

asio::awaitable<void> WeworkNotice::run() {
  co_return;
}


}  // namespace notice::wework
