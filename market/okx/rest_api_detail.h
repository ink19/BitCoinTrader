#ifndef __MARKET_OKX_REST_API_DETAIL_H__
#define __MARKET_OKX_REST_API_DETAIL_H__

#include <boost/json.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

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
  RestResponeDataAccountBalanceCCYDetail() = default;
  RestResponeDataAccountBalanceCCYDetail(const json::value& data);
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

class RestResponeDataAccountBalance : public RestResponeData {
 public:
  RestResponeDataAccountBalance() = default;
  RestResponeDataAccountBalance(const json::value& data);
  int64_t uTime;              // 	账户信息的更新时间，Unix时间戳的毫秒数格式
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
  std::vector<RestResponeDataAccountBalanceCCYDetail> ccy;  // 币种信息
  std::string string() const override;
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