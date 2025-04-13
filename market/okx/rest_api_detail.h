#ifndef __MARKET_OKX_REST_API_DETAIL_H__
#define __MARKET_OKX_REST_API_DETAIL_H__

#include <boost/json.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "component.hpp"

namespace Market {
namespace Okx {
namespace Detail {

namespace json = boost::json;
namespace mp = boost::multiprecision;
using dec_float = mp::cpp_dec_float_100;

enum RestResponeTypeEnum {
  RestResponeTypeEnum_NONE = 0,
  RestResponeTypeEnum_Account_Balance = 1,
};

class RestResponeData {
 public:
  RestResponeData() = default;
  RestResponeData(RestResponeTypeEnum respone_type);
  static std::shared_ptr<RestResponeData> CreateData(RestResponeTypeEnum respone_type, const json::value& data);
  virtual std::string string() const = 0;

  friend std::ostream& operator<<(std::ostream& os, const RestResponeData& respone) {
    os << respone.string();
    return os;
  }

  friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<RestResponeData> respone) {
    os << respone->string();
    return os;
  }

 private:
  RestResponeTypeEnum respone_type;
};

class RestResponeDataAccountBalanceCCYDetail {
 public:
  std::string ccy;        // 币种
  int64_t uTime;          // 币种信息的更新时间，Unix时间戳的毫秒数格式
  dec_float eq,           // 币种总权益
      cashBal,            // 币种余额
      isoEq,              // 币种逐仓仓位权益
      availEq,            // 可用保证金
      disEq,              // 美金层面币种折算权益
      fixedBal,           // 抄底宝、逃顶宝功能的币种冻结金额
      availBal,           // 可用余额
      frozenBal,          // 币种占用金额
      ordFrozen,          // 挂单冻结数量
      liab,               // 币种负债额
      upl,                // 币种未实现盈亏
      uplLiab,            // 由于仓位未实现亏损导致的负债
      crossLiab,          // 币种全仓负债额
      isoLiab,            // 币种逐仓负债额
      rewardBal,          // 体验金余额
      mgnRatio,           // 币种全仓保证金率，衡量账户内某项资产风险的指标
      imr,                // 币种维度全仓占用保证金
      mmr,                // 币种维度全仓维持保证金
      interest,           // 计息，应扣未扣利息
      maxLoan,            // 币种最大可借金额
      eqUsd,              // 币种权益折算成美金的金额
      borrowFroz,         // 币种美金层面潜在借币占用保证金
      notionalLever,      // 币种杠杆倍数
      stgyEq,             // 策略权益
      isoUpl,             // 逐仓未实现盈亏
      spotInUseAmt,       // 现货对冲占用数量
      clSpotInUseAmt,     // 用户自定义现货占用数量
      maxSpotInUse,       // 系统计算得到的最大可能现货占用数量
      spotIsoBal,         // 现货逐仓余额
      smtSyncEq,          // 合约智能跟单权益
      spotCopyTradingEq,  // 现货智能跟单权益
      spotBal,            // 	现货余额 ，单位为 币种
      openAvgPx,          // 现货开仓成本价 单位 USD
      accAvgPx,           // 现货累计成本价 单位 USD
      spotUpl,            // 现货未实现收益，单位 USD
      spotUplRatio,       // 	现货未实现收益率
      totalPnl;           // 现货累计收益，单位 USD

  double totalPnlRatio;    // 现货累计收益率
  bool collateralEnabled;  // 质押币/非质押币
  int twap;                // 当前负债币种触发系统自动换币的风险
             // 0、1、2、3、4、5其中之一，数字越大代表您的负债币种触发自动换币概率越高
};

class RestResponeDataAccountBalanceDetail {
 public:
  int64_t uTime;              // 账户信息的更新时间，Unix时间戳的毫秒数格式
  dec_float totalEq,          // 美金层面权益
      isoEq,                  // 美金层面逐仓仓位权益
      adjEq,                  // 美金层面有效保证金
      ordFroz,                // 美金层面全仓挂单占用保证金
      imr,                    // 美金层面占用保证金
      mmr,                    // 美金层面维持保证金
      borrowFroz,             // 美金层面借贷冻结
      mgnRatio,               // 美金层面保证金率
      notionalUsd,            // 以美金价值为单位的持仓数量，即仓位美金价值
      notionalUsdForBorrow,   // 借币金额（美元价值）
      notionalUsdForSwap,     // 永续合约持仓美元价值
      notionalUsdForFutures,  // 交割合约持仓美元价值
      notionalUsdForOption,   // 期权持仓美元价值
      upl;                    // 账户层面全仓未实现盈亏（美元单位）
  std::vector<RestResponeDataAccountBalanceCCYDetail> details;  // 币种信息
};

class RestResponeDataAccountBalance : public RestResponeData, public RestResponeDataAccountBalanceDetail {
 public:
  RestResponeDataAccountBalance() = default;
  RestResponeDataAccountBalance(const json::value& data);

