#ifndef __NOTICE_WEWORK_WEWORK_API_H__
#define __NOTICE_WEWORK_WEWORK_API_H__

#include <string>
#include <boost/asio/awaitable.hpp>

namespace Notice {
namespace WeWork {

namespace asio = boost::asio;

class WeWorkText {
public:
  std::string content;
};

class WeWorkData {
public:
  std::string msgtype = "text";
  WeWorkText text;
};

class WeWorkAPI {
 public:
  WeWorkAPI(std::string key);
  asio::awaitable<int> send(std::string msg);

 private:
  std::string m_base_uri = "https://qyapi.weixin.qq.com/cgi-bin/webhook/send?key=";
  std::string m_key = "";
  std::string m_uri = "";
};

}  // namespace WeWork
}  // namespace Notice

#endif
