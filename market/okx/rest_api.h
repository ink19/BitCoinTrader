#ifndef __MARKET_OKX_REST_API_H__
#define __MARKET_OKX_REST_API_H__

#include <string>
#include "api.h"
#include <map>

namespace Market {

namespace Okx {

class RestApi : private API {
public:
  RestApi(const std::string& api_key, const std::string& secret_key,
    const std::string& passphrase);

private:
  std::map<std::string, std::string> prepare_headers(const std::string& method, const std::string& path);
};

} // namespace Okx

} // namespace Market

#endif