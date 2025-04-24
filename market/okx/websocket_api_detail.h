#ifndef __MARKET_OKX_WEBSOCKET_API_DETAIL_H__
#define __MARKET_OKX_WEBSOCKET_API_DETAIL_H__

#include <boost/json.hpp>
#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <functional>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <memory>

namespace Market {

namespace Okx {

namespace Detail {

namespace mp = boost::multiprecision;
using dec_float = mp::cpp_dec_float_100;
  

class WsRequestArgsParam {
 public:
  virtual boost::json::value Json() const = 0;
};

class WsRequestArgsParamLogin : public WsRequestArgsParam {
 public:
  WsRequestArgsParamLogin(const std::string& api_key, const std::string& passphrase, const std::string& sign,
                        const std::string& timestamp);
  boost::json::value Json() const override;

 private:
  std::string m_api_key;
  std::string m_passphrase;
  std::string m_sign;
  std::string m_timestamp;
};

class WsRequestArgsParamSubscribe : public WsRequestArgsParam {
 public:
  WsRequestArgsParamSubscribe(const std::string& channel = "", const std::string& inst_type = "",
                            const std::string& inst_family = "", const std::string& inst_id = "");

  boost::json::value Json() const override;

 private:
  std::string m_channel;
  std::string m_inst_id;
  std::string m_inst_type;
  std::string m_inst_family;
};

enum WsRequestOpEnum {
  OpNONE = 0,
  OpLOING = 1,
  OpSUBSCRIBE = 2,
  OpUNSUBSCRIBE = 3,
};

class WsRequestBody {
 public:
  WsRequestBody(WsRequestOpEnum op, std::vector<std::shared_ptr<WsRequestArgsParam>> args) : m_op(op), m_args(args) {}
  WsRequestBody(WsRequestOpEnum op, std::shared_ptr<WsRequestArgsParam> args) : m_op(op) { m_args.push_back(args); }

  boost::json::value Json() const;

 private:
  std::string op_string(WsRequestOpEnum op) const;

  WsRequestOpEnum m_op;
  std::vector<std::shared_ptr<WsRequestArgsParam>> m_args;
};

enum WsResponeEventEnum {
  EventNONE = 0,
  EventLogin = 1,
  EventSubscribe = 2,
  EventUnSubscribe = 3,
  EventNotice = 4,
  EventError = 5,
};

enum WsResponeCodeEnum {
  CodeOK = 0,
};

class WsResponeArgsParam {
  public:
    WsResponeArgsParam() = default;
    WsResponeArgsParam(WsResponeEventEnum event) : m_event(event) {}
    WsResponeEventEnum Type();
    static std::shared_ptr<WsResponeArgsParam> Create(WsResponeEventEnum event, const boost::json::value& data);
  private:
    WsResponeEventEnum m_event;
};

class WsResponeArgsParamSubscribe : public WsResponeArgsParam {
  public:
    WsResponeArgsParamSubscribe(const boost::json::value& data);
    
  private:
    std::string m_channel;
    std::string m_inst_id;
    std::string m_inst_type;
    std::string m_inst_family;
};

class WsResponeBody {
  public:
    WsResponeBody(const boost::json::value& data);
    WsResponeBody(const std::string& data);
    WsResponeEventEnum event() const { return m_event; }

    friend std::ostream& operator<<(std::ostream& os, const WsResponeBody& respone) {
      os << respone.string();
      return os;
    }

    friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<WsResponeBody> respone) {
      os << respone->string();
      return os;
    }

    std::string string() const;
  protected:
    WsResponeEventEnum m_event;
    WsResponeCodeEnum m_code;
    std::string m_msg;
    std::string m_conn_id;
    std::vector<std::shared_ptr<WsResponeArgsParam>> args;
    boost::json::value m_data;
    bool m_init = false;
    WsResponeEventEnum event_transform(const std::string& event);
    void read_args(const boost::json::value& data);
    void read_args(const boost::json::array& data);
};

class WsResponeSubscribeArg {
public:
  std::string channel;
  std::string instId;
};

class WsResponeSubscribeData {
public:
  std::string instId;
  std::string tradeId;
  std::string side;
  dec_float px,
  sz,
  count;
  int64_t ts;
};

class WsResponeSubscribe {
public:
  WsResponeSubscribeArg arg;
  std::vector<WsResponeSubscribeData> data;
};

}  // namespace Detail

}  // namespace Okx

}  // namespace Market

#endif
