#ifndef __MARKET_BASE_GATEWAY_H__
#define __MARKET_BASE_GATEWAY_H__

#include <memory>

#include "engine.h"
#include "object.h"

namespace market::base {

using namespace engine;

// 通用交易网关
class Gateway : public engine::Component, public std::enable_shared_from_this<Gateway> {
public:
  Gateway(engine::EnginePtr engine, const std::string& name);
  virtual ~Gateway();

  asio::awaitable<void> init() override;

  // name 返回交易网关名称
  std::string name() const { return _name; }

  // on_tick tick更新
  asio::awaitable<void> on_tick(TickDataPtr tick);

  // on_order 订单
  asio::awaitable<void> on_order(OrderDataPtr order);

  // on_position 持仓更新
  asio::awaitable<void> on_position(PositionDataPtr position);

  // on_account 账户更新
  asio::awaitable<void> on_account(AccountDataPtr account);

  // on_book 订单更新
  asio::awaitable<void> on_book(BookPtr book);

  // on_trade 成交更新
  asio::awaitable<void> on_trade(TradeDataPtr trade);

  virtual void connect() = 0;
  virtual void close() = 0;

  // 订阅行情
  virtual void subscribe(const std::string& symbol) = 0;
  virtual void unsubscribe(const std::string& symbol) = 0;

  // 下单
  virtual void send_order(OrderDataPtr order) = 0;
  virtual void cancel_order(OrderDataPtr order) = 0;

  // 查询账户
  virtual asio::awaitable<void> query_account(engine::QueryAccountDataPtr data) = 0;

  // 查询持仓
  virtual asio::awaitable<void> query_position(engine::QueryPositionDataPtr data) = 0;

  // 查询历史订单
  virtual asio::awaitable<void> query_order(engine::QueryOrderDataPtr data) = 0;

  // 订阅book
  virtual asio::awaitable<void> subscribe_book(engine::SubscribeDataPtr data) = 0;

  // 订阅ticker
  virtual asio::awaitable<void> subscribe_tick(engine::SubscribeDataPtr data) = 0;

  virtual asio::awaitable<void> market_init() = 0;
private:
  const std::string _name;
  EnginePtr _engine;
};

}  // namespace market::base

#endif  // __MARKET_BASE_GATEWAY_H__
