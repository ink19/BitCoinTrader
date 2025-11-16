#include "stragy.h"

namespace stragy::base {

Stragy::Stragy(engine::EnginePtr engine) : _engine(engine) {
}

Stragy::~Stragy() {}

// 初始化策略，注册各类事件的回调函数
asio::awaitable<void> Stragy::init() {
  // 注册账户数据事件回调
  _engine->register_callback<engine::AccountData>(engine::EventType::kAccount,
    std::bind(&Stragy::recv_account, shared_from_this(), std::placeholders::_1));

  // 注册持仓数据事件回调
  _engine->register_callback<engine::PositionData>(engine::EventType::kPosition,
    std::bind(&Stragy::recv_position, shared_from_this(), std::placeholders::_1));

  // 注册订单簿数据事件回调
  _engine->register_callback<engine::Book>(engine::EventType::kBook,
    std::bind(&Stragy::recv_book, shared_from_this(), std::placeholders::_1));

  // 注册Tick数据事件回调
  _engine->register_callback<engine::TickData>(engine::EventType::kTick,
    std::bind(&Stragy::recv_tick, shared_from_this(), std::placeholders::_1));
  
  co_return;
}

asio::awaitable<void> Stragy::on_message(engine::MessageDataPtr msg) {
  return _engine->on_event(engine::EventType::kMessage, msg);
}

asio::awaitable<void> Stragy::on_request_account() {
  return _engine->on_event(engine::EventType::kQueryAccount, std::make_shared<engine::QueryAccountData>());
}

asio::awaitable<void> Stragy::on_request_position() {
  return _engine->on_event(engine::EventType::kQueryPosition, std::make_shared<engine::QueryPositionData>());
}

// 订阅指定交易对的订单簿数据
asio::awaitable<void> Stragy::on_subscribe_book(const std::string& symbol) {
  auto book = std::make_shared<engine::SubscribeData>();
  book->symbol = symbol;
  return _engine->on_event(engine::EventType::kSubscribeBook, book);
}

// 订阅指定交易对的Tick数据
asio::awaitable<void> Stragy::on_subscribe_tick(const std::string& symbol) {
  auto tick = std::make_shared<engine::SubscribeData>();
  tick->symbol = symbol;
  return _engine->on_event(engine::EventType::kSubscribeTick, tick);
}

// 发送订单
asio::awaitable<void> Stragy::on_send_order(engine::OrderDataPtr order) {
  return _engine->on_event(engine::EventType::kSendOrder, order);
}

}
