#ifndef __MARKET_MARKET_H__
#define __MARKET_MARKET_H__

#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>

namespace Market {

using dec_float = boost::multiprecision::cpp_dec_float_100;

class TradeData {
public:
  std::string inst_id;
  std::string trade_id;
  std::string side;
  dec_float px, sz, count;
  int64_t ts;
};

}

#endif
