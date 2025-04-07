#ifndef __COMMON_ERRCODE_H
#define __COMMON_ERRCODE_H

#include <boost/system.hpp>

enum ErrCode {
    ErrCode_OK = 0,
    ErrCode_Invalid_Param = 1,
    ErrCode_Resolve_Fail = 2,
    ErrCode_API_Return_Error = 3,
    ErrCode_SSL_ERROR = 4,
};

static std::string GetErrCodeString(ErrCode err) {
  switch (err) {
    case ErrCode_OK:
      return "OK";
    case ErrCode_Invalid_Param:
      return "Invalid Param";
    case ErrCode_Resolve_Fail:
      return "Resolve Fail";
    case ErrCode_API_Return_Error:
      return "API Return Error";
    case ErrCode_SSL_ERROR:
      return "SSL Error";
    default:
      return "Unknown Error";
  }
}

// 市场通信错误
class MarketErrorCategory : public boost::system::error_category {
public:
  const char* name() const noexcept override {
    return "MarketError";
  }

  std::string message(int ev) const override {
    return GetErrCodeString(static_cast<ErrCode>(ev));
  }
};

// 网络层错误
class RequestErrorCategory : public boost::system::error_category {
public:
  const char* name() const noexcept override {
    return "RequestError";
  }

  std::string message(int ev) const override {
    return GetErrCodeString(static_cast<ErrCode>(ev));
  }
};

#endif
