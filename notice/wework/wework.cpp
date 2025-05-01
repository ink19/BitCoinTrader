#include "wework.h"

#include "request.h"
#include "component.hpp"
#include <boost/json.hpp>

namespace Notice {
namespace WeWork {

WeWorkAPI::WeWorkAPI(std::string key) : m_key(key), m_uri(m_base_uri + key) {}

asio::awaitable<int> WeWorkAPI::send(std::string msg) {
  auto send_obj = WeWorkData();
  send_obj.text.content = msg;

  auto req = Common::HttpRequest(
    m_uri, "POST", boost::json::serialize(Common::DataSerializer<WeWorkData>::write(send_obj))
  );

  auto resp = co_await req.request();

  co_return 0;
}


}  // namespace WeWork
}  // namespace Notice
