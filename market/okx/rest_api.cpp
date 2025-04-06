#include "rest_api.h"
#include "utils.h"
#include "request.h"
#include <boost/json.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/system.hpp>
#include <boost/system/system_error.hpp>
#include <memory>
#include "errcode.h"
#include <fmt/format.h>
#include <glog/logging.h>

namespace Market {
namespace Okx {

namespace asio = boost::asio;
namespace bs = boost::system;

RestApi::RestApi(const std::string& api_key, const std::string& secret_key, const std::string& passphrase)
    : API(api_key, secret_key, passphrase) {}

std::map<std::string, std::string> RestApi::prepare_headers(const std::string& method, const std::string& path) {
  std::map<std::string, std::string> headers;
  int64_t ts = Common::get_current_time_s();
  std::string ts_str = Common::time_format_iso(ts);
  
  std::string signature = genSingature(ts_str, method, path);

  headers["OK-ACCESS-KEY"] = m_api_key;
  headers["OK-ACCESS-SIGN"] = signature;
  headers["OK-ACCESS-TIMESTAMP"] = ts_str;
  headers["OK-ACCESS-PASSPHRASE"] = m_passphrase;
  // headers["Content-Type"] = "application/json";

  return headers;
}

asio::awaitable<std::shared_ptr<Detail::RestResponeDataAccountBalance>> RestApi::get_account_balance() {
  const std::string path = "/api/v5/account/balance";
  const std::string method = "GET";
  const std::string path_with_query = path + "?ccy=ETH";
  const std::string request_uri = base_url + path_with_query;

  auto headers = prepare_headers(method, path_with_query);
  Common::HttpRequest handler(request_uri, method);
  handler.set_header(headers);

  auto response_body = co_await handler.request();

  LOG(INFO) << "Response: " << response_body;

  auto json_data = boost::json::parse(response_body);

  auto respone = Detail::RestRespone(json_data);
  if (respone.code != 0) {
    LOG(ERROR) << "Error: " << respone.code << " - " << respone.msg;
    throw bs::system_error(
        bs::error_code(static_cast<int>(ErrCode_Invalid_Param), bs::generic_category()),
        fmt::format("Error: {} - {}", respone.code, respone.msg));
  }

  if (respone.data.empty()) {
    LOG(ERROR) << "No data found in response";
    throw bs::system_error(
        bs::error_code(static_cast<int>(ErrCode_Invalid_Param), bs::generic_category()),
        "No data found in response");
  }

  auto account_balance = std::dynamic_pointer_cast<Detail::RestResponeDataAccountBalance>(respone.data[0]);

  co_return account_balance;
}

}  // namespace Okx
}  // namespace Market
