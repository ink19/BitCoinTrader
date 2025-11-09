#ifndef MARKET_OKX_DATA_H_
#define MARKET_OKX_DATA_H_

#include <string>
#include <utils/utils.h>

namespace market::okx {

template <typename T>
struct Respone {
  int code;
  std::string msg;
  T data;
};

struct AccountDetail {
  uint64_t uTime;
  std::string ccy;
  dec_float eq;
  dec_float cashBal;
  dec_float availBal;
};

struct Account {
  uint64_t uTime;
  dec_float totalEq;
  std::vector<AccountDetail> details;
};

typedef Respone<std::vector<Account>> AccountRespone;

struct PositionDetail {
  uint64_t uTime;
  std::string instType;
  std::string posId;
  std::string ccy;
  std::string posSide;

  dec_float pos;
  dec_float avgPx;
  dec_float pnl;
};

typedef Respone<std::vector<PositionDetail>> PositionRespone;

struct OrderDetail {
  uint64_t uTime;
  std::string instId;
  std::string ordId;

  dec_float px; // 委托价格
  dec_float sz; // 委托数量
  std::string side; // 委托方向
  dec_float accFillSz; // 已成交数量
  dec_float avgPx; // 平均成交价格
  std::string state; // 订单状态
};

typedef Respone<std::vector<OrderDetail>> OrderRespone;

}  // namespace market::okx

#endif  // MARKET_OKX_DATA_H_
