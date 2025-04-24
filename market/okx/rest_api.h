#ifndef __MARKET_OKX_REST_API_H__
#define __MARKET_OKX_REST_API_H__

#include <string>
#include <map>

#include "api.h"
#include "rest_api_detail.h"
#include <boost/asio/awaitable.hpp>

namespace Market {
namespace Okx {

namespace asio = boost::asio;



class RestApi : private API {
public:
  RestApi(const std::string& api_key, const std::string& secret_key,
    const std::string& passphrase);
  
    asio::awaitable<std::shared_ptr<Detail::RestResponeDataAccountBalance>> get_account_balance();
    asio::awaitable<std::shared_ptr<Detail::RestResponeDataAccountInstrument>> get_account_instruments(Detail::InstTypeEnum instType);

private:
  std::map<std::string, std::string> prepare_headers(const std::string& method, const std::string& path);
  std::string base_url = "https://www.okx.com";
};

} // namespace Okx

} // namespace Market

#endif
