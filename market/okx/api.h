#ifndef __MARKET_OKX_API_H__
#define __MARKET_OKX_API_H__

#include <string>

namespace Market {
namespace Okx {
class API {
 public:
  API(const std::string& api_key, const std::string& secret_key, const std::string& passphrase);
  protected:
  std::string m_api_key;
  std::string m_secret_key;
  std::string m_passphrase;
  std::string genSingature(const std::string& timestamp, const std::string& method, const std::string& request_path, const std::string& body = "");
};

}  // namespace Okx

}  // namespace Market

#endif