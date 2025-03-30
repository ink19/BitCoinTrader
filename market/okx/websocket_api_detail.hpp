#ifndef __MARKET_OKX_WEBSOCKET_API_DETAIL_H__
#define __MARKET_OKX_WEBSOCKET_API_DETAIL_H__

#include <boost/json.hpp>
#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
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
                        const std::string& timestamp)
      : m_api_key(api_key), m_passphrase(passphrase), m_sign(sign), m_timestamp(timestamp) {}

  boost::json::value Json() const override {
    return {{"apiKey", m_api_key}, {"passphrase", m_passphrase}, {"sign", m_sign}, {"timestamp", m_timestamp}};
  }

 private:
  std::string m_api_key;
  std::string m_passphrase;
  std::string m_sign;
  std::string m_timestamp;
};

class RequestArgsParamSubscribe : public RequestArgsParam {
 public:
  RequestArgsParamSubscribe(const std::string& channel = "", const std::string& inst_type = "",
                            const std::string& inst_family = "", const std::string& inst_id = "")
      : m_channel(channel), m_inst_id(inst_id), m_inst_family(inst_family), m_inst_type(inst_type) {}

  boost::json::value Json() const override {
    boost::json::object arg;
    if (!m_inst_type.empty()) {
      arg["instType"] = m_inst_type;
    }

    if (!m_inst_family.empty()) {
      arg["instFamily"] = m_inst_family;
    }

    if (!m_inst_id.empty()) {
      arg["instId"] = m_inst_id;
    }

    if (!m_channel.empty()) {
      arg["channel"] = m_channel;
    }

    return arg;
  }

 private:
  std::string m_channel;
  std::string m_inst_id;
  std::string m_inst_type;
  std::string m_inst_family;
};

class RequestArgsParamUnSubscribe : public RequestArgsParam {
 public:
  RequestArgsParamUnSubscribe(const std::string& channel = "", const std::string& inst_type = "",
                              const std::string& inst_family = "", const std::string& inst_id = "")
      : m_channel(channel), m_inst_id(inst_id), m_inst_family(inst_family), m_inst_type(inst_type) {}

  boost::json::value Json() const override {
    boost::json::object arg;
    if (!m_inst_type.empty()) {
      arg["instType"] = m_inst_type;
    }

    if (!m_inst_family.empty()) {
      arg["instFamily"] = m_inst_family;
    }

    if (!m_inst_id.empty()) {
      arg["instId"] = m_inst_id;
    }

    if (!m_channel.empty()) {
      arg["channel"] = m_channel;
    }

    return arg;
  }

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

  boost::json::value Json() const {
    boost::json::array args_json;
    for (const auto& arg : m_args) {
      args_json.push_back(arg->Json());
    }
    return {{"op", op_string(m_op)}, {"args", args_json}};
  }

 private:
  std::string op_string(RequestOpEnum op) const {
    switch (op) {
      case OpNONE:
        return "none";
      case OpLOING:
        return "login";
      case OpSUBSCRIBE:
        return "subscribe";
      case OpUNSUBSCRIBE:
        return "unsubscribe";
      default:
        return "none";
    }
  }

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

class ResponeBody {
  public:
    ResponeBody(const boost::json::value& data) : m_data(data) {}
  
    ResponeEventEnum event() const { return m_event; }
    boost::json::value data() const { return m_data; }
  
  private:
    ResponeEventEnum m_event;
    boost::json::value m_data;

    ResponeEventEnum event_transform(const std::string& event) {
      if (event == "login") {
        return EventLogin;
      } else if (event == "subscribe") {
        return EventSubscribe;
      } else if (event == "unsubscribe") {
        return EventUnSubscribe;
      } else if (event == "notice") {
        return EventNotice;
      } else if (event == "error") {
        return EventError;
      }
      return EventNONE;
    }
};

}  // namespace Detail

}  // namespace Okx

}  // namespace Market

#endif