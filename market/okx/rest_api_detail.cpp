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
  :RestResponeDataAccountBalanceDetail(Common::DataReader<RestResponeDataAccountBalanceDetail>::read(data.as_object())) {
  LOG(INFO) << Common::DataPrinter(this->base());
}

std::string RestResponeDataAccountBalance::string() const { return fmt::format("uTime: {}", uTime); }

RestRespone::RestRespone(const json::value& jdata) {
  if (!jdata.is_object()) {
    throw bs::system_error(bs::error_code(static_cast<int>(ErrCode_Invalid_Param), bs::generic_category()),
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
      throw bs::system_error(bs::error_code(static_cast<int>(ErrCode_Invalid_Param), bs::generic_category()),
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
    default:
      throw bs::system_error(bs::error_code(static_cast<int>(ErrCode_Invalid_Param), bs::generic_category()),
                             "Invalid respone type");
  }
}

}  // namespace Detail
}  // namespace Okx
}  // namespace Market