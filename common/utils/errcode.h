#ifndef __COMMON_H
#define __COMMON_H

#include <boost/system.hpp>

enum class ErrCode {
  OK = 0,
  Invalid_Param = -1,
  Resolve_Fail = -2,
  API_Return_Error = -3,
  SSL_ERROR = -4,
  Deserialize_Fail = -5,
};

static std::string GetErrCodeString(ErrCode err) {
  switch (err) {
    case ErrCode::OK:
      return "OK";
    case ErrCode::Invalid_Param:
      return "Invalid Param";
    case ErrCode::Resolve_Fail:
      return "Resolve Fail";
    case ErrCode::API_Return_Error:
      return "API Return Error";
    case ErrCode::SSL_ERROR:
      return "SSL Error";
    default:
      return "Unknown Error";
  }
}

// 市场通信错误
class MarketErrorCategory : public boost::system::error_category {
 public:
  const char* name() const noexcept override { return "MarketError"; }

  std::string message(int ev) const override { return GetErrCodeString(static_cast<ErrCode>(ev)); }
};

// 网络层错误
class RequestErrorCategory : public boost::system::error_category {
 public:
  const char* name() const noexcept override { return "RequestError"; }

  std::string message(int ev) const override { return GetErrCodeString(static_cast<ErrCode>(ev)); }
};

#endif
