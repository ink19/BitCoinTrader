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
  kTick,
  kOrderBook,
  kTrade,
  kPosition,
  kAccount,
  kLog,
  kMessage,
  kAll,
};

class Event : public std::enable_shared_from_this<Event>{
public:
  Event(EventType type, std::shared_ptr<BaseData> data) {
    this->type = type;
    this->data = data;
  }
  EventType type;
  std::shared_ptr<BaseData> data;
};

typedef std::shared_ptr<Event> EventPtr;


class TickData;
typedef std::shared_ptr<TickData> TickDataPtr;

// 订单簿项
class OrderBookItem : public BaseData {
 public:
  const dec_float price;
  const dec_float volume;
};

typedef std::shared_ptr<OrderBookItem> OrderBookItemPtr;

// 订单簿
class OrderBook : public BaseData {
 public:
  std::vector<OrderBookItemPtr> bids;
  std::vector<OrderBookItemPtr> asks;
};

typedef std::shared_ptr<OrderBook> OrderBookPtr;

// 市场数据
class MarketData : public BaseData {
 public:
  dec_float last_price;   // 最新价
  dec_float last_volume;  // 成交量
  dec_float turnover;     // 成交额

  dec_float open_price;        // 开盘价
  dec_float high_price;        // 最高价
  dec_float low_price;         // 最低价
  dec_float last_close_price;  // 昨收价

  OrderBookPtr order_book;
  TickDataPtr last_tick_data;
};

typedef std::shared_ptr<MarketData> MarketDataPtr;

// Tick数据
class TickData : public BaseData {
 public:
  dec_float price;            // 成交价
  dec_float volume;           // 成交量
  MarketDataPtr market_data;  // 市场数据
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

class OrderData : public BaseData {
 public:
  std::string order_id;

  Direction direction;
  dec_float price;
  dec_float volume;
  dec_float filled_volume;  // 已成交数量
  OrderStatus status; // 订单状态
};

typedef std::shared_ptr<OrderData> OrderDataPtr;

class TradeData : public BaseData {
 public:
  std::string trade_id; // 成交ID

  Direction direction;
  dec_float price;
  dec_float volume;
  OrderDataPtr order;  // 订单
};

typedef std::shared_ptr<TradeData> TradeDataPtr;

// 持仓数据
class PositionData : public BaseData {
 public:
  dec_float volume;  // 持仓数量
  Direction direction;
  dec_float frozen_volume;  // 冻结数量
  dec_float price;  // 持仓价格
  dec_float pnl;  // 持仓盈亏
};

typedef std::shared_ptr<PositionData> PositionDataPtr;

class AccountData : public BaseData {
 public:
  std::string account_id;

  dec_float balance;  // 账户余额
  dec_float frozen_balance;  // 冻结余额
};

typedef std::shared_ptr<AccountData> AccountDataPtr;

enum class LogLevel {
  kError,
  kWarning,
  kInfo,
  kDebug,
};

class LogData : public BaseData {
public:
  LogData(LogLevel level, const std::string& log) : level(level), log(log) {}
  std::string log;
  LogLevel level;
  
};

typedef std::shared_ptr<LogData> LogDataPtr;

class MessageData : public BaseData {
public:
  MessageData(const std::string& message) : message(message) {}
  std::string message;
};

typedef std::shared_ptr<MessageData> MessageDataPtr;


}  // namespace market

#endif  // BITCOINTRADER_MARKET_BASE_OBJECT_H_
