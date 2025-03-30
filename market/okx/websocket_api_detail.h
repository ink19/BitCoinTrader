#ifndef __MARKET_OKX_WEBSOCKET_API_DETAIL_H__
#define __MARKET_OKX_WEBSOCKET_API_DETAIL_H__

#include <boost/json.hpp>
#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <functional>
#include <memory>

namespace Market {

namespace Okx {

namespace Detail {

class RequestArgsParam {
 public:
  virtual boost::json::value Json() const = 0;
};

class RequestArgsParamLogin : public RequestArgsParam {
 public:
  RequestArgsParamLogin(const std::string& api_key, const std::string& passphrase, const std::string& sign,
                        const std::string& timestamp);
  boost::json::value Json() const override;

 private:
  std::string m_api_key;
  std::string m_passphrase;
  std::string m_sign;
  std::string m_timestamp;
};

class RequestArgsParamSubscribe : public RequestArgsParam {
 public:
  RequestArgsParamSubscribe(const std::string& channel = "", const std::string& inst_type = "",
                            const std::string& inst_family = "", const std::string& inst_id = "");

  boost::json::value Json() const override;

 private:
  std::string m_channel;
  std::string m_inst_id;
  std::string m_inst_type;
  std::string m_inst_family;
};

enum RequestOpEnum {
  OpNONE = 0,
  OpLOING = 1,
  OpSUBSCRIBE = 2,
  OpUNSUBSCRIBE = 3,
};

class RequestBody {
 public:
  RequestBody(RequestOpEnum op, std::vector<std::shared_ptr<RequestArgsParam>> args) : m_op(op), m_args(args) {}
  RequestBody(RequestOpEnum op, std::shared_ptr<RequestArgsParam> args) : m_op(op) { m_args.push_back(args); }

  boost::json::value Json() const;

 private:
  std::string op_string(RequestOpEnum op) const;

  RequestOpEnum m_op;
  std::vector<std::shared_ptr<RequestArgsParam>> m_args;
};

enum ResponeEventEnum {
  EventNONE = 0,
  EventLogin = 1,
  EventSubscribe = 2,
  EventUnSubscribe = 3,
  EventNotice = 4,
  EventError = 5,
};

enum RespontCodeEnum {
  CodeOK = 0,
};

class ResponeArgsParam {
  public:
    ResponeArgsParam() = default;
    ResponeArgsParam(ResponeEventEnum event) : m_event(event) {}
    ResponeEventEnum Type();
    static std::shared_ptr<ResponeArgsParam> Create(ResponeEventEnum event, const boost::json::value& data);
  private:
    ResponeEventEnum m_event;
};

class ResponeArgsParamSubscribe : public ResponeArgsParam {
  public:
    ResponeArgsParamSubscribe(const boost::json::value& data);
    
  private:
    std::string m_channel;
    std::string m_inst_id;
    std::string m_inst_type;
    std::string m_inst_family;
};

class ResponeBody {
  public:
    ResponeBody(const boost::json::value& data);
    ResponeBody(const std::string& data);
    ResponeEventEnum event() const { return m_event; }

    friend std::ostream& operator<<(std::ostream& os, const ResponeBody& respone) {
      os << respone.string();
      return os;
    }

    friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<ResponeBody> respone) {
      os << respone->string();
      return os;
    }

    std::string string() const;
  protected:
    ResponeEventEnum m_event;
    RespontCodeEnum m_code;
    std::string m_msg;
    std::string m_conn_id;
    std::vector<std::shared_ptr<ResponeArgsParam>> args;
    boost::json::value m_data;
    bool m_init = false;
    ResponeEventEnum event_transform(const std::string& event);
    void read_args(const boost::json::value& data);
    void read_args(const boost::json::array& data);
};

}  // namespace Detail

}  // namespace Okx

}  // namespace Market

#endif