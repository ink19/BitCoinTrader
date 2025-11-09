#ifndef _BITCONITRADER_MARKET_OKX_OKX_H
#define _BITCONITRADER_MARKET_OKX_OKX_H

#include <string>

#include "base/gateway.h"
#include "engine.h"
#include "okx_http.h"

namespace market::okx {

class Okx : public base::Gateway {
 public:
  Okx(engine::EnginePtr engine, std::string api_key, std::string secret_key, std::string passphrase);
  ~Okx() {}

  void connect() override{};
  void close() override{};

  asio::awaitable<void> run() override;

  // 订阅行情
  void subscribe(const std::string& symbol) override{};
  void unsubscribe(const std::string& symbol) override{};

  // 下单
  void send_order(engine::OrderDataPtr order) override{};
  void cancel_order(engine::OrderDataPtr order) override{};

  // 查询账户
  asio::awaitable<void> query_account(engine::QueryAccountDataPtr data) override;

  // 查询持仓
  asio::awaitable<void> query_position(engine::QueryPositionDataPtr data) override;

  // 查询历史订单
  asio::awaitable<void> query_order(engine::QueryOrderDataPtr data) override;

 private:
  OkxHttp http_;
};

}  // namespace market::okx

#endif  // _BITCONITRADER_MARKET_OKX_OKX_H
