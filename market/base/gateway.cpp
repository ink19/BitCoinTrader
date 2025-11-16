#include "gateway.h"

namespace market::base {

Gateway::Gateway(EnginePtr engine, const std::string& name) : _engine(engine), _name(name) {}

Gateway::~Gateway() {}

asio::awaitable<void> Gateway::init() {
  _engine->register_callback<engine::QueryAccountData>(engine::EventType::kQueryAccount,
    std::bind(&Gateway::query_account, shared_from_this(), std::placeholders::_1));
  
  _engine->register_callback<engine::QueryPositionData>(engine::EventType::kQueryPosition,
    std::bind(&Gateway::query_position, shared_from_this(), std::placeholders::_1));
  
  _engine->register_callback<engine::QueryOrderData>(engine::EventType::kQueryOrder,
    std::bind(&Gateway::query_order, shared_from_this(), std::placeholders::_1));

  _engine->register_callback<engine::SubscribeData>(engine::EventType::kSubscribeBook,
    std::bind(&Gateway::subscribe_book, shared_from_this(), std::placeholders::_1));
  
  co_await market_init();
  co_return;
}

asio::awaitable<void> Gateway::on_tick(TickDataPtr tick) {
  return _engine->on_event(EventType::kTick, tick);
}

asio::awaitable<void> Gateway::on_position(PositionDataPtr position) {
  return _engine->on_event(EventType::kPosition, position);
}

asio::awaitable<void> Gateway::on_account(AccountDataPtr account) {
  return _engine->on_event(EventType::kAccount, account);
}

asio::awaitable<void> Gateway::on_book(BookPtr book) {
  return _engine->on_event(EventType::kBook, book);
}

asio::awaitable<void> Gateway::on_trade(TradeDataPtr trade) {
  return _engine->on_event(EventType::kTrade, trade);
}

asio::awaitable<void> Gateway::on_order(OrderDataPtr order) {
  return _engine->on_event(EventType::kOrder, order);
}

}
