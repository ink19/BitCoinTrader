#include "rest_api_detail.h"

#include <fmt/format.h>

#include <boost/system.hpp>
#include <boost/system/system_error.hpp>
#include <cstdlib>

#include "errcode.h"
#include "component.hpp"

namespace Market {
namespace Okx {
namespace Detail {

namespace bs = boost::system;

RestResponeData::RestResponeData(RestResponeTypeEnum respone_type) : respone_type(respone_type) {}

RestResponeDataAccountBalance::RestResponeDataAccountBalance(const json::value& data) 
  // :RestResponeDataAccountBalanceDetail(Common::DataReader<RestResponeDataAccountBalanceDetail>::read(data.as_object()))
{
}

std::string RestResponeDataAccountBalance::string() const {
  std::stringstream ss;
  // ss << Common::DataPrinter(this->base());
  return ss.str();
}

RestResponeDataAccountInstrument::RestResponeDataAccountInstrument(const json::value& data) 
  // :RestResponeDataAccountInstrumentDetail(Common::DataReader<RestResponeDataAccountInstrumentDetail>::read(data.as_object()))
{
}

std::string RestResponeDataAccountInstrument::string() const {
  std::stringstream ss;
  // ss << Common::DataPrinter(this->base());
  return ss.str();
}


RestRespone::RestRespone(const json::value& jdata) {
  if (!jdata.is_object()) {
    throw bs::system_error(bs::error_code(static_cast<int>(ErrCode::Invalid_Param), MarketErrorCategory()),
                           "Invalid data");
  }
  auto odata = jdata.as_object();
  if (odata.contains("code")) {
    auto sCode = odata["code"];
    code = std::atoi(sCode.as_string().c_str());
  }
  if (odata.contains("msg")) {
    auto sMsg = odata["msg"];
    msg = sMsg.as_string().c_str();
  }
  if (odata.contains("data")) {
    auto sData = odata["data"];
    if (sData.is_array()) {
      for (const auto& item : sData.as_array()) {
        data.push_back(RestResponeData::CreateData(RestResponeTypeEnum_Account_Balance, item));
      }
    } else {
      throw bs::system_error(bs::error_code(static_cast<int>(ErrCode::Invalid_Param), MarketErrorCategory()),
                             "Invalid data");
    }
  }
}

std::shared_ptr<RestResponeData> RestResponeData::CreateData(RestResponeTypeEnum respone_type,
                                                             const json::value& data) {
  switch (respone_type) {
    case RestResponeTypeEnum_Account_Balance: {
      return std::make_shared<RestResponeDataAccountBalance>(data);
    }
    case RestResponeTypeEnum_Account_Instrument: {
      return std::make_shared<RestResponeDataAccountInstrument>(data);
    }
    default:
      throw bs::system_error(bs::error_code(static_cast<int>(ErrCode::Invalid_Param), MarketErrorCategory()),
                             "Invalid respone type");
  }
}

}  // namespace Detail
}  // namespace Okx
}  // namespace Market
