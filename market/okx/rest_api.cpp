#include "rest_api.h"
#include "utils.h"

namespace Market {
namespace Okx {

RestApi::RestApi(const std::string& api_key, const std::string& secret_key, const std::string& passphrase)
    : API(api_key, secret_key, passphrase) {}

std::map<std::string, std::string> RestApi::prepare_headers(const std::string& method, const std::string& path) {
  std::map<std::string, std::string> headers;
  int64_t ts = Common::get_current_time_s();
  std::string ts_str = std::to_string(ts);
  std::string signature = genSingature(ts_str, method, path);

  headers["OK-ACCESS-KEY"] = m_api_key;
  headers["OK-ACCESS-SIGN"] = signature;
  headers["OK-ACCESS-TIMESTAMP"] = ts_str;
  headers["OK-ACCESS-PASSPHRASE"] = m_passphrase;
  headers["Content-Type"] = "application/json";

  return headers;
}

}  // namespace Okx
}  // namespace Market