  std::string string() const override;
  RestResponeDataAccountBalanceDetail base() const { return RestResponeDataAccountBalanceDetail(*this); }
};

typedef Common::StringEnum InstTypeEnum;
const auto InstTypeEnumNone = InstTypeEnum("");
const auto InstTypeEnumSPOT = InstTypeEnum("SPOT");        // 现货
const auto InstTypeEnumFUTURES = InstTypeEnum("FUTURES");  // 交割合约
const auto InstTypeEnumSWAP = InstTypeEnum("SWAP");        // 永续合约
const auto InstTypeEnumOPTION = InstTypeEnum("OPTION");    // 期权

typedef Common::StringEnum OptTypeEnum;
const auto OptTypeEnumNone = OptTypeEnum("");
const auto OptTypeEnumC = OptTypeEnum("C");  // 看涨期权
const auto OptTypeEnumP = OptTypeEnum("P");  // 看跌期权

typedef Common::StringEnum  CTTypeEnum;
const auto CTTypeEnumNone = CTTypeEnum("");
const auto CTTypeEnumLINEAR = CTTypeEnum("linear");  // 永续合约
const auto CTTypeEnumINVERSE = CTTypeEnum("inverse");  // 永续合约

typedef Common::StringEnum  InstStatusEnum;
const auto InstStatusEnumNone = InstStatusEnum("");
const auto InstStatusEnumLIVE = InstStatusEnum("live");  // 已创建
const auto InstStatusEnumSUSPEND = InstStatusEnum("suspend");  // 暂停中
const auto InstStatusEnumPREOPEN = InstStatusEnum("preopen");  // 预上线，交割和期权合约轮转生成到开始交易；部分交易产品上线前
const auto InstStatusEnumTest = InstStatusEnum("Test");  // 测试中（测试产品，不可交易）

typedef Common::StringEnum  InstRuleTypeEnum;
const auto InstRuleTypeEnumNone = InstRuleTypeEnum("");
const auto InstRuleTypeEnumNORMAL = InstRuleTypeEnum("normal");  // 普通交易
const auto InstRuleTypeEnumPREMARKET = InstRuleTypeEnum("pre_market");  // 盘前交易


class RestResponeDataAccountInstrumentDetail {
 public:
  InstTypeEnum instType;   // 产品类型
  std::string instId;      // 产品ID
  std::string uly;         // 合约标的
  std::string instFamily;  // 交易品种，如 BTC-USDT 中的 BTC ，仅适用于币币/币币杠杆
  std::string quoteCcy;    // 计价货币币种，如 BTC-USDT 中的USDT ，仅适用于币币/币币杠杆
  std::string settleCcy;   // 盈亏结算和保证金币种，如 BTC 仅适用于交割/永续/期权
  dec_float ctVal;         // 合约面值，仅适用于交割/永续/期权
  dec_float ctMult;        // 合约乘数，仅适用于交割/永续/期权
  std::string ctValCcy;    // 合约面值计价币种，仅适用于交割/永续/期权
  OptTypeEnum optType;     // 期权类型，C或P 仅适用于期权
  dec_float stk; // 行权价格，仅适用于期权
  uint64_t listTime; // 产品上市时间，Unix时间戳的毫秒数格式
  uint64_t auctionEndTime; // 集合竞价结束时间，Unix时间戳的毫秒数格式，仅适用于通过集合竞价方式上线的币币，其余情况返回""
  uint64_t expTime; // 产品下线时间
  dec_float lever; // 杠杆倍数
  dec_float tickSz; // 下单价格精度
  dec_float lotSz; // 下单数量精度，合约的数量单位是张，现货的数量单位是交易货币
  dec_float minSz; // 最小下单数量
  CTTypeEnum ctType; // 合约类型
  InstStatusEnum status; // 产品状态
  InstRuleTypeEnum ruleType; // 产品规则类型
  dec_float maxLmtSz; // 限价单的单笔最大委托数量
  dec_float maxMktSz; // 市价单的单笔最大委托数量
  dec_float maxLmtAmt; // 限价单的单笔最大美元价值
  dec_float maxMktAmt; // 市价单的单笔最大美元价值
  dec_float maxTwapSz; // 时间加权单的单笔最大委托数量
  dec_float maxIcebergSz; // 冰山委托的单笔最大委托数量
  dec_float maxTriggerSz;// 计划委托委托的单笔最大委托数量
  dec_float maxStopSz;// 止盈止损市价委托的单笔最大委托数量
  bool futureSettlement;// 交割合约是否支持每日结算
};

class RestRespone {
 public:
  RestRespone() = default;
  RestRespone(const json::value& data);

  int code;
  std::string msg;
  std::vector<std::shared_ptr<RestResponeData>> data;
};

}  // namespace Detail
}  // namespace Okx
}  // namespace Market

#endif
