#include "gateway.h"

namespace market::base {

Gateway::Gateway(EnginePtr engine) : _engine(engine) {}

Gateway::~Gateway() {}


asio::awaitable<void> Gateway::on_tick(OrderDataPtr order) {
  return _engine->on_event(EventType::kTick, order);
}

asio::awaitable<void> Gateway::on_position(PositionDataPtr position) {
  return _engine->on_event(EventType::kPosition, position);
}

asio::awaitable<void> Gateway::on_account(AccountDataPtr account) {
  return _engine->on_event(EventType::kAccount, account);
}

asio::awaitable<void> Gateway::on_log(LogDataPtr log) {
  return _engine->on_event(EventType::kLog, log);
}

asio::awaitable<void> Gateway::on_order(OrderDataPtr order) {
  return _engine->on_event(EventType::kOrderBook, order);
}

asio::awaitable<void> Gateway::on_trade(TradeDataPtr trade) {
  return _engine->on_event(EventType::kTrade, trade);
}

}
