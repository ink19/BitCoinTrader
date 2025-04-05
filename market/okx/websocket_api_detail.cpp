#include "websocket_api_detail.h"
#include <glog/logging.h>
#include <string>
#include <fmt/core.h>

Market::Okx::Detail::WsRequestArgsParamLogin::WsRequestArgsParamLogin(const std::string& api_key, const std::string& passphrase, const std::string& sign,
  const std::string& timestamp)
: m_api_key(api_key), m_passphrase(passphrase), m_sign(sign), m_timestamp(timestamp) {}

boost::json::value Market::Okx::Detail::WsRequestArgsParamLogin::Json() const {
  return {{"apiKey", m_api_key}, {"passphrase", m_passphrase}, {"sign", m_sign}, {"timestamp", m_timestamp}};
}

Market::Okx::Detail::WsRequestArgsParamSubscribe::WsRequestArgsParamSubscribe(const std::string& channel, const std::string& inst_type,
  const std::string& inst_family, const std::string& inst_id)
: m_channel(channel), m_inst_id(inst_id), m_inst_family(inst_family), m_inst_type(inst_type) {}

boost::json::value Market::Okx::Detail::WsRequestArgsParamSubscribe::Json() const {
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

boost::json::value Market::Okx::Detail::WsRequestBody::Json() const {
  boost::json::array args_json;
  for (const auto& arg : m_args) {
    args_json.push_back(arg->Json());
  }
  return {{"op", op_string(m_op)}, {"args", args_json}};
}

std::string Market::Okx::Detail::WsRequestBody::op_string(WsRequestOpEnum op) const {
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

Market::Okx::Detail::WsResponeEventEnum Market::Okx::Detail::WsResponeBody::event_transform(const std::string& event) {
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

Market::Okx::Detail::WsResponeBody::WsResponeBody(const std::string& data) : WsResponeBody(boost::json::parse(data)) {}


Market::Okx::Detail::WsResponeBody::WsResponeBody(const boost::json::value& data) {
  try {
    if (data.as_object().contains("event")) {
      m_event = event_transform(data.at("event").as_string().c_str());
    } else {
      m_event = EventNONE;
    }

    if (data.as_object().contains("code")) {
      std::string code_str = data.at("code").as_string().data();
      if (code_str == "0") {
        m_code = CodeOK;
      } else {
        m_code = static_cast<WsResponeCodeEnum>(std::stoi(code_str));
      }
    } else {
      m_code = CodeOK;
    }

    if (data.as_object().contains("msg")) {
      m_msg = data.at("msg").as_string().c_str();
    } else {
      m_msg = "";
    }

    if (data.as_object().contains("connId")) {
      m_conn_id = data.at("connId").as_string().c_str();
    } else {
      m_conn_id = "";
    }

    m_data = data;
    m_init = true;
  } catch (const std::exception& e) {
    LOG(ERROR) << "Error parsing JSON: " << e.what();
  }
}

void Market::Okx::Detail::WsResponeBody::read_args(const boost::json::value& data) {
  if (data.is_array()) {
    read_args(data);
  }

  if (data.is_object()) {
    auto shared_arg = WsResponeArgsParam::Create(m_event, data);
  }
}

void Market::Okx::Detail::WsResponeBody::read_args(const boost::json::array& data) {
  for (const auto& item : data) {
    read_args(item);
  }
}

Market::Okx::Detail::WsResponeArgsParamSubscribe::WsResponeArgsParamSubscribe(const boost::json::value& data) : WsResponeArgsParam(EventSubscribe) {
  std::string channel, inst_id, inst_type, inst_family;

  if (data.as_object().contains("channel")) {
    channel = data.at("channel").as_string().c_str();
  } else {
    channel = "";
  }

  if (data.as_object().contains("instId")) {
    inst_id = data.at("instId").as_string().c_str();
  } else {
    inst_id = "";
  }

  if (data.as_object().contains("instType")) {
    inst_type = data.at("instType").as_string().c_str();
  } else {
    inst_type = "";
  }

  if (data.as_object().contains("instFamily")) {
    inst_family = data.at("instFamily").as_string().c_str();
  } else {
    inst_family = "";
  }
  
  m_channel = channel;
  m_inst_id = inst_id;
  m_inst_type = inst_type;
  m_inst_family = inst_family;
}

std::shared_ptr<Market::Okx::Detail::WsResponeArgsParam> Market::Okx::Detail::WsResponeArgsParam::Create(WsResponeEventEnum event, const boost::json::value& data) {
  switch (event) {
    case EventSubscribe:
      return std::make_shared<WsResponeArgsParamSubscribe>(data);
    case EventUnSubscribe:
      return std::make_shared<WsResponeArgsParamSubscribe>(event);
    default:
      return nullptr;
  }
}

std::string Market::Okx::Detail::WsResponeBody::string() const {
  auto result = fmt::format("Event: {}, Code: {}, Msg: {}, ConnId: {}", int(m_event), int(m_code), m_msg, m_conn_id);
  return result;
}
