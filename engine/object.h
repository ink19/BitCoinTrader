#ifndef BITCOINTRADER_MARKET_BASE_OBJECT_H_
#define BITCOINTRADER_MARKET_BASE_OBJECT_H_

#include <memory>
#include <string>

#include "utils/utils.h"

namespace engine {

class BaseData : public std::enable_shared_from_this<BaseData> {
 public:
  virtual ~BaseData() = default;

  std::string symbol;
  std::string exchange;
  int64_t timestamp_ms;
};

enum class EventType {
  kQuit,  // 退出

  kSubscribeTick,  // 添加订阅
  kTick,

  kSubscribeBook,
  kBook,

  kSendTrade,
  kQueryOrder,
  kOrder,

  kTrade,

  kQueryPosition,
  kPosition,

  kQueryAccount,
  kAccount,

  kMessage,

  kAll,
};

class Event : public std::enable_shared_from_this<Event> {
 public:
  Event(EventType type, std::shared_ptr<const BaseData> data) {
    this->type = type;
    this->data = data;
  }
  EventType type;
  std::shared_ptr<const BaseData> data;
};

typedef std::shared_ptr<const Event> EventPtr;

class TickData;
typedef std::shared_ptr<const TickData> TickDataPtr;

// 订单簿项
class BookItem : public BaseData {
 public:
  dec_float price;
  dec_float volume;
};

// 订单簿
class Book : public BaseData {
 public:
  std::vector<BookItem> bids;
  std::vector<BookItem> asks;
  
  const static EventType type = EventType::kBook;
};

typedef std::shared_ptr<const Book> BookPtr;

// Tick数据
class TickData : public BaseData {
 public:
  dec_float last_price;   // 最新价
  dec_float last_volume;  // 成交量
  dec_float turnover;     // 成交额

  dec_float open_price;        // 开盘价
  dec_float high_price;        // 最高价
  dec_float low_price;         // 最低价
  dec_float last_close_price;  // 昨收价

  BookPtr order_book;
  TickDataPtr last_tick_data;

  const static EventType type = EventType::kTick;
};

// Bar数据
class BarData : public BaseData {
 public:
  int64_t interval;

  dec_float volume;  // 成交量

  dec_float open_price;   // 开盘价
  dec_float high_price;   // 最高价
  dec_float low_price;    // 最低价
  dec_float close_price;  // 昨收价
};

enum class Direction { BUY, SELL };

enum class OrderStatus {
  SUBMITTING,      // 提交中
  PENDING,         // 待成交
  PARTIAL_FILLED,  // 部分成交
  FILLED,          // 已成交
  CANCELLED,       // 已取消
  REJECTED,        // 已拒绝
};

class OrderDataItem : public BaseData {
 public:
  std::string order_id;

  Direction direction;
  dec_float price;
  dec_float volume;
  dec_float filled_volume;  // 已成交数量
  OrderStatus status;       // 订单状态
};

typedef std::shared_ptr<const OrderDataItem> OrderDataItemPtr;

class OrderData : public BaseData {
 public:
  std::vector<OrderDataItemPtr> items;
};

typedef std::shared_ptr<const OrderData> OrderDataPtr;

class TradeData : public BaseData {
 public:
  std::string trade_id;  // 成交ID

  Direction direction;
  dec_float price;
  dec_float volume;
  OrderDataPtr order;  // 订单

  const static EventType type = EventType::kTrade;
};

typedef std::shared_ptr<const TradeData> TradeDataPtr;

// 持仓数据
class PositionItem {
 public:
  std::string symbol;
  dec_float volume;  // 持仓数量
  Direction direction;
  dec_float frozen_volume;  // 冻结数量
  dec_float price;          // 持仓价格
  dec_float pnl;            // 持仓盈亏

  const static EventType type = EventType::kPosition;
};

typedef std::shared_ptr<const PositionItem> PositionItemPtr;

class PositionData : public BaseData {
 public:
  std::vector<PositionItemPtr> items;

  const static EventType type = EventType::kPosition;
};

typedef std::shared_ptr<const PositionData> PositionDataPtr;

class BalanceItem {
 public:
  std::string symbol;
  dec_float balance;         // 账户余额
  dec_float frozen_balance;  // 冻结余额

};

typedef std::shared_ptr<const BalanceItem> BalanceItemPtr;

class AccountData : public BaseData {
 public:
  std::string account_id;

  dec_float balance;         // 账户余额
  dec_float frozen_balance;  // 冻结余额

  std::vector<BalanceItemPtr> items;

  const static EventType type = EventType::kAccount;
};

typedef std::shared_ptr<const AccountData> AccountDataPtr;

class QueryAccountData : public BaseData {
 public:
  const static EventType type = EventType::kQueryAccount;
};

typedef std::shared_ptr<const QueryAccountData> QueryAccountDataPtr;

class QueryPositionData : public BaseData {
 public:
  const static EventType type = EventType::kQueryPosition;
};

typedef std::shared_ptr<const QueryPositionData> QueryPositionDataPtr;

class QueryOrderData : public BaseData {
 public:
  const static EventType type = EventType::kQueryOrder;
};

typedef std::shared_ptr<const QueryOrderData> QueryOrderDataPtr;

class MessageData : public BaseData {
 public:
  MessageData(const std::string& message) : message(message) {}
  std::string message;

  const static EventType type = EventType::kMessage;
};

typedef std::shared_ptr<const MessageData> MessageDataPtr;

class SubscribeData : public BaseData {
 public:

  const static EventType type = EventType::kSubscribeBook;
};

typedef std::shared_ptr<const SubscribeData> SubscribeDataPtr;

}  // namespace engine

#endif  // BITCOINTRADER_MARKET_BASE_OBJECT_H_
