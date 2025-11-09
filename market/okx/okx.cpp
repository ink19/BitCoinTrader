#include "okx.h"
#include "config/config.h"

namespace market::okx {

Okx::Okx(engine::EnginePtr engine, std::string api_key, std::string secret_key, std::string passphrase)
    : base::Gateway(engine, "okx"), http_(api_key, secret_key, passphrase)
{
  
}

asio::awaitable<void> Okx::query_account(engine::QueryAccountDataPtr data) {
  auto account = co_await http_.get_account();
  
  auto account_data = std::make_shared<engine::AccountData>();
  account_data->balance = account.totalEq;
  account_data->exchange = name();
  account_data->timestamp_ms = account.uTime;

  for (auto& item : account.details) {
    auto balance_item = std::make_shared<engine::BalanceItem>();
    balance_item->symbol = item.ccy;
    balance_item->balance = item.eq;
    account_data->items.push_back(balance_item);
  }

  co_await on_account(account_data);
  co_return;
}

asio::awaitable<void> Okx::query_position(engine::QueryPositionDataPtr data) {
  auto positions = co_await http_.get_positions();
  auto position_data = std::make_shared<engine::PositionData>();
  position_data->exchange = name();

  if (positions.empty()) {
    co_await on_position(position_data);
    co_return;
  }

  position_data->timestamp_ms = positions[0].uTime;

  for (auto& pos_item : positions) {
    auto item = std::make_shared<engine::PositionItem>();
    item->symbol = pos_item.ccy;
    item->volume = pos_item.pos;
    item->price = pos_item.avgPx;
    item->pnl = pos_item.pnl;

    pos_item.posSide == "long" ? item->direction = engine::Direction::BUY : item->direction = engine::Direction::SELL;

    position_data->items.push_back(item);
  }

  co_await on_position(position_data);

  co_return;
}

asio::awaitable<void> Okx::query_order(engine::QueryOrderDataPtr data) {
  auto orders = co_await http_.get_orders();

  auto orders_data = std::make_shared<engine::OrderData>();

  for (auto& order : orders) {
    auto order_item = std::make_shared<engine::OrderDataItem>();
    order_item->order_id = order.ordId;
    order_item->direction = order.side == "buy" ? engine::Direction::BUY : engine::Direction::SELL;
    order_item->price = order.px;
    order_item->volume = order.sz;
    order_item->filled_volume = order.accFillSz;
    order_item->status = order.state == "live" ? engine::OrderStatus::PENDING : engine::OrderStatus::PARTIAL_FILLED;

    orders_data->items.push_back(order_item);
  }

  co_await on_order(orders_data);
  co_return;
}

asio::awaitable<void> Okx::run() {
  co_return;
}

};  // namespace market::okx
