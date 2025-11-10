#include "gateway.h"

namespace market::base {

Gateway::Gateway(EnginePtr engine, const std::string& name) : _engine(engine), _name(name) {}

Gateway::~Gateway() {}

void Gateway::init() {
  _engine->register_callback<engine::QueryAccountData>(engine::EventType::kQueryAccount,
    std::bind(&Gateway::query_account, shared_from_this(), std::placeholders::_1));
  _engine->register_callback<engine::QueryPositionData>(engine::EventType::kQueryPosition,
    std::bind(&Gateway::query_position, shared_from_this(), std::placeholders::_1));
  _engine->register_callback<engine::QueryOrderData>(engine::EventType::kQueryOrder,
    std::bind(&Gateway::query_order, shared_from_this(), std::placeholders::_1));
}

asio::awaitable<void> Gateway::on_tick(OrderDataPtr order) {
  return _engine->on_event(EventType::kTick, order);
}

asio::awaitable<void> Gateway::on_position(PositionDataPtr position) {
  return _engine->on_event(EventType::kPosition, position);
}

asio::awaitable<void> Gateway::on_account(AccountDataPtr account) {
  return _engine->on_event(EventType::kAccount, account);
}

asio::awaitable<void> Gateway::on_order(OrderDataPtr order) {
  return _engine->on_event(EventType::kOrderBook, order);
}

asio::awaitable<void> Gateway::on_trade(TradeDataPtr trade) {
  return _engine->on_event(EventType::kTrade, trade);
}

}
