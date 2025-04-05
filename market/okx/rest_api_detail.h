#ifndef __MARKET_OKX_REST_API_DETAIL_H__
#define __MARKET_OKX_REST_API_DETAIL_H__

#include <string>
#include <vector>

namespace Market {
namespace OKX {
namespace Detail {

class RestResponeData {

};


class Respone {
 public:
  int code;
  std::string msg;
  std::vector<RestResponeData> data;
};


}  // namespace Detail
}  // namespace OKX
}  // namespace Market

#endif